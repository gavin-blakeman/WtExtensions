//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                progressGroup.cpp
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

#include "include/progressGroup.h"

// Miscellaneous libraries
#include <GCL>

void CProgressGroup::beginStep()
{
  if (currentItem->status != S_ACTIVE)
  {
    if (currentItem->status == S_PENDING)
    {
      currentItem->status = S_ACTIVE;
    }
    else if (currentItem->status == S_COMPLETE)
    {
      currentItem++;
      currentItem->status = S_ACTIVE;
    }
    else
    {
      CODE_ERROR();
    }
  }

  // Delete the text and replace with a progress bar

//  currentItem->container->clear();
//  currentItem->container->addNew(std::make_unique());
}

void CProgressGroup::completeStep()
{
  if (currentItem->status != S_ACTIVE)
  {
    CODE_ERROR();
  }
  currentItem->status = S_COMPLETE;

  // Delete the progress bar and replace with text.

}

void CProgressGroup::setRange(double dMin, double dMax)
{
  RUNTIME_ASSERT(currentItem->progress != nullptr, "Calling setMinMax before current bar is active.");

  currentItem->progress->setRange(dMin, dMax);
}

void CProgressGroup::updateProgress(double p)
{
  // Get the session lock.


}
