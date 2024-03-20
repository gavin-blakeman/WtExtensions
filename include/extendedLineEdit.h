//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                extendedLineEdit.h
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

#ifndef EXTENDEDLINEEDIT_H
#define EXTENDEDLINEEDIT_H

#include <cstdint>

#include <Wt/WLineEdit.h>
#include <Wt/WSignal.h>

class extendedLineEdit: public Wt::WLineEdit
{
private:
  std::size_t userData;
  Wt::Signal<std::size_t> selectionChanged_;

public:
  extendedLineEdit(std::size_t ud) : WLineEdit(), userData(ud)
  {
    changed().connect([this]{ selectionChanged_.emit(userData); });
  }
  void setUID(std::size_t uid) { userData = uid; }


  Wt::Signal<std::size_t> &changed()
  {
    return selectionChanged_;
  }


};

#endif // EXTENDEDLINEEDIT_H
