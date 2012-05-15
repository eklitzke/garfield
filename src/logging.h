// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_LOGGING_H_
#define SRC_LOGGING_H_

#include <functional>

namespace garfield {
enum LogLevel {
  ACCESS = 0,
  ERROR  = 1
};

typedef std::function<void(LogLevel level, const char *msg, int bytes)> Logger;

void Log(LogLevel level, const char *fmt, ...);

Logger GetLogger();
void SetLogger(Logger);

void FileLogger(LogLevel level, const char *msg, int bytes);
void StdErrLogger(LogLevel level, const char *msg, int bytes);
}

#endif  // SRC_LOGGING_H_
