#include "include/extensions.h"

namespace std
{
  std::string to_string(Wt::WString const &ws)
  {
    return "'" + ws.toUTF8() + "'";
  }

  std::string to_string(Wt::WDate const &wd)
  {
    return "'" + wd.toString("yyyyMMdd").toUTF8() + "'";
  }

  std::string to_string(Wt::WDateTime const &wdt)
  {
    return "'" + wdt.toString("yyyy-MM-ddTHH:mm:ss").toUTF8() + "'";
  }

  std::string to_string(Wt::WTime const &wt)
  {
    return "'" + wt.toString("HH:mm:ss").toUTF8() + "'";
  }


}
