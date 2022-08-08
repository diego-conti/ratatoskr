# Ratatoskr

Ratatoskr is a (header-only?) library built on top of [Wedge](https://github.com/diego-conti/wedge) to enable one to write command-line based programs to make computations on lie algebras etc.

## Requirements

*Ratatoskr* requires [cmake](https://cmake.org/) and [Wedge](https://github.com/diego-conti/wedge)

To compile, run

	mkdir build
	cd build
	cmake ..
	cmake --build .

You may need to set the WEDGE_PATH environment variable to point to your installation of Wedge, e.g.

	export WEDGE_PATH=/home/user/wedge


