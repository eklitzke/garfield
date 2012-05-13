Garfield is a toy HTTP server, written in C++ using boost::asio. It's
single-threaded, but fully asynchronous (i.e. non-blocking), so it can handle
many concurrent requests. It should be pretty fast, too.

HTTP servers are *srs bidnis*, and writing a real one that handles all of the
weird parts of the HTTP protocol takes a lot of work. You should consider
Garfield to be "just for fun". You might find the source code useful if you're
learning about boost::asio.

Usage
=====

When you build Garfield, it will actually build a shared library
(e.g. `libgarfield.so` on Linux systems). It's up to you to write a program that
links against Garfield. There is no stand-alone server program.

There's a sample program included, `example-server.cc`, that illustrates how to
write a simple program that uses Garfield. The included Makefile with this
project will build the example server, so you can run it and test it out.
