// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_RESPONSE_H_
#define SRC_RESPONSE_H_

#include <string>
#include <vector>

#include "./headers.h"

namespace garfield {
class Response {
 public:
  Response();
  void Write(const std::string &msg);
  std::string GetStatusName();

  // Convenience methods for handlers
  void NotFound();
  void NotModified();

  // Getters and setters
  int status() const { return status_; }
  void set_status(int status) { status_ = status; }

  bool suppress_content_length() const { return suppress_content_length_; }
  void set_suppress_content_length(bool suppress_content_length) {
    suppress_content_length_ = suppress_content_length;
  }

  HeadersDict* headers() { return &headers_; }
  const std::vector<std::string>& output() const { return output_; }

 private:
  int status_;
  HeadersDict headers_;
  std::vector<std::string> output_;
  bool suppress_content_length_;
};
}

#endif  // SRC_RESPONSE_H_
