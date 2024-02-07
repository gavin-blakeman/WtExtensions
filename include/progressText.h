#ifndef PROGRESSTEXT_H
#define PROGRESSTEXT_H

  // Standard C++ library

#include <streambuf>
#include <ostream>

  // Wt++ header files

#include <Wt/WApplication.h>
#include <Wt/WText.h>

class CProgressText : private std::streambuf, public std::ostream, public Wt::WText
{
public:
  using Traits = std::streambuf::traits_type;

  CProgressText();

private:
  Wt::WApplication *app = nullptr;

  virtual std::streambuf::int_type overflow(int c) override;


};

#endif // PROGRESSTEXT_H
