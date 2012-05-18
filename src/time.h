// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_TIME_H_
#define SRC_TIME_H_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include <string>

namespace garfield {
// Get the current date/time as a string suitable for putting in an access log.
std::string GetLogTime();

// Get the date/time as a string in RFC 1123 format.
std::string GetRFC1123Time();
std::string GetRFC1123Time(std::time_t when);
std::string GetRFC1123Time(boost::posix_time::ptime when);
}

#endif  // SRC_TIME_H_
