//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                progressText.cpp
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

#include "include/progressText.h"

  // Standard C++ header files

#include <string>

  // Wt header files

#include <web/DomElement.h>
#include <Wt/WString.h>

CProgressText::CProgressText() : std::ostream(this), Wt::WText(), app(Wt::WApplication::instance())
  {
    setTextFormat(Wt::TextFormat::Plain);
    setStyleClass("textarea");
    app->enableUpdates(true);
  }

std::streambuf::int_type CProgressText::overflow(int ch)
{
  Wt::WApplication::UpdateLock uiLock(app);

  if (uiLock)
  {
    if (ch != Traits::eof())
    {
      setText(text() + std::string{static_cast<char>(ch)});
      doJavaScript(jsRef() + ".scrollTop = "+ jsRef() + ".scrollHeight;");
      app->triggerUpdate();
      return 0;
    };
  };
}

void CProgressText::updateDom(Wt::DomElement& element, bool all)
{
  Wt::WText::updateDom(element, all);
  element.setProperty(Wt::Property::StyleOverflowY, "auto");
}
