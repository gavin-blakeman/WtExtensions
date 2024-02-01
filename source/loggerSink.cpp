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
//  enum ESeverity
//  {
//    trace,      ///< Lowest level log severity. Used for tracing program execution through function entry/exit.
//    exception,  ///< Used to capture exceptions. These do not need to be actioned as the code may manage them.
//    debug,      ///< Used to display debugging messages.
//    info,       ///< Information messages.
//    notice,     ///< Legal notices etc.
//    warning,    ///< Failure of action. Normally built into program logic. (Time outs etc)
//    error,      ///< Failure of program function. May be recoverable.
//    critical,   ///< Non-recoverable failure.
//  };

  GCL::logger::ESeverity severity = GCL::logger::debug;

  if (type == "debug")
  {
    severity = GCL::logger::debug;
  }
  else if (type == "info")
  {
    severity = GCL::logger::info;
  }
  else if (type == "warning")
  {
    severity = GCL::logger::warning;
  }
  else if (type == "error")
  {
    severity = GCL::logger::error;
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
