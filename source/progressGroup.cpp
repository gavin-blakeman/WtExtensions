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

// Standard C++ library header files
#include <vector>

// Wt++ header files
#include <Wt/WAbstractItemDelegate.h>
#include <Wt/WAbstractItemModel.h>
#include <Wt/WApplication.h>
#include <Wt/WFlags.h>
#include <Wt/WModelIndex.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTreeView.h>
#include <Wt/WText.h>
#include <Wt/WWidget.h>

// Miscellaneous libraries
#include <boost/locale.hpp>
#include <GCL>

/* Multi-threading
 * The classes need to support multi-threading. The class maintains it's own thread to handle the GUI updates.
 * The class may also be called by multi-threaded applications. As a result some of the functions need protection while others
 * don't.
 * The class does need to support multi-threading, no static members, re-entrant functions.
 *
 * These classes need protection for both the data and the individual actions.
 * In theory the data can be accessed by the following (1 Writer, Many readers.)
 * > addAction - Update value_list, byID and parentChild trees.
 * > updateThread - Read value_list and byID
 * > callbackFunction - read value_list and byID
 * > Reload model
 * The action data can be accessed by the following:
 * > updateThread - Read and write data. Writing data could all be assigned to atomics.
 * > callbackFunction - write data. Could be done using atomics.
 * > delegate - read/write data - Could be done with atomics.
 * > model - read data - atomics ok.
 * The action data does not require a mutex if all the mutable data can be stored in atomics, including the pointer. Noting that
 * atomics may be implemented using locks.
 */


/* The CProgressGroupModel handles providing the parent/child hierarchy to the treeView. The model takes the list provided and
 * prepares the preOrder vector.
 *
 * As this is a multi-level tree, the model is more complex than a flat tree. Each time an item is added to the tree, the model
 * needs to be re-organised.
 */

class CProgressGroupModel final : public Wt::WAbstractItemModel
{
public:
  using ID_t = CProgressGroup::ID_t;
  using value_type = CProgressGroup::value_type;
  using value_ref = std::reference_wrapper<value_type>;
  using value_list = CProgressGroup::value_list;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using mutex_type = CProgressGroup::mutex_type;
  using unique_lock = CProgressGroup::unique_lock;
  using shared_lock = CProgressGroup::shared_lock;
  using data_type = CProgressGroup::data_t;

  /*! @brief      Constructor.
   *  @param[in]  d: The data for the model to manage.
   *  @throws
   */
  CProgressGroupModel(data_type &d) : data_(d)
  {
  }

  /*! @brief    Class destructor.
   */
  virtual ~CProgressGroupModel() = default;

protected:
  virtual std::any headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const override
  {
    std::any returnValue;

    if (orientation == Wt::Orientation::Horizontal)
    {
      switch (role.value())
      {
        case Wt::ItemDataRole::Display:
        {
          switch (section)
          {
            case 0:
            {
              returnValue = static_cast<std::string>(boost::locale::translate("Action"));
              break;
            };
            case 1:
            {
              returnValue = static_cast<std::string>(boost::locale::translate("Status"));
              break;
            };
            default:
            {
              CODE_ERROR();
              // Does not return.
            }
          };
          break;
        };
      };
    }
    else
    {
      CODE_ERROR();
    };

    return returnValue;
  }

  /// @brief      Return the row count for the specified indx.
  /// @param[in]  indx: The index to retrieve the number of rows for.
  /// @throws     Can throw on an invalid index.
  /// @version    2024-04-29/GGB - Function created.

  virtual int rowCount(const Wt::WModelIndex &indx) const override
  {
    /*    +---------------------+-----------+--------+------------+---------+-----------------+
     *    | Thread Group        | Call      | mData  | mActonItem | mModel  | sUpdateRequired |
     *    |---------------------|-----------+--------+------------+---------+-----------------+
     *    | 1. Outside Threads  |  NO       |        |            |         |                 |
     *    | 2. Update Thread    |  NO       |        |            |         |                 |
     *    | 3. GUI Thread       |  YES      | SHARED |   SHARED   |         |                 |
     *    +---------------------+-----------+--------+------------+---------+-----------------+
     */

    int rv;

    std::uint64_t ID;
    if (!indx.isValid())
    {
      ID = 0;
    }
    else
    {
      if (indx.column() != 0)
      {
        return 0;
      }
      else
      {
        shared_lock sl{data_.mData};
        CProgressGroup::actionItem_t &actionItem = data_.byID.at(indx.internalId());
        sl.unlock();
        shared_lock slAI1{actionItem.mActionItem};
        ID = actionItem.children.at(indx.row()).get().ID;
        slAI1.unlock();
        sl.lock();
        CProgressGroup::actionItem_t &actionItem2 = data_.byID.at(ID);
        sl.unlock();
        shared_lock slAI2{actionItem2.mActionItem};
        rv = actionItem2.children.size();
      }
    }


    return rv;
  }

  /// @brief      Creates an index for the specified item.
  /// @param[in]  row: The row of the index.
  /// @param[in]  column: The column of the index.
  /// @param[in]  parent: The parent item.
  /// @throws
  /// @version    2024-04-29/GGB - Function created.

  virtual Wt::WModelIndex index(int row, int col, const Wt::WModelIndex &parent = Wt::WModelIndex()) const override
  {
    /*    +---------------------+-----------+--------+--------------+---------+-----------------+
     *    | Thread Group        | Call      | mData  | mActionItem  | mModel  | sUpdateRequired |
     *    |---------------------|-----------+--------+--------------+---------+-----------------+
     *    | 1. Outside Threads  |  NO       |        |              |         |                 |
     *    | 2. Update Thread    |  NO       |        |              |         |                 |
     *    | 3. GUI Thread       |  YES      | SHARED |   SHARED     |         |                 |
     *    +---------------------+-----------+--------+--------------+---------+-----------------+
     */

    int PID = 0;

    if (parent.isValid())
    {
      shared_lock sl{data_.mData};
      CProgressGroup::actionItem_t &actionItem = data_.byID.at(parent.internalId()).get();
      sl.unlock();
      shared_lock slAI{actionItem.mActionItem};
      PID = actionItem.children.at(parent.row()).get().ID;
    }

    return createIndex(row, col, PID);
  }

  /// @brief      Returns an index representing the parent of the specified index.
  /// @param[in]  indx: The specified index.
  /// @returns    A model index representing the parent.
  /// @throws
  /// @version    2024-04-29/GGB - Function created.

  virtual Wt::WModelIndex parent(const Wt::WModelIndex &indx) const override
  {
    /*    +---------------------+-----------+--------+--------------+---------+-----------------+
     *    | Thread Group        | Call      | mData  | mActionItem  | mModel  | sUpdateRequired |
     *    |---------------------|-----------+--------+--------------+---------+-----------------+
     *    | 1. Outside Threads  |  NO       |        |              |         |                 |
     *    | 2. Update Thread    |  NO       |        |              |         |                 |
     *    | 3. GUI Thread       |  YES      | SHARED |   SHARED     |         |                 |
     *    +---------------------+-----------+--------+--------------+---------+-----------------+
     */

    if (!indx.isValid() || indx.internalId() == 0)
    {
      return Wt::WModelIndex();   // Root of the tree.
    }
    else
    {
      shared_lock sl{data_.mData};
      CProgressGroup::actionItem_t &actionItem = data_.byID.at(indx.internalId()).get();
      sl.unlock();

      shared_lock slAI{actionItem.mActionItem};
      return createIndex(actionItem.PID, 0, actionItem.ID);
    }
  }

  /// @brief      Returns the number of columns. This is always 2.
  /// @param[in]  ignored
  /// @returns    2.
  /// @throws     noexcept
  /// @version    2024-04-29/GGB - Function created.

  virtual int columnCount(const Wt::WModelIndex &) const noexcept override { return 2; }

  /// @brief      Returns the data for the specified model index.
  /// @param[in]  index: The index to return the data for
  /// @param[in]  role: The role to return.

  virtual std::any data(const Wt::WModelIndex &index, Wt::ItemDataRole role) const override
  {
    /*    +---------------------+-----------+--------+--------------+---------+-----------------+
     *    | Thread Group        | Call      | mData  | mActionItem  | mModel  | sUpdateRequired |
     *    |---------------------|-----------+--------+--------------+---------+-----------------+
     *    | 1. Outside Threads  |  NO       |        |              |         |                 |
     *    | 2. Update Thread    |  NO       |        |              |         |                 |
     *    | 3. GUI Thread       |  YES      | SHARED |   SHARED     |         |                 |
     *    +---------------------+-----------+--------+--------------+---------+-----------------+
     */

    shared_lock sl{data_.mData};
    CProgressGroup::actionItem_t &actionItem = data_.byID.at(index.internalId()).get();
    sl.unlock();
    shared_lock slAI{actionItem.mActionItem};
    CProgressGroup::actionItem_t &actionItem2 = actionItem.children.at(index.row()).get();
    slAI.unlock();
    shared_lock slAI2{actionItem2.mActionItem};

    std::any rv;
    switch (index.column())
    {
      case 0: // Display the item text.
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            rv = actionItem2.itemText;
          }
          default:
          {
            //CODE_ERROR();
            break;
          }
        }
        break;
      }
      case 1:
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            switch (actionItem2.status)
            {
              case CProgressGroup::S_PENDING:
              {
                //rv = parent.initialText;
                break;
              }
              case CProgressGroup::S_COMPLETE:
              {
                //rv = parent.finalText;
                break;
              }
              default:
              {
                CODE_ERROR();
                // Does not return.
              }
            }
            break;
          }
          default:
          {
            break;
          };
        }
        break;
      };
      default:
      {
        CODE_ERROR();
        // Does not return.
      }
    }
    return rv;
  }

  virtual Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex &index) const override { return Wt::WFlags<Wt::ItemFlag>(); }

private:
  CProgressGroupModel(int rows, int columns) = delete;
  CProgressGroupModel(CProgressGroupModel const &) = delete;
  CProgressGroupModel(CProgressGroupModel &&) = delete;
  CProgressGroupModel &operator=(CProgressGroupModel const &) = delete;
  CProgressGroupModel &operator=(CProgressGroupModel &&) = delete;

  data_type &data_;                                          // Refers to the list maintained by the progress group.
};


/*! @class  Implements the delegate to display the progress bar or the text.
 */
class CProgressItemDelegate : public Wt::WAbstractItemDelegate
{
public:
  using mutex_type = CProgressGroup::mutex_type;
  using unique_lock = CProgressGroup::unique_lock;
  using shared_lock = CProgressGroup::shared_lock;

  CProgressItemDelegate() = default;
  virtual ~CProgressItemDelegate() = default;

protected:
  virtual std::unique_ptr<Wt::WWidget> update(Wt::WWidget *widget, Wt::WModelIndex const &index, Wt::WFlags<Wt::ViewItemRenderFlag>)
  {
    std::unique_ptr<Wt::WWidget> rv;

//    const_pointer d = std::any_cast<const_pointer>(index.data(Wt::ItemDataRole::User));
//    pointer data = const_cast<pointer>(d);    // Need a non-const pointer to change values.

//    shared_lock sl{data->mActionItem};
//    switch(data->status)  // shared_lock
//    {
//      case CProgressGroup::S_PENDING:
//      {
//        if (data->textEditStatus == nullptr) // shared_lock
//        {
//          rv = std::make_unique<Wt::WText>(pendingText);  // shared_lock
//          sl.unlock();
//          unique_lock ul{data->mActionItem};
//          data->textEditStatus = dynamic_cast<Wt::WText *>(rv.get());
//        } // unique_lock unlocks
//        break;
//      }
//      case CProgressGroup::S_ACTIVE:
//      {
//        if (data->textEditStatus != nullptr)  // shared_lock
//        {
//          sl.unlock();
//          unique_lock ul{data->mActionItem};
//          data->textEditStatus = nullptr;
//          rv = std::make_unique<Wt::WProgressBar>();
//          data->progressBar = dynamic_cast<Wt::WProgressBar *>(rv.get());
//
//        } // unique_lock unlocks
//        sl.lock();
//        if (data->progressBar && !data->updateRequired.test_and_set())
//        {
//          if (data->progress.holds_value<double>)
//          {
//            data->progressBar->setValue(std::get<double>(data->progress))
//          }
//          else if (data->progress.holds_value<std::array<std::uint64_t>[2])
//          {
//            data->progressBar->setValue(std::get<std::array<std::uint64_t>[2]>(data->progress));
//          }
//          else
//          {
//            // Not an error. May not have been updated yet.
//          }
//          sl.unlock();
//        }
//        break;
//      }
//      case CProgressGroup::S_COMPLETE:
//      {
//        if (data->progressBar != nullptr) // shared_lock
//        {
//          sl.unlock();
//          unique_lock ul{data->mActionItem};
//          data->progressBar = nullptr;
//          rv = std::make_unique<Wt::WText>(completeText);
//          data->textEditStatus = dynamic_cast<Wt::WText *>(rv.get());
//        } // unique_lock unlocks
//        break;
//      }
//      default:
//      {
//        CODE_ERROR();
//        // Does not return.
//      }
//    }
    return std::move(rv);
  }

private:
  CProgressItemDelegate(CProgressItemDelegate const &) = delete;
  CProgressItemDelegate(CProgressItemDelegate &&) = delete;
  CProgressItemDelegate &operator=(CProgressItemDelegate const &) = delete;
  CProgressItemDelegate &operator=(CProgressItemDelegate &&) = delete;
};

CProgressGroup::CProgressGroup(Wt::WApplication &a, std::string const &pt, std::string const &ct) : application(a)
{
  data.pendingText = pt;
  data.completeText = ct;
  model = std::make_shared<CProgressGroupModel>(data);

  createWidget();
}

void CProgressGroup::beginStep(ID_t actionID)
{
  /*    +---------------------+-----------+--------+------------+---------+-----------------+
   *    | Thread Group        | Call      | mData  | mActonItem | mModel  | sUpdateRequired |
   *    |---------------------|-----------+--------+------------+---------+-----------------+
   *    | 1. Outside Threads  |  YES      | SHARED | UNIQUE     | UNIQUE  |                 |
   *    | 2. Update Thread    |  NO       |        |            |         |                 |
   *    | 3. GUI Thread       | POSSIBLE  | SHARED |            | UNIQUE  |                 |
   *    +---------------------+-----------+--------+------------+---------+-----------------+
   */

  unique_lock ul{data.mData};

  if (data.byID.contains(actionID))
  {
    actionItem_t &actionItem = data.byID.at(actionID).get();
    ul.unlock();
    unique_lock ulAI{actionItem.mActionItem};
    actionItem.status.store(S_ACTIVE);
    ulAI.unlock();
    unique_lock ulM{data.mModel};
    //model->updateAction(actionID);
  }
  else
  {
    CODE_ERROR();
    // Does not return.
  }

}

void CProgressGroup::createWidget()
{
  setLayoutSizeAware(true);
  setOverflow(Wt::Overflow::Scroll);
  treeView = addWidget(std::make_unique<Wt::WTreeView>());
  treeView->setItemDelegateForColumn(1, std::make_shared<CProgressItemDelegate>());
  treeView->setModel(model);
}

void CProgressGroup::completeStep(ID_t actionID)
{
  /*    +---------------------+-----------+--------+------------+---------+-----------------+
   *    | Thread Group        | Call      | mData  | mActonItem | mModel  | sUpdateRequired |
   *    |---------------------|-----------+--------+------------+---------+-----------------+
   *    | 1. Outside Threads  |  YES      | SHARED | UNIQUE     | UNIQUE  |                 |
   *    | 2. Update Thread    |  NO       |        |            |         |                 |
   *    | 3. GUI Thread       | POSSIBLE  | SHARED |            | UNIQUE  |                 |
   *    +---------------------+-----------+--------+------------+---------+-----------------+
   */

  unique_lock ul{data.mData};

  if (data.byID.contains(actionID))
  {
    actionItem_t &actionItem = data.byID.at(actionID).get();
    ul.unlock();
    unique_lock ulAI{actionItem.mActionItem};
    actionItem.status.store(S_COMPLETE);
    ulAI.unlock();
    unique_lock ulM{data.mModel};
    //model->updateAction(actionID);
  }
  else
  {
    CODE_ERROR();
    // Does not return
  }
}

void CProgressGroup::insertAction(ID_t actionID, ID_t parentID, ID_t sortOrder, std::string const &actionText)
{
  /*    +---------------------+-----------+--------+------------+---------+-----------------+
   *    | Thread Group        | Call      | mData  | mActonItem | mModel  | sUpdateRequired |
   *    |---------------------|-----------+--------+------------+---------+-----------------+
   *    | 1. Outside Threads  |  YES      | UNIQUE | UNIQUE     | UNIQUE  |                 |
   *    | 2. Update Thread    |  NO       |        |            |         |                 |
   *    | 3. GUI Thread       | POSSIBLE  | UNIQUE |            | UNIQUE  |                 |
   *    +---------------------+-----------+--------+------------+---------+-----------------+
   */

  unique_lock ul{data.mData};
  actionItem_t &actionItem = data.actions.emplace_front(actionID, parentID, sortOrder, actionText);
  data.byID.emplace(actionID, std::ref(actionItem));
  actionItem_t &parentItem = data.byID.at(actionItem.PID).get();
  data.parentChild.clear();  // Invalidate the tree.
  ul.unlock();

  unique_lock ulPI{parentItem.mActionItem};
  //parentItem.children.emplace(std::ref(actionItem));
  ulPI.unlock();

  data.recordsUpdated.test_and_set();
}

void CProgressGroup::updateProgress(ID_t actionID, updateEvent_e updateEvent, updateVariant_t const *updateData)
{
  switch(updateEvent)
  {
    case U_BEGIN:
    {
      beginStep(actionID);
      break;
    }
    case U_PROGRESS:
    {
      uProgress_t progress = std::get<uProgress_t>(*updateData);
      updateStep(actionID, progress);
      break;
    }
    case U_COMPLETE:
    {
      completeStep(actionID);
      break;
    }
    case U_ADD:
    {
      uAdd_t data = std::get<uAdd_t>(*updateData);
      //std::apply(std::bind_front(&CProgressGroup::insertAction, this, data));
      break;
    }
    default:
    {
      CODE_ERROR();
      // Does not return.
    }
  }
}

  void CProgressGroup::updateStep(ID_t ID, double p)
  {
    /*    +---------------------+-----------+--------+------------+-----------------+
     *    | Thread Group        | Call      | mData  | mActonItem | sUpdateRequired |
     *    |---------------------|-----------+--------+------------+-----------------+
     *    | 1. Outside Threads  |  YES      | SHARED | UNIQUE     |  RELEASE        |
     *    | 2. Update Thread    |  NO       |        |            |                 |
     *    | 3. GUI Thread       | POSSIBLE  | SHARED |            |  RELEASE        |
     *    +---------------------+-----------+--------+------------+-----------------+
     */

     shared_lock sl{data.mData};

     if (data.byID.contains(ID))
     {
       actionItem_t &actionItem = data.byID.at(ID).get();
       sl.unlock();
       unique_lock ulAI{actionItem.mActionItem};
       actionItem.progress = p;
       sUpdateRequired.release();
     }
     else
     {
       sl.unlock();
       CODE_ERROR();
       // Does not return.
     }
  }

  void CProgressGroup::updateStep(ID_t ID, std::uint64_t n, std::uint64_t d)
  {
    updateStep(ID, n/d);
  }

  void CProgressGroup::updateThread()
  {
    /* Updating requires that updates be rolled up the tree. So any update on any item requires that sections of the tree be
     * recalculated.
     * This is likely going to be an expensive operation as the actions need to be locked for most of the process.
     * Updating the tree can either be brute-forced. IE recalculate from bottom to top. This ensures each node is visited once and
     * the tree updates correctly. O(N)
     * There are some potential alternative approaches available as nodes that have been updated are flagged.
     * > Iterate the tree from bottom to top, only updating where required. (node->parent->parent). This is not ideal as the
     *   worst case timing is O(N!).
     * > Another approach is to pre-process the tree. IE visit each node and create a subset of nodes tha need update. IE capture
     *   the child->parent->parent nodes. A node cannot be added to the tree twice.
     *   The subset can then be preOrdered and processed. This avoids calculating unnecessary nodes, but the pre-processing could
     *   be expensive. preProcessing O(N) postProcessing O(<N), worst case O(2N), best case O(N).
     * Based on this, probably the best approach is just to recalculate all.
     * However, give each node a flag. preOrder iteratate. If the child has changed then force the parent to update. Updating a
     * parent will fetch the updates from each child. So flag the children as update complete and then flag the parent as update
     * required. This will allow quicker tree propogation and leave the tree with all flags reset at the end. Using an atomic_flag
     * reduces the need to lock the records.
     */

  /*    +---------------------+-----------+--------+------------+---------+-----------------+
   *    | Thread Group        | Call      | mData  | mActonItem | mModel  | sUpdateRequired |
   *    |---------------------|-----------+--------+------------+---------+-----------------+
   *    | 1. Outside Threads  |   NO      |        |            |         |                 |
   *    | 2. Update Thread    |  YES      |        |            |         |     aquire()    |
   *    | 3. GUI Thread       |   NO      |        |            |         |                 |
   *    +---------------------+-----------+--------+------------+---------+-----------------+
   */

    while (terminateThread.test_and_set())
    {
      std::this_thread::sleep_for(std::chrono::seconds(updatePeriod));
      sUpdateRequired.acquire();

      shared_lock slData{data.mData};
      for (auto &node: data.preOrderTree)
      {
        if (node.get().recalcRequired.test_and_set())
        {
          node.get().updateRequired.clear();
          ID_t nodeID = data.byID.at(node.get().ID).get().PID;

          // Recalculate the parent.
          std::vector<ID_t> children = data.parentChild.children(nodeID);
          double progress = 0.0;
          for (auto const &child: children)
          {
            progress += data.byID.at(child).get().progress;
            if (!data.byID.at(child).get().recalcRequired.test_and_set())      // Children won't force a recalc.
            {
              data.byID.at(child).get().updateRequired.clear();
            }
          };
          data.byID.at(nodeID).get().progress.store(progress / children.size());
          data.byID.at(nodeID).get().recalcRequired.clear();     // Force a recalc of the parent's parent.
        }
        Wt::WApplication *application = Wt::WApplication::instance(); // Get this instance.
      }
    }
  }
