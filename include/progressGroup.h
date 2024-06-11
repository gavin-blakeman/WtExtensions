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
#include <array>
#include <atomic>
#include <cstdint>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <mutex>
#include <semaphore>
#include <string>
#include <tuple>
#include <variant>

// Wt Library
#include <Wt/WContainerWidget.h>
#include <Wt/WAbstractItemModel.h>

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
    enum updateEvent_e { U_BEGIN, U_COMPLETE, U_ADD, U_PROGRESS, };
    using actionID_t = std::uint32_t;

    using ID_t = std::uint64_t;
    using mutex_type = std::shared_mutex;
    using unique_lock = std::unique_lock<mutex_type>;
    using shared_lock = std::shared_lock<mutex_type>;
    using uAdd_t = std::tuple<actionID_t, actionID_t, std::string>;
    using uProgress_t = double_t;
    using updateVariant_t = std::variant<std::monostate, uAdd_t, uProgress_t>;
    enum status_e
    {
      S_NONE,         //
      S_PENDING,      // The pending text is displayed. IE this is a future action. The initialText is displayed.
      S_ACTIVE,       // This is the active action. There should only be one active action. When active, the progress bar is
                      // displayed.
      S_COMPLETE,     // The action is complete. The finalText is displayed.
    };
    struct actionItem_t;
    using value_ref = std::reference_wrapper<actionItem_t>;

    struct actionItem_t
    {
      // Basic data.
      ID_t const ID;                                // Application assigned ID.
      ID_t const PID;                               // Application provided PID.
      ID_t sortOrder;                               // Used for ordering the list of children.
      std::string const itemText;                   // Application provided item text.
      mutex_type mActionItem;

      // Model created Data.

      SCL::vector_sorted<value_ref> children;       // Ordered list of children.

      std::atomic<status_e> status = S_PENDING;     // Current status of the item
      Wt::WText *textEditStatus = nullptr;          // Active when status == Pending or complete.
      Wt::WProgressBar *progressBar = nullptr;      // Active when status == Active.
      std::atomic<double> progress{0};              // Progress measurement.
      std::atomic_flag updateRequired;
      std::atomic_flag recalcRequired;
      Wt::WModelIndex index;

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
        else if constexpr (N == 2)
        {
          return sortOrder;
        }
        else
        {
          CODE_ERROR();
        }
      }

      friend bool operator<(actionItem_t const &LHS, actionItem_t const &RHS) noexcept { return LHS.sortOrder < RHS.sortOrder; }
      friend bool operator<(value_ref const &LHS, value_ref const &RHS) noexcept { return LHS.get().sortOrder < RHS.get().sortOrder; }
    };
    using value_type = actionItem_t;
    using reference = value_type &;
    using const_reference = value_type const &;
    using pointer = value_type *;
    using const_pointer = value_type const *;
    using value_list = std::forward_list<value_type>;
    struct data_t
    {
      mutex_type mModel;
      mutex_type mData;                                 // To read or update any of the fields in data this mutex must be held.
      value_list actions;
      std::map<ID_t, value_ref> byID;
      std::atomic_flag recordsUpdated;
      std::string pendingText;
      std::string completeText;
      SCL::parentChild<ID_t, value_type, 0, 1, std::forward_list<value_type>, false> parentChild = {actions};
      std::list<value_ref> preOrderTree;
    };

    /*! @brief      Class constructor.
     *  @param[in]  application: The application that owns this instance.
     *  @param[in]  ps: The string to display for pending items.
     *  @param[in]  cs: The string to display for complete items.
     */
    CProgressGroup(Wt::WApplication &application, std::string const &ps, std::string const &cs);
    ~CProgressGroup() = default;

    /*! @brief      Begins a action step. Change the text to a progress bar.
     *  @param[in]  actionID: The action to begin.
     *  @throws
     */
    void beginStep(ID_t actionID);

    /*! @brief      Marks a step as completed. Does not move onto the next step. Changes the progress bar to a text edit.
     *  @param[in]  actionID: The action to begin.
     *  @throws
     */
    void completeStep(ID_t actionID);

    /*! @brief      Insert a range of actions.
     *  @param[in]  begin: The beginning of the range.
     *  @param[in]  end: The end of the range.
     *  @throws
     */
    template<typename Iter>
    void insertActions(Iter begin, Iter end)
    {
      for (; begin != end; begin++)
      {
        std::apply(std::bind_front(&CProgressGroup::insertAction, this), *begin);
      }
    }

    /*! @brief      Inserts an action.
     *  @parm[in]   actionID: The ID of the new action.
     *  @param[in]  parentID: The parentID of the new action. (0 = top level, multiple allowed)
     *  @param[in]  sortOrder: The sorting order of the item.
     *  @param[in]  actionText: The text to associate with the action.
     *  @throws     SIGNAL(1) if the action already exists.
     *  @throws     SIGNAL(2) if the PID does not exist. (PID 0 is always allowed.)
     */
    void insertAction(ID_t actionID, ID_t parentID, ID_t sortOrder, std::string const &actionText);

    /*! @brief      Callback function that can be caleld to update the progress group,
     *  @param[in]  actionID: The action to update.
     *  @param[in]  updateEvent: The update event.
     *  @param[in]  updateData: The update data.
     */
    void updateProgress(ID_t, updateEvent_e updateEvent, updateVariant_t const *updateData = nullptr);

    /*! @brief      Updates an action step.
     *  @param[in]  actionID: The action to begin.
     *  @param[in]  num: The numerator of the action progress.
     *  @param[in]  denom: The denominator of the action progress.
     *  @throws
     */
    void updateStep(ID_t actionID, std::uint64_t num, std::uint64_t denom);

    /*! @brief      Updates an action step.
     *  @param[in]  actionID: The action to begin.
     *  @param[in]  per: The percentage complete.
     *  @throws
     */
    void updateStep(ID_t actionID, double per);

  protected:
    std::shared_ptr<CProgressGroupModel> model;
    data_t data;
    Wt::WTreeView *treeView = nullptr;

    /*! @brief    Creates the subwidgets for the progressGroup.
     *  @throws
     */
    void createWidget();


  private:
    CProgressGroup() = delete;
    CProgressGroup(CProgressGroup const &) = delete;
    CProgressGroup(CProgressGroup &&) = delete;
    CProgressGroup &operator=(CProgressGroup const &) = delete;
    CProgressGroup &operator=(CProgressGroup &&) = delete;

    Wt::WApplication &application;
    std::atomic_flag terminateThread;
    std::atomic_flag updatesReceived;
    std::binary_semaphore sUpdateRequired{0};
    std::uint16_t updatePeriod = 1;

    /*! @brief      The thread that is used to update the GUI periodically.
     */
    void updateThread();
};

#endif
