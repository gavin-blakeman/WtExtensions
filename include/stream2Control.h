//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                stream2Control.h
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
