//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                loggerSink.cpp
// LANGUAGE:            C++
// TARGET OS:           None.
// NAMESPACE:           GCL
// AUTHOR:              Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2024 Gavin Blakeman.
//                      This file is part of the WtExtensions Library (WtExtensions)
//
//                      WtExtensions is free software: you can redistribute it and/or modify it under the terms of the GNU General
//                      Public License as published by the Free Software Foundation, either version 2 of the License, or
//                      (at your option) any later version.
//
//                      WtExtensions is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
//                      implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//                      for more details.
//
//                      You should have received a copy of the GNU General Public License along with WtExtensions.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:            A progress bar that can be updated from outside the UI loop.
//
// HISTORY:             2024-03-01 GGB - File Created
//
//*********************************************************************************************************************************

#include "include/loggerSink.h"

  // Miscellaneous library header files

#include <GCL>

Wt::WLogSink &customWtLogger()
{
  static CWTLogger cl;

  return cl;
}

/// @brief      Logs the specified message.
/// @param[in]  type:
/// @param[in]  scope:
/// @param[in]  message:
/// @throws     NOEXCEPT
/// @version    2022-10-13/GGB - Function created.

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

  if (scope != "WEnvironment" && scope != "WApplication")
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
