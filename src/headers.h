// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_HEADERS_H_
#define SRC_HEADERS_H_

#include <map>
#include <string>

namespace garfield {
class HeadersDict {
 public:
  // Read the value of a header, and return an empty string if the header does
  // not already exist.
  std::string GetHeader(const std::string &name) const;

  // Replace a header (insert the header if it doesn't exist, replace the header
  // if it does exist).
  void SetHeader(const std::string &name, const std::string &val);

  // This adds a header (creating a header with a duplicate name if one already
  // exists).
  void AddHeader(const std::string &name, const std::string &val);

  // Get the formatted headers as a string suitable for including in an HTTP
  // request.
  std::string GetHeadersAsString() const;

 private:
  std::multimap<std::string, std::string> headers_;
};
}
#endif  // SRC_HEADERS_H_
