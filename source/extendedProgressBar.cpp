//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                extendedProgressBar.cpp
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

#include "include/extendedProgressBar.h"

extendedProgressBar::extendedProgressBar(Wt::WApplication &a) : Wt::WProgressBar(), app(&a)
{
  app->enableUpdates(true);
}


void extendedProgressBar::setValue(double value)
{
  Wt::WApplication::UpdateLock uiLock(app);

  if (uiLock)
  {
    WProgressBar::setValue(value);
    app->triggerUpdate();
  };
}

void extendedProgressBar::setRange(double minimum, double maximum)
{
  Wt::WApplication::UpdateLock uiLock(app);

  if (uiLock)
  {
    WProgressBar::setRange(minimum, maximum);
    app->triggerUpdate();
  };
}
