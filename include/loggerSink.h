#ifndef LOGGERSINK_H
#define LOGGERSINK_H

  // Standard C++ library

#include <Wt/WLogSink.h>

class CWTLogger : public Wt::WLogSink
{
public:
  virtual void log(std::string const &, std::string const &, std::string const &) const noexcept override;
  virtual bool logging(std::string const &, std::string const &) const noexcept override;
};

Wt::WLogSink &customWtLogger();


#endif // LOGGERSINK_H
