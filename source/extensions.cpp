//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                extensions.cpp
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
