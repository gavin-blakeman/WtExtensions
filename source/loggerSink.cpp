#include "include/loggerSink.h"

  // Miscellaneous library header files

#include <GCL>

Wt::WLogSink &customWtLogger()
{
  static CWTLogger cl;

  return cl;
}

/// @brief Logs the specified message.
/// @param[in] type:
/// @param[in] scope:
/// @param[in] message:
/// @throws NOEXCEPT
/// @version 2022-10-13/GGB - Function created.

void CWTLogger::log(std::string const &type, std::string const &scope, std::string const &message) const noexcept
{

  GCL::logger::severity_e severity;

  if (type == "debug")
  {
    severity = GCL::logger::s_debug;
  }
  else if (type == "info")
  {
    severity = GCL::logger::s_information;
  }
  else if (type == "warning")
  {
    severity = GCL::logger::s_warning;
  }
  else if (type == "error")
  {
    severity = GCL::logger::s_error;
  }
  else
  {
    std::cout << type << std::endl;
  }

  if (scope != "WEnvironment" &&
      scope != "WApplication")
  {
    LOGMESSAGE(severity, message);
  };
}

/// @brief Determine if the specified types of messages should be logged.
/// @throws NOEXCEPT
/// @version 2022-10-13/GGB - Function created.

bool CWTLogger::logging(std::string const &/*type*/, std::string const &/*scope*/) const noexcept
{
  return true;
}
