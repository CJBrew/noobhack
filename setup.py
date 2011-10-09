from distutils.core import setup, Extension

intrinsicsModule = Extension('intrinsicsC',
		sources = ['noobhack/game/cpp/intrinsics.cpp'],
		libraries = ['boost_regex'])

setup(
    name="noobhack",
    version="0.3",
    author="Sam Gibson",
    author_email="sam@ifdown.net",
    url="https://github.com/samfoo/noobhack",
    description="noobhack helps you ascend at nethack",
    long_description=open("readme.md", "r").read(),
    requires=["vt102 (>=0.3.2)"],
    packages=["noobhack", "noobhack.game"],
    scripts=["scripts/noobhack"],
    license="Lesser General Public License v3.0",
    ext_modules=[intrinsicsModule], #Extension('intrinsicsC', ['noobhack/game/cpp/intrinsics.cpp'])],
)


