from setuptools import Extension
from setuptools.command.build_ext import build_ext


ext_modules = [
    Extension(
        "gimli.ref",
        sources=["src/ref.c"],
        extra_compile_args=['-O2'],
    ),
]


def build(setup_kwargs):
    setup_kwargs.update(
        {"ext_modules": ext_modules, "cmdclass": {"build_ext": build_ext}}
    )
