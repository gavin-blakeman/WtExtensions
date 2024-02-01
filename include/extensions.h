#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <Wt/WDate.h>
#include <Wt/WDateTime.h>
#include <Wt/WString.h>
#include <Wt/WTime.h>

namespace std
{
  std::string to_string(Wt::WString const &);
  std::string to_string(Wt::WDate const &);
  std::string to_string(Wt::WDateTime const &);
  std::string to_string(Wt::WTime const &);
}

#endif // EXTENSIONS_H
