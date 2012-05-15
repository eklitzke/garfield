// Copyright 2012, Evan Klitzke <evan@eklitzke.org>

#include "./handlers.h"

#include <stdio.h>
#include <unistd.h>

#include <map>
#include <utility>

namespace {
#ifndef PATH_MAX
enum { PATH_MAX = 4096 };
#endif
enum { READ_SIZE = 8192 };

std::map<std::string, std::string> mimetypes;

// Initialize the mimetypes mapping; this list was (mostly) created from the
// Python mimetypes module.
void InitializeMimetypes() {
  mimetypes.insert(mimetypes.end(), std::make_pair("a", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ai", "application/postscript"));
  mimetypes.insert(mimetypes.end(), std::make_pair("aif", "audio/x-aiff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("aifc", "audio/x-aiff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("aiff", "audio/x-aiff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("au", "audio/basic"));
  mimetypes.insert(mimetypes.end(), std::make_pair("avi", "video/x-msvideo"));
  mimetypes.insert(mimetypes.end(), std::make_pair("bat", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("bcpio", "application/x-bcpio"));
  mimetypes.insert(mimetypes.end(), std::make_pair("bin", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("bmp", "image/x-ms-bmp"));
  mimetypes.insert(mimetypes.end(), std::make_pair("c", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("cc", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("cdf", "application/x-netcdf"));
  mimetypes.insert(mimetypes.end(), std::make_pair("cpio", "application/x-cpio"));
  mimetypes.insert(mimetypes.end(), std::make_pair("csh", "application/x-csh"));
  mimetypes.insert(mimetypes.end(), std::make_pair("css", "text/css"));
  mimetypes.insert(mimetypes.end(), std::make_pair("dll", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("doc", "application/msword"));
  mimetypes.insert(mimetypes.end(), std::make_pair("dot", "application/msword"));
  mimetypes.insert(mimetypes.end(), std::make_pair("dvi", "application/x-dvi"));
  mimetypes.insert(mimetypes.end(), std::make_pair("eml", "message/rfc822"));
  mimetypes.insert(mimetypes.end(), std::make_pair("eps", "application/postscript"));
  mimetypes.insert(mimetypes.end(), std::make_pair("etx", "text/x-setext"));
  mimetypes.insert(mimetypes.end(), std::make_pair("exe", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("gif", "image/gif"));
  mimetypes.insert(mimetypes.end(), std::make_pair("gtar", "application/x-gtar"));
  mimetypes.insert(mimetypes.end(), std::make_pair("h", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("hdf", "application/x-hdf"));
  mimetypes.insert(mimetypes.end(), std::make_pair("htm", "text/html"));
  mimetypes.insert(mimetypes.end(), std::make_pair("html", "text/html"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ief", "image/ief"));
  mimetypes.insert(mimetypes.end(), std::make_pair("jpe", "image/jpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("jpeg", "image/jpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("jpg", "image/jpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("js", "application/x-javascript"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ksh", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("latex", "application/x-latex"));
  mimetypes.insert(mimetypes.end(), std::make_pair("m1v", "video/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("man", "application/x-troff-man"));
  mimetypes.insert(mimetypes.end(), std::make_pair("me", "application/x-troff-me"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mht", "message/rfc822"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mhtml", "message/rfc822"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mif", "application/x-mif"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mov", "video/quicktime"));
  mimetypes.insert(mimetypes.end(), std::make_pair("movie", "video/x-sgi-movie"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mp2", "audio/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mp3", "audio/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mp4", "video/mp4"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mpa", "video/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mpe", "video/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mpeg", "video/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("mpg", "video/mpeg"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ms", "application/x-troff-ms"));
  mimetypes.insert(mimetypes.end(), std::make_pair("nc", "application/x-netcdf"));
  mimetypes.insert(mimetypes.end(), std::make_pair("nws", "message/rfc822"));
  mimetypes.insert(mimetypes.end(), std::make_pair("o", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("obj", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("oda", "application/oda"));
  mimetypes.insert(mimetypes.end(), std::make_pair("p12", "application/x-pkcs12"));
  mimetypes.insert(mimetypes.end(), std::make_pair("p7c", "application/pkcs7-mime"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pbm", "image/x-portable-bitmap"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pdf", "application/pdf"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pfx", "application/x-pkcs12"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pgm", "image/x-portable-graymap"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pl", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("png", "image/png"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pnm", "image/x-portable-anymap"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pot", "application/vnd.ms-powerpoint"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ppa", "application/vnd.ms-powerpoint"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ppm", "image/x-portable-pixmap"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pps", "application/vnd.ms-powerpoint"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ppt", "application/vnd.ms-powerpoint"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ps", "application/postscript"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pwz", "application/vnd.ms-powerpoint"));
  mimetypes.insert(mimetypes.end(), std::make_pair("py", "text/x-python"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pyc", "application/x-python-code"));
  mimetypes.insert(mimetypes.end(), std::make_pair("pyo", "application/x-python-code"));
  mimetypes.insert(mimetypes.end(), std::make_pair("qt", "video/quicktime"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ra", "audio/x-pn-realaudio"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ram", "application/x-pn-realaudio"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ras", "image/x-cmu-raster"));
  mimetypes.insert(mimetypes.end(), std::make_pair("rdf", "application/xml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("rgb", "image/x-rgb"));
  mimetypes.insert(mimetypes.end(), std::make_pair("roff", "application/x-troff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("rtx", "text/richtext"));
  mimetypes.insert(mimetypes.end(), std::make_pair("sgm", "text/x-sgml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("sgml", "text/x-sgml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("sh", "application/x-sh"));
  mimetypes.insert(mimetypes.end(), std::make_pair("shar", "application/x-shar"));
  mimetypes.insert(mimetypes.end(), std::make_pair("snd", "audio/basic"));
  mimetypes.insert(mimetypes.end(), std::make_pair("so", "application/octet-stream"));
  mimetypes.insert(mimetypes.end(), std::make_pair("src", "application/x-wais-source"));
  mimetypes.insert(mimetypes.end(), std::make_pair("sv4cpio", "application/x-sv4cpio"));
  mimetypes.insert(mimetypes.end(), std::make_pair("sv4crc", "application/x-sv4crc"));
  mimetypes.insert(mimetypes.end(), std::make_pair("swf", "application/x-shockwave-flash"));
  mimetypes.insert(mimetypes.end(), std::make_pair("t", "application/x-troff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tar", "application/x-tar"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tcl", "application/x-tcl"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tex", "application/x-tex"));
  mimetypes.insert(mimetypes.end(), std::make_pair("texi", "application/x-texinfo"));
  mimetypes.insert(mimetypes.end(), std::make_pair("texinfo", "application/x-texinfo"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tif", "image/tiff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tiff", "image/tiff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tr", "application/x-troff"));
  mimetypes.insert(mimetypes.end(), std::make_pair("tsv", "text/tab-separated-values"));
  mimetypes.insert(mimetypes.end(), std::make_pair("txt", "text/plain"));
  mimetypes.insert(mimetypes.end(), std::make_pair("ustar", "application/x-ustar"));
  mimetypes.insert(mimetypes.end(), std::make_pair("vcf", "text/x-vcard"));
  mimetypes.insert(mimetypes.end(), std::make_pair("wav", "audio/x-wav"));
  mimetypes.insert(mimetypes.end(), std::make_pair("wiz", "application/msword"));
  mimetypes.insert(mimetypes.end(), std::make_pair("wsdl", "application/xml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xbm", "image/x-xbitmap"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xlb", "application/vnd.ms-excel"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xls", "application/vnd.ms-excel"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xml", "text/xml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xpdl", "application/xml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xpm", "image/x-xpixmap"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xsl", "application/xml"));
  mimetypes.insert(mimetypes.end(), std::make_pair("xwd", "image/x-xwindowdump"));
  mimetypes.insert(mimetypes.end(), std::make_pair("zip", "application/zip"));
}

// Guess the mime type for an extension
std::string GuessMimeType(const std::string &extension) {
  if (mimetypes.empty()) {
    InitializeMimetypes();
  }
  auto it = mimetypes.find(extension);
  if (it == mimetypes.end()) {
    return "application/octet-stream";
  } else {
    return it->second;
  }
}
}

namespace garfield {
void NotFoundHandler(Request *req, Response *resp) {
  resp->set_status(404);
  resp->headers()->SetHeader("Content-Type", "text/plain");
  resp->Write("The page you were looking for could not be found.\n");
}

void StaticFileHandler(Request *req, Response *resp) {
  std::string path = req->path;
  while (path.size() && path[0] == '/') {
    path = path.substr(1, path.size() - 1);
  }
  if (path.empty()) {
    resp->set_status(404);
  } else {
    std::unique_ptr<char[]> rp_buf(new char[PATH_MAX]);
    char *rp_cstr = realpath(path.c_str(), rp_buf.get());
    if (rp_cstr == nullptr) {
      resp->set_status(404);
      return;
    }
    std::string rp(rp_cstr);
    std::unique_ptr<char[]> cwd(new char[PATH_MAX]);
    getcwd(cwd.get(), PATH_MAX);
    std::string curdir(cwd.get());
    
    // Ensure that the realpath is in a subdirectory of the current directory
    if (rp.substr(0, curdir.length()) != curdir) {
      resp->set_status(404);
    } else {
      FILE *f = fopen(rp_cstr, "r");
      if (f == nullptr) {
        resp->set_status(404);
        return;
      }

      std::string::size_type dotpos = rp.find_last_of('.');
      if (dotpos == std::string::npos) {
        resp->headers()->SetHeader("Content-Type", "application/octet-stream");
      } else {
        std::string extension = rp.substr(dotpos + 1, rp.length() - dotpos - 1);
        resp->headers()->SetHeader("Content-Type", GuessMimeType(extension));
      }
      std::unique_ptr<char[]> buf(new char[READ_SIZE]);
      while (true) {
        std::size_t bytes_read = fread(buf.get(), sizeof(char), READ_SIZE, f);
        if (bytes_read == 0) {
          break;
        }
        resp->Write(std::string(buf.get(), bytes_read));
      }
    }
  }
}
}
