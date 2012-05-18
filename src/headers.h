// -*- C++ -*-
// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#ifndef SRC_HEADERS_H_
#define SRC_HEADERS_H_

#include <map>
#include <string>

namespace garfield {
std::string NormalizeHeaderName(const std::string &name);

struct HeaderKey {
  HeaderKey(const std::string &key)
      :raw_key(key), norm_key(NormalizeHeaderName(key)) {}

  bool operator == (const HeaderKey &other) const {
    return (norm_key == other.norm_key);
  }

  bool operator != (const HeaderKey &other) const {
    return (norm_key != other.norm_key);
  }

  bool operator< (const HeaderKey &other) const {
    return (norm_key < other.norm_key);
  }

  std::string raw_key;
  std::string norm_key;
};

class HeadersDict {
 public:
  // Read the value of a header, and return an empty string if the header does
  // not already exist.
  std::string GetHeader(const std::string &name) const;

  // Replace a header (insert the header if it doesn't exist, replace the header
  // if it does exist).
  void SetHeader(const HeaderKey &key, const std::string &val);
  void SetHeader(const std::string &key, const std::string &val);

  // This adds a header (creating a header with a duplicate name if one already
  // exists).
  void AddHeader(const HeaderKey &key, const std::string &val);
  void AddHeader(const std::string &key, const std::string &val);

  // Get the formatted headers as a string suitable for including in an HTTP
  // request.
  std::string GetHeadersAsString() const;

 private:
  std::multimap<HeaderKey, std::string> headers_;
};
}
#endif  // SRC_HEADERS_H_
