#pragma once

/*
https://gist.github.com/andreasxp/ac9adcf8a2b37ac05ff7047f8728b3c7

MIT No Attribution

Copyright 2020-2023 Andrey Zhukov

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <istream>
#include <streambuf>
#include <string_view>

#include <iostream>

namespace zh {

/**
 * string_view-compatible buffer for reading read-only characters from memory.
 * Implements a buffer based on stackoverflow discussions. Written to be used
 * via isviewstream.
 *
 * @warning sviewbuf does not own memory. If the lifetime of the string ends
 * before this buffer, reading from it invokes undefined behavior.
 *
 * @see https://stackoverflow.com/a/13059195
 * @see https://stackoverflow.com/a/46069245
 */
class sviewbuf: public std::streambuf {
private:
	using ios_base = std::ios_base;

protected:
	pos_type seekoff(off_type off, ios_base::seekdir dir, ios_base::openmode openmode = ios_base::in | ios_base::out) override {
		if(dir == ios_base::cur){
			gbump(off);
			pbump(off);
		}else if(dir == ios_base::end){
			setg(eback(), egptr() + off, egptr());
			setp(egptr() + off, egptr());
		}
		else if(dir == ios_base::beg){
			setg(eback(), eback() + off, egptr());
			setp(egptr() + off, egptr());
		}
		printStatus();
		return gptr() - eback();
	}

	pos_type seekpos(pos_type sp, ios_base::openmode openmode) override {
		printStatus();
		return seekoff(sp - pos_type(off_type(0)), ios_base::beg, openmode);
	}

	int_type overflow(int_type ch) override {
		// Fixed stream buffer doesn't handle overflow because we don't want to
		throw std::runtime_error("sviewbuf overflow!");
	}

public:
	sviewbuf(const char *s, std::size_t count) {
		auto *p = const_cast<char *>(s);
		setg(p, p, p + count);
		setp(p, p + count);
		printStatus();
	}


	void printStatus(){
		std::cerr << "pptr() - pbase() " << pptr() - pbase() << " epptr() - pptr() " << epptr() - pptr() << std::endl;
	}

	sviewbuf(std::string_view str): sviewbuf(str.data(), str.size()) {}
	~sviewbuf() {
		printStatus();
		*(pptr()) = 0;
	}
};

/**
 * A std::stringstream analog for string_view.
 * Implements a string_view-based input stream based on stackoverflow
 * discussions.
 *
 * @warning isviewstream does not own memory. If the lifetime of the string
 * ends before this stream, reading from it invokes undefined behavior.
 *
 * @see https://stackoverflow.com/a/13059195
 * @see https://stackoverflow.com/a/46069245
 */
class sviewstream: private virtual sviewbuf, public std::iostream {
  public:
	sviewstream(std::string_view str): sviewbuf(str), std::iostream(static_cast<std::streambuf *>(this)) {
	}
};

};// namespace zh
