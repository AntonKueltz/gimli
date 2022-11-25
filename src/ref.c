#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define WORDS 12

// reference implementation from https://gimli.cr.yp.to/spec.html
#include <stdint.h>

uint32_t rotate(uint32_t x, int bits)
{
  if (bits == 0) return x;
  return (x << bits) | (x >> (32 - bits));
}

extern void gimli(uint32_t *state)
{
  int round;
  int column;
  uint32_t x;
  uint32_t y;
  uint32_t z;

  for (round = 24; round > 0; --round)
  {
    for (column = 0; column < 4; ++column)
    {
      x = rotate(state[    column], 24);
      y = rotate(state[4 + column],  9);
      z =        state[8 + column];

      state[8 + column] = x ^ (z << 1) ^ ((y&z) << 2);
      state[4 + column] = y ^ x        ^ ((x|z) << 1);
      state[column]     = z ^ y        ^ ((x&y) << 3);
    }

    if ((round & 3) == 0) { // small swap: pattern s...s...s... etc.
      x = state[0];
      state[0] = state[1];
      state[1] = x;
      x = state[2];
      state[2] = state[3];
      state[3] = x;
    }
    if ((round & 3) == 2) { // big swap: pattern ..S...S...S. etc.
      x = state[0];
      state[0] = state[2];
      state[2] = x;
      x = state[1];
      state[1] = state[3];
      state[3] = x;
    }

    if ((round & 3) == 0) { // add constant: pattern c...c...c... etc.
      state[0] ^= (0x9e377900 | round);
    }
  }
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define rateInBytes 16

void Gimli_hash(const uint8_t *input,
                uint64_t inputByteLen,
                uint8_t *output,
                uint64_t outputByteLen)
{
    uint32_t state[12];
    uint8_t* state_8 = (uint8_t*)state;
    uint64_t blockSize = 0;
    uint64_t i;

    // === Initialize the state ===
    memset(state, 0, sizeof(state));

    // === Absorb all the input blocks ===
    while(inputByteLen > 0) {
        blockSize = MIN(inputByteLen, rateInBytes);
        for(i=0; i<blockSize; i++)
            state_8[i] ^= input[i];
        input += blockSize;
        inputByteLen -= blockSize;

        if (blockSize == rateInBytes) {
            gimli(state);
            blockSize = 0;
        }
    }

    // === Do the padding and switch to the squeezing phase ===
    state_8[blockSize] ^= 0x1F;
    // Add the second bit of padding
    state_8[rateInBytes-1] ^= 0x80;
    // Switch to the squeezing phase
    gimli(state);

    // === Squeeze out all the output blocks ===
    while(outputByteLen > 0) {
        blockSize = MIN(outputByteLen, rateInBytes);
        memcpy(output, state, blockSize);
        output += blockSize;
        outputByteLen -= blockSize;

        if (outputByteLen > 0)
            gimli(state);
    }
}


// python binding
static PyObject * gimli_permutation(PyObject *self, PyObject *args) {
  Py_buffer stateBuffer;
  if (!PyArg_ParseTuple(args, "y*", &stateBuffer)) {
    return NULL;
  }

  uint8_t * bytes = (uint8_t *)stateBuffer.buf;

  // encode words in little endian
  uint32_t state[WORDS];
  for (int i = 0; i < WORDS; i++) {
    int bi = 4 * i;
    state[i] = (
      bytes[bi + 3] << 24 |
      bytes[bi + 2] << 16 |
      bytes[bi + 1] << 8 |
      bytes[bi + 0]
    );
  }

  // apply the permutation
  gimli(state);

  // decode words from little endian
  for (int i = 0; i < WORDS; i++) {
    int bi = 4 * i;
    bytes[bi] = state[i] & 0xff;
    bytes[bi + 1] = state[i] >> 8 & 0xff;
    bytes[bi + 2] = state[i] >> 16 & 0xff;
    bytes[bi + 3] = state[i] >> 24 & 0xff;
  }

  return Py_BuildValue("y#", bytes, WORDS * 4);
}

static PyObject * gimli_hash(PyObject *self, PyObject *args) {
  Py_buffer stateBuffer;
  unsigned long long inputLength;
  if (!PyArg_ParseTuple(args, "y*K", &stateBuffer, &inputLength)) {
    return NULL;
  }

  uint8_t * input = (uint8_t *)stateBuffer.buf;
  uint64_t outputLength = 32;
  uint8_t output[outputLength];

  Gimli_hash(input, inputLength, output, outputLength);

  return Py_BuildValue("y#", output, outputLength);
}

static PyMethodDef GimliMethods[] = {
  {"_permutation",  gimli_permutation, METH_VARARGS, "Apply the gimli permutation to 384 bit state."},
  {"_hash",  gimli_hash, METH_VARARGS, "Apply the gimli hash to a sequence of bytes."},
  {NULL, NULL, 0, NULL}        // Sentinel
};

static struct PyModuleDef permutationmodule = {
  PyModuleDef_HEAD_INIT,
  "native",         // name of module
  NULL,             // module documentation, may be NULL
  -1,               // size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
  GimliMethods
};

PyMODINIT_FUNC PyInit_ref(void) {
  return PyModule_Create(&permutationmodule);
}
