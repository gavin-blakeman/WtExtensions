//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                extendedProgressBar.h
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

#ifndef WTEXTENSIONS_INCLUDE_EXTENDEDPROGRESSBAR_H_
#define WTEXTENSIONS_INCLUDE_EXTENDEDPROGRESSBAR_H_

// Wt++ library

#include <Wt/WApplication.h>
#include <Wt/WProgressBar.h>

class extendedProgressBar : public Wt::WProgressBar
{
public:
  extendedProgressBar(Wt::WApplication &);
  ~extendedProgressBar() = default;

  void setValue(double);
  void setRange(double, double);

private:
  extendedProgressBar() = delete;
  extendedProgressBar(extendedProgressBar const &) = delete;
  extendedProgressBar(extendedProgressBar &&) = delete;
  extendedProgressBar &operator=(extendedProgressBar const &) = delete;
  extendedProgressBar &operator=(extendedProgressBar &&) = delete;

  Wt::WApplication *app = nullptr;
};


#endif /* WTEXTENSIONS_INCLUDE_EXTENDEDPROGRESSBAR_H_ */
