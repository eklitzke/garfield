// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_TIME_H_
#define SRC_TIME_H_

#include <string>

namespace garfield {
// Get the current date/time as a string suitable for putting in an access log.
std::string GetLogTime();

// Get the current date/time as a string in RFC 1123 format.
std::string GetRFC1123Time();
}

#endif  // SRC_TIME_H_
