from gimli.ref import _permutation, _hash


def permutation(state: bytes) -> bytes:
    return _permutation(state)


def gimli_hash(message: bytes) -> bytes:
    return _hash(message, len(message))
