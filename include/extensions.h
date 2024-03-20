//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                extensions.h
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
// OVERVIEW:
//
// HISTORY:             2024-03-01 GGB - File Created
//
//*********************************************************************************************************************************

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
