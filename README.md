Garfield -- an asynchronous, embeddable C++ HTTP server
=======================================================

Garfield is a toy HTTP server, written in C++ using
[boost::asio](www.boost.org/libs/asio). It's single-threaded, but fully
asynchronous (i.e. non-blocking), so it can handle many concurrent requests. It
should be pretty fast, too.

HTTP servers are *srs bidnis*, and writing a real one that handles all of the
weird parts of the HTTP protocol takes a lot of work. You should consider
Garfield to be "just for fun". You might find the source code useful if you're
learning about boost::asio.

Usage
-----

When you build Garfield, it will actually build a shared library
(e.g. `libgarfield.so` on Linux systems). It's up to you to write a program that
links against Garfield. There is no stand-alone server program.

The basic structure of a program that uses Garfield is like this:

```c++
#include <boost/asio.hpp>
#include <garfield.h>

void HelloWorldHandler(garfield::Request *req, garfield::Response *resp) {
  resp->headers()->SetHeader("Content-Type", "text/plain");
  resp->Write("Hello, World!\n");
}

int main(int argc, char **argv) {
  boost::asio::io_service io_service;
  garfield::HTTPServer http_server(&io_service);
  http_server.AddRoute("/", HelloWorldHandler);
  http_server.Bind(80);  // whatever port you want to bind on here
  io_service.run();      // start the main boost::asio loop
  return 0;
}
```

Each route added by `AddRoute()` consists of a Perl-style regular expression,
followed by the request handler. The request handler has two inputs, a `Request`
object and a `Response` object; you can read attributes about the incoming
request (e.g. the path, headers sent from the client) by inspecting the request
object, and you can modify information about the response (e.g. set custom
headers) by calling the various methods on the response object.  When a new
request comes in for the HTTP server, each route will be checked (in the order
that `AddRoute()` was called), and the first route with a matching specification
for the URL will have its request handler invoked. If you want to implement a
custom 404 handler, you can add a handler whose specification is `".*"` (just
make sure you add it last).

There's a sample program included, `example-server.cc`, which is a complete
example of a a simple program that uses Garfield. The included Makefile with
this project will build the example server.

Limitations
-----------

Garfield does not currently support any of the following features:

* Keep-Alive connections
* Chunked transfer-encoding
* gzip/deflate encoding, or any other transforms
