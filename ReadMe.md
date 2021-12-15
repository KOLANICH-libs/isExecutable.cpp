isExecutable.cpp [![Unlicensed work](https://raw.githubusercontent.com/unlicense/unlicense.org/master/static/favicon.png)](https://unlicense.org/)
================
~~[![GitHub Actions](https://github.com/KOLANICH-libs/isExecutable.cpp/workflows/CI/badge.svg)](https://github.com/KOLANICH-libs/isExecutable.cpp/actions/)~~
[![Libraries.io Status](https://img.shields.io/librariesio/github/KOLANICH-libs/isExecutable.cpp.svg)](https://libraries.io/github/KOLANICH-libs/isExecutable.cpp)

A library for checking if a file is executable.

## Features

Does:
* parses headers of
	* ELF
	* PE
	* Mach-O
* reports
	* file class
		* ordinary
		* executable
		* shared library
		* shell script
	* whether PIC used (according to headers)
	* bitness: 32 or 64
	* access rights - whether `x` bit is set
* POSIX platforms:
	* checks shebang
* Win32
	* if a file name matches the one of shell scripts
* provides functions to convert machine-readable reports into strings, both human-readable and machine-readable
	* JSON
	* human-readable 1 char space-separated "brief" format
	* human-readable space-separated "verbose" format
* provides C wrappers to be used from the languages other than C++

Doesn't do:
* doesn't parse the format fully, only some few checks on headers are done
* big-endian machines are out of scope, everything is assummed to be little endian
* doesn't check machine code
* doesn't check whether content is really a shell script
* doesn't check if we can really run that file


## Installation

Packaging with CPack is implemented, you can generate an installable package for Debian and RPM-based distros.
All the dependencies are assummed to be installed the same way.

## Usage
See the example in `bin` directory.


Related projects
----------------
* https://github.com/fitzgen/is_executable - a lib doing the similar thing in Rust. Doesn't parse ELF.
