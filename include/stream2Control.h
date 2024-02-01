

#ifndef STREAM2CONTROL_H
#define STREAM2CONTROL_H

  // Standard C++ library header files

#include <iostream>

  // Wt++ library header files

#include <Wt/WWebWidget.h>

class stream2Control : private std::streambuf, public std::ostream
{
public:
  stream2Control(Wt::WWebWidget *) : std::ostream(this) {}

private:
  int overflow(int c) override{};

};


#endif // STREAM2CONTROL_H
