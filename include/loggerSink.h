#ifndef WT_EXTENSIONS_LOGGERSINK_H
#define WT_EXTENSIONS_LOGGERSINK_H

  // Standard C++ library

#include <Wt/WLogSink.h>

class CWTLogger : public Wt::WLogSink
{
public:
  virtual void log(std::string const &, std::string const &, std::string const &) const noexcept override;
  virtual bool logging(std::string const &, std::string const &) const noexcept override;
};

Wt::WLogSink &customWtLogger();


#endif // WT_EXTENSIONS_LOGGERSINK_H
