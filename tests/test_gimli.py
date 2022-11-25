from binascii import unhexlify, hexlify
from unittest import TestCase

from gimli import gimli_hash, permutation


class TestPermutation(TestCase):
    def test_reference_vector(self):
        state = unhexlify(
            "00000000ba79379e7af36e3c466da6da"
            "24e7dd781a6115172edb4cb566558453"
            "c8cfbbf15a4af38f22c52a2e264062cc"

        )
        expected = unhexlify(
            "5ac811ba19d1ba9180e80c38682c4cd2"
            "eaffce3e1c927a27bda0734fd89c5ada"
            "f073b684f72fe53449ef2b9ed6b81bf4"
        )

        result = permutation(state)

        self.assertEqual(result, expected)


class TestHash(TestCase):
    def test_paper_vector1(self):
        message = b"There's plenty for the both of us, may the best Dwarf win."
        expected = unhexlify(
            "4afb3ff784c7ad6943d49cf5da79facfa7c4434e1ce44f5dd4b28f91a84d22c8"
        )

        result = gimli_hash(message)

        self.assertEqual(result, expected)
    
    def test_paper_vector2(self):
        message = (
            b"If anyone was to ask for my opinion, which I note they're not, "
            b"I'd say we were taking the long way around."
        )
        expected = unhexlify(
            "ba82a16a7b224c15bed8e8bdc88903a4006bc7beda78297d96029203ef08e07c"
        )

        result = gimli_hash(message)

        self.assertEqual(result, expected)
    
    def test_paper_vector3(self):
        message = b"Speak words we can all understand!"
        expected = unhexlify(
            "8dd4d132059b72f8e8493f9afb86c6d86263e7439fc64cbb361fcbccf8b01267"
        )

        result = gimli_hash(message)

        self.assertEqual(result, expected)
    
    def test_paper_vector4(self):
        message = (
            b"It's true you don't see many Dwarf-women. And in fact, they are so "
            b"alike in voice and appearance, that they are often mistaken for Dwarf-men.  "
            b"And this in turn has given rise to the belief that there are no Dwarf-women, "
            b"and that Dwarves just spring out of holes in the ground! Which is, of course, "
            b"ridiculous."
        )
        expected = unhexlify(
            "ebe9bfc05ce15c73336fc3c5b52b01f75cf619bb37f13bfc7f567f9d5603191a"
        )

        result = gimli_hash(message)

        self.assertEqual(result, expected)
        