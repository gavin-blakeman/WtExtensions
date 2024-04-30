//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                progressGroup.h
// LANGUAGE:            C++
// TARGET OS:           None.
// NAMESPACE:
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
#include <functional>
#include <initializer_list>
#include <string>
#include <tuple>

// Wt Library
#include <Wt/WContainerWidget.h>

// Miscellaneous library header files
#include <SCL>

/* A progress groups is a container (maybe a tree or table) widget that has a list of progress items. Each item has a string
 * attached and a progress bar or text string. A tableview is closer packed than a table. However the tableview requires a
 * model to support data retrieval.
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

class CProgressGroupModel;

class CProgressGroup : public Wt::WContainerWidget
{
  public:
    using ID_t = std::uint_least8_t;
    enum status_e
    {
      S_NONE,         //
      S_PENDING,      // The pending text is displayed. IE this is a future action. The initialText is displayed.
      S_ACTIVE,       // This is the active action. There should only be one active action. When active, the progress bar is
                      // displayed.
      S_COMPLETE,     // The action is complete. The finalText is displayed.
    };
    struct actionItem_t
    {
      ID_t ID;
      ID_t PID;
      std::string itemText;
      status_e status;
      Wt::WText *textEditStatus = nullptr;  // Active when status == Pending or complete.
      Wt::WProgressBar *progressBar = nullptr;  // Active when status == Active.

      template<int N> ID_t get() const noexcept
      {
        if constexpr (N == 0)
        {
          return ID;
        }
        else if constexpr (N == 1)
        {
          return PID;
        }
        else
        {
          CODE_ERROR();
        }
      }
    };
    using value_type = actionItem_t;
    using reference = value_type &;
    using const_reference = value_type const &;
    using pointer = value_type *;
    using value_list = std::list<value_type>;

    CProgressGroup(Wt::WApplication &, std::initializer_list<std::tuple<ID_t, ID_t, std::string>> &, std::string const &, std::string const &);
    ~CProgressGroup() = default;

    void beginStep();
    void updateStep(double, double); // The two double are just converted to a percentage. (Floats and all ints can convert to double)
    void updateStep(double);
    void completeStep();

  protected:
    Wt::WApplication &application;              // The application is needed to get the GUI lock for updating from outside the GUI thread.
    std::shared_ptr<CProgressGroupModel> model;
    std::string pendingText;
    std::string completeText;
    std::list<actionItem_t> items;
    std::list<actionItem_t>::iterator currentItem;
    Wt::WTreeView *treeView = nullptr;

    void createWidget();


  private:
    CProgressGroup() = delete;
    CProgressGroup(CProgressGroup const &) = delete;
    CProgressGroup(CProgressGroup &&) = delete;
    CProgressGroup &operator=(CProgressGroup const &) = delete;
    CProgressGroup &operator=(CProgressGroup &&) = delete;
};

#endif
