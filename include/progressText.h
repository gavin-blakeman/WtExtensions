#ifndef PROGRESSTEXT_H
#define PROGRESSTEXT_H

  // Wt++ header files

#include <Wt/WText.h>

class CProgressText : private std::streambuf, public std::ostream, public Wt::WText
{
public:
  CProgressText() : std::ostream(this), Wt::WText() {}

private:
  int overflow(int c) override;

};

#endif // PROGRESSTEXT_H
