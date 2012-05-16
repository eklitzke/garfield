#include "./time.h"

#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <sstream>
#include <string>

namespace garfield {

// FIXME: we should try to not reallocate these facet every time; in fact, there
// ought to be a way to create the facet/stringstream on the heap, imbue the
// stringstream once, and that way less allocation has to happen when this
// method is called (doing this would require adding an Initialize() and
// Finish() method to garfield, to ensure no memory is leaked).
//
// See
// http://stackoverflow.com/questions/5330459/ownership-deleteing-the-facet-in-a-locale-stdlocale
// for more details.
std::string GetLogTime() {
  boost::local_time::local_time_facet *facet(
      new boost::local_time::local_time_facet("%Y-%m-%d %H:%M:%S.%f"));
  std::stringstream date_stream;
  date_stream.imbue(std::locale(date_stream.getloc(), facet));
  date_stream << boost::local_time::local_microsec_clock::local_time(boost::local_time::time_zone_ptr());
  return date_stream.str();
}

std::string GetRFC1123Time() {
  boost::posix_time::time_facet *facet = (
      new boost::posix_time::time_facet("%a, %d %b %Y %H:%M:%S GMT"));
  std::stringstream date_stream;
  date_stream.imbue(std::locale(date_stream.getloc(), facet));
  date_stream << boost::posix_time::second_clock::universal_time();
  return date_stream.str();
}
}
