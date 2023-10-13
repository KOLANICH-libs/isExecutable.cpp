isExecutable.cpp [![Unlicensed work](https://raw.githubusercontent.com/unlicense/unlicense.org/master/static/favicon.png)](https://unlicense.org/)
================
~~[![GitHub Actions](https://github.com/KOLANICH-libs/isExecutable.cpp/workflows/CI/badge.svg)](https://github.com/KOLANICH-libs/isExecutable.cpp/actions/)~~
[![Libraries.io Status](https://img.shields.io/librariesio/github/KOLANICH-libs/isExecutable.cpp.svg)](https://libraries.io/github/KOLANICH-libs/isExecutable.cpp)

**We have moved to https://codeberg.org/KOLANICH-libs/isExecutable.cpp, grab new versions there.**

Under the disguise of "better security" Micro$oft-owned GitHub has [discriminated users of 1FA passwords](https://github.blog/2023-03-09-raising-the-bar-for-software-security-github-2fa-begins-march-13/) while having commercial interest in success and wide adoption of [FIDO 1FA specifications](https://fidoalliance.org/specifications/download/) and [Windows Hello implementation](https://support.microsoft.com/en-us/windows/passkeys-in-windows-301c8944-5ea2-452b-9886-97e4d2ef4422) which [it promotes as a replacement for passwords](https://github.blog/2023-07-12-introducing-passwordless-authentication-on-github-com/). It will result in dire consequencies and is competely inacceptable, [read why](https://codeberg.org/KOLANICH/Fuck-GuanTEEnomo).

If you don't want to participate in harming yourself, it is recommended to follow the lead and migrate somewhere away of GitHub and Micro$oft. Here is [the list of alternatives and rationales to do it](https://github.com/orgs/community/discussions/49869). If they delete the discussion, there are certain well-known places where you can get a copy of it. [Read why you should also leave GitHub](https://codeberg.org/KOLANICH/Fuck-GuanTEEnomo).

---

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
