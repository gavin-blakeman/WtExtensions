//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                progressGroup.h
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

#ifndef WTEXTENSIONS_PROGRESSGROUP_H
#define WTEXTENSIONS_PROGRESSGROUP_H

// Standard C++ header files
#include <cstdint>
#include <initializer_list>
#include <string>

// Wt Library

#include <Wt/WContainerWidget.h>
#include <Wt/WProgressBar.h>

/* A progress groups is a container (maybe a tree or table) widget that has a list of progress items. Each item has a string
 * attached and a progress bar or text string.
 *
 * Format is as follows
 * Step 1   Load Files      Complete
 * Step 2   Process Data    <Progress Bar - Updating>
 * Step 3   Prepare Output  Pending
 *
 * Bars need to be close stacked (if possible, may need .css) and scroll to display the current bar.
 * Updates need to be limited, maybe 1/s, 1/2s/ 1/5s.
 *
 * When the group is initialised. All items are display the itemText and the initialText
 * Step 1. Load Files       Pending
 * Step 2. Process Data     Pending
 * Step 3. Prepare Output   Pending
 *
 * When beginStep() is called
 * Step 1. Load Files       |--------|
 * Step 2. Process Data     Pending
 *
 * When completeStep() is called
 * Step 1. Load Files       Complete
 * Step 2. Process Data     Pending
 *
 * Step order is order of initialisation.
 * This approach allows a calling application to set up all the steps. Successive library calls can move to the next step and
 * update progress without knowing what steps they are on.
 */

class CProgressGroup
{
  public:
    using progressBarID_t = std::uint_least8_t;
    enum status_e
    {
      S_NONE,
      S_PENDING,
      S_ACTIVE,
      S_COMPLETE,
    };

    CProgressGroup(std::initializer_list<std::tuple<progressBarID_t, std::string, std::string, std::string>> &);

    void createBar(std::string const &, std::string const &);
    void setRange(double, double);
    void updateProgress(double);
    void beginStep();
    void completeStep();
    void completeProgress();


  protected:
    struct progress_t
    {
      std::string itemText;
      std::string initialText;
      std::string finalText;
      Wt::WTextEdit *textEditItem;
      Wt::WTextEdit *textEditStatus;
      Wt::WProgressBar *progress;
      status_e status = S_PENDING;
    };

    std::list<progress_t> progressBars;
    std::list<progress_t>::iterator currentItem;

  private:
    CProgressGroup() = delete;
    CProgressGroup(CProgressGroup const &) = delete;
    CProgressGroup(CProgressGroup &&) = delete;
    CProgressGroup &operator=(CProgressGroup const &) = delete;
    CProgressGroup &operator=(CProgressGroup &&) = delete;
};

#endif
