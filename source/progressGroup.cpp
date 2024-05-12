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
 * > updateThread - Read and write data. Writing data could all be assinged to atomics.
 * > callbackFunction - write data. Could be done using atomics.
 * > delegate - read/write data - Could be done with atomics.
 * > model - read data - atomics ok.
 * The action data does not require a mutex if all the mutable data can be stored in atomics, including the pointer. Noting that
 * atomics may be implemeted using locks.
 */


/* The CProgressGroupModel handles providing the parent/child hierarchy to the treeView. The model takes the list provided and
 * prepares the preOrder vector.
 *
 * The model needs to be stored to support the retrieval required by the tree view.
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
//    std::list<ID_t> idList = parentChild.preOrder_ID<std::list<ID_t>>(0);
//    for (auto const &id: idList)
//    {
//      modelData[id] = std::make_tuple(parentChild.parent(id), 0, std::move(parentChild.children(id)));
//    }
//
//    // Set the row numbers. Just work through all the items and assign row numbers as required.
//    // Note: it may be quicker to use a stack to track row numbers, but this is just as easy and speed is probably not a
//    // requirement at this time.
//
//    for (auto &item: modelData)
//    {
//      if (std::get<2>(item.second).size() != 0)
//      {
//        std::size_t row = 0;
//        for (auto const &child: std::get<2>(item.second))
//        {
//          std::get<1>(modelData[child]) = row++;
//        }
//      }
//    }
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
              returnValue = boost::locale::translate("Action");
              break;
            };
            case 1:
            {
              returnValue = boost::locale::translate("Status");
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
        //ID = std::get<2>(data_.at(indx.internalId())).at(indx.row());
      }
    }
    //return std::get<2>(data_.at(ID)).size();
  }

  /// @brief      Creates an index for the specified item.
  /// @param[in]  row: The row of the index.
  /// @param[in]  column: The column of the index.
  /// @param[in]  parent: The parent item.
  /// @throws
  /// @version    2024-04-29/GGB - Function created.

  virtual Wt::WModelIndex index(int row, int col, const Wt::WModelIndex &parent = Wt::WModelIndex()) const override
  {
    int PID = 0;

    if (parent.isValid())
    {
      //PID = std::get<2>(modelData.at(parent.internalId())).at(parent.row());
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
    if (!indx.isValid() || indx.internalId() == 0)
    {
      return Wt::WModelIndex();   // Root of the tree.
    }
    else
    {
//      child_type const &item = data_.byID.at(indx.internalId());
//      return createIndex(std::get<1>(item), 0, std::get<0>(item));
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
    std::any rv;

    //ID_t valueID = std::get<2>(modelData.at(index.internalId()))[index.row()];

    switch (index.column())
    {
      case 0: // Display the item text.
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            //rv = data_.byID.at(valueID).get().itemText;
          }
          default:
          {
            //CODE_ERROR();
            break;
          }
        }
        break;
      }
//      case 1:
//      {
//        switch (role.value())
//        {
//          case Wt::ItemDataRole::Display:
//          {
//            switch (valueRef.status)
//            {
//              case S_PENDING:
//              {
//                rv = parent.initialText;
//                break;
//              }
//              case S_COMPLETE:
//              {
//                rv = parent.finalText;
//                break;
//              }
//              default:
//              {
//                CODE_ERROR();
//                // Does not return.
//              }
//            }
//            break;
//          }
//          default:
//          {
//            break;
//          };
//        }
//        break;
//      };
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

  using child_type = std::tuple<ID_t, int, std::vector<ID_t>>;  // parentID, row, children
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

CProgressGroup::CProgressGroup(std::string const &pt, std::string const &ct)
{
  data.pendingText = pt;
  data.completeText = ct;
  model = std::make_shared<CProgressGroupModel>(data);

  createWidget();
}

void CProgressGroup::beginStep(ID_t actionID)
{
  unique_lock ul{data.mData};
  data.byID.at(actionID).get().status.store(S_ACTIVE);
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
  unique_lock ul{data.mData};
  data.byID.at(actionID).get().status.store(S_COMPLETE);
}

void CProgressGroup::insertAction(ID_t actionID, ID_t parentID, std::string const &actionText)
{
  unique_lock ul{data.mData};
  data.actions.emplace_front(actionID, parentID, actionText);

  data.byID.emplace(actionID, std::ref(data.actions.front()));
  ul.unlock();
  data.recordsUpdated.clear();
}

  void CProgressGroup::updateStep(ID_t ID, double p)
  {
//    if (currentItem->progressBar != nullptr)
//    {
//      // Get the session lock.
//      Wt::WApplication::UpdateLock uiLock(&application);
//
//      if (uiLock)
//      {
//        //currentItem->progressBar->setValue(p);
//      }
//    }
//    else
//    {
//      ERRORMESSAGE("CProgressGroup: Commands out of order.");
//    }
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
     * the tree updates correctly. (N)
     * There are some potential alternative approaches available as nodes that have been updated are flagged.
     * > Iterate the tree from bottom to top, only updating where required. (node->parent->parent). This is not ideal as the
     *   worst case timing in (N!).
     * > Another approach is to pre-process the tree. IE visit each node and create a subset of nodes tha need update. IE capture
     *   the child->parent->parent nodes. A node cannot be added to the tree twice.
     *   The subset can then be preOrdered and processed. This avoids calculating unnecessary nodes, but the pre-processing could
     *   be expensice. preProcessing (N) postProcessing (<N) worst case (2N), best case (N)
     * Based on this, probably the best approach is just to recalculate all.
     * However, give each node a flag. preOrder iteratate. If the child has changed then force the parent to update. Updating a
     * parent will fetch the updates from each child. So flag the children as update complete and then flag the parent as update
     * required. This will allow quicker tree propogation and leave the tree with all flags reset at the end. Using an atomic_flag
     * reduces the need to lock the records.
     *
     * Note: The tree is also recalculated after additions, but that is to determine the tree structure.
     */

    while (terminateThread.test_and_set())
    {
      std::this_thread::sleep_for(std::chrono::seconds(updatePeriod));
      if (!updatesReceived.test_and_set())
      {
        shared_lock slData{data.mData}; // Need a shared lock on the data. (Prevent any updates of the tree.)
        for (auto &node: data.preOrderTree)
        {
          if (!node.get().recalcRequired.test_and_set())
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
  }
