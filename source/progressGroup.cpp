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

  CProgressGroupModel(value_list &valueList) : records(valueList)
  {
    for (auto &val: records)
    {
      byID.emplace(val.get<0>(), std::ref(val));
    }

    SCL::parentChild<ID_t, value_type, 0, 1, value_list, false> parentChild(records);
    std::list<ID_t> idList = parentChild.preOrder_ID<std::list<ID_t>>(0);
    for (auto const &id: idList)
    {
      modelData[id] = std::make_tuple(parentChild.parent(id), 0, std::move(parentChild.children(id)));
    }

    // Set the row numbers. Just work through all the items and assign row numbers as required.
    // Note: it may be quicker to use a stack to track row numbers, but this is just as easy and speed is probably not a
    // requirement at this time.

    for (auto &item: modelData)
    {
      if (std::get<2>(item.second).size() != 0)
      {
        std::size_t row = 0;
        for (auto const &child: std::get<2>(item.second))
        {
          std::get<1>(modelData[child]) = row++;
        }
      }
    }
  }
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
        ID = std::get<2>(modelData.at(indx.internalId())).at(indx.row());
      }
    }
    return std::get<2>(modelData.at(ID)).size();
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
      PID = std::get<2>(modelData.at(parent.internalId())).at(parent.row());
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
      child_type const &item = modelData.at(indx.internalId());
      return createIndex(std::get<1>(item), 0, std::get<0>(item));
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

    ID_t valueID = std::get<2>(modelData.at(index.internalId()))[index.row()];

    switch (index.column())
    {
      case 0: // Display the item text.
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            rv = byID.at(valueID).get().itemText;
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

  using child_type = std::tuple<ID_t, int, std::vector<ID_t>>; // parentID, row, children
  value_list &records;
  std::map<ID_t, value_ref> byID;
  std::map<ID_t, child_type> modelData;    // MyID, <parentID, row>
};


/// Implements the delegate to display the progress bar or the text.
class CProgressItemDelegate : public Wt::WAbstractItemDelegate
{
public:
  CProgressItemDelegate(std::string const &pt, std::string const &ct) : pendingText(pt), completeText(ct) {}
  virtual ~CProgressItemDelegate() = default;

protected:
  virtual std::unique_ptr<Wt::WWidget> update(Wt::WWidget *widget, Wt::WModelIndex const &index, Wt::WFlags<Wt::ViewItemRenderFlag>)
  {
    std::unique_ptr<Wt::WWidget> rv;
    CProgressGroup::pointer data = std::any_cast<CProgressGroup::pointer>(index.data(Wt::ItemDataRole::User));

    CProgressGroupModel const *model = dynamic_cast<CProgressGroupModel const *>(index.model());

    switch(data->status)
    {
      case CProgressGroup::S_PENDING:
      {
        if (data->textEditStatus == nullptr)
        {
          rv = std::make_unique<Wt::WText>(pendingText);
          data->textEditStatus = dynamic_cast<Wt::WText *>(rv.get());
        }
        break;
      }
      case CProgressGroup::S_ACTIVE:
      {
        if (data->textEditStatus != nullptr)
        {
          data->textEditStatus = nullptr;
          rv = std::make_unique<Wt::WProgressBar>();
          data->progressBar = dynamic_cast<Wt::WProgressBar *>(rv.get());
        }
        break;
      }
      case CProgressGroup::S_COMPLETE:
      {
        if (data->progressBar != nullptr)
        {
          data->progressBar = nullptr;
          rv = std::make_unique<Wt::WText>(completeText);
          data->textEditStatus = dynamic_cast<Wt::WText *>(rv.get());
        }
        break;
      }
      default:
      {
        CODE_ERROR();
        // Does not return.
      }
    }
    return std::move(rv);
  }

private:
  CProgressItemDelegate() = delete;
  CProgressItemDelegate(CProgressItemDelegate const &) = delete;
  CProgressItemDelegate(CProgressItemDelegate &&) = delete;
  CProgressItemDelegate &operator=(CProgressItemDelegate const &) = delete;
  CProgressItemDelegate &operator=(CProgressItemDelegate &&) = delete;

  std::string const &pendingText;
  std::string const &completeText;
};

/// @brief      Class constructor. This is the only way to construct the class and ensures that all parameters are set.
/// @param[in]  a: The owning application. (Needed to synchronise/access the GUI thread)
/// @param[in]  actionList: All the actions to be associated with the progress bar.
/// @param[in]  il: The initialistation list. <itemID, itemText, initialText, finalText>
/// @param[in]  pt: The text to use for pending items.
/// @param[in]  ct: The text to use for completed items.
/// @throws     std::bad_alloc
/// @version    2024-04-29/GGB - Function created.

CProgressGroup::CProgressGroup(Wt::WApplication &a,
                               std::initializer_list<std::tuple<ID_t, ID_t, std::string>> &il,
                               std::string const &pt, std::string const &ct)
: application(a), pendingText(pt), completeText(ct)
{
  for (auto const &item: il)
  {
    items.emplace_back(std::get<0>(item), std::get<1>(item), std::get<2>(item), S_PENDING);
  }

  currentItem = items.begin();

  model = std::make_shared<CProgressGroupModel>(items);

  createWidget();
}


/// @brief      Begin the next step. This can also be called to begin the first step.
/// @note       completeStep() does not move onto the next step. It merely finishes the last step.
/// @throws     CODE_ERROR(),
/// @version    2024-04-29/GGB - Function created.

void CProgressGroup::beginStep()
{
  if (currentItem != items.end())
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
        // Does not return.
      }
    }
    else
    {
      CODE_ERROR();
      // Does not return.
    }
  }
  // Delete the text and replace with a progress bar
  // Get the session lock.

  Wt::WApplication::UpdateLock uiLock(&application);

  if (uiLock)
  {
    treeView->refresh();
  };
}

/// @brief      Creates the widget sub-widgets.
/// @throws
/// @version    2024-04-29/GGB - Function created.

void CProgressGroup::createWidget()
{
  setLayoutSizeAware(true);
  setOverflow(Wt::Overflow::Scroll);
  treeView = addWidget(std::make_unique<Wt::WTreeView>());
  treeView->setItemDelegateForColumn(1, std::make_shared<CProgressItemDelegate>(pendingText, completeText));

  model = std::make_shared<CProgressGroupModel>(items);
  treeView->setModel(model);
}

/// @brief    Marks a step as completed. Does not move onto the next step. Changes the progress bar to a text exit.
/// @throws
/// @version  2024-04-29/GGB - Function created.

void CProgressGroup::completeStep()
{
  if (currentItem != items.end())
  {
    if (currentItem->status != S_ACTIVE)
    {
      CODE_ERROR();
    }
    else
    {
      currentItem->status = S_COMPLETE;

      // Delete the progress bar and replace with text.
      // Get the session lock.
      Wt::WApplication::UpdateLock uiLock(&application);

      if (uiLock)
      {
        treeView->refresh();
      };
    }
  }
}

  /// @brief      Updates the progress bar. This will normally be done from outside the GUI thread.
  /// @param[in]  p: The percentage complete.
  /// @throws
  /// @version    2024-04-29/GGB - Function created.

  void CProgressGroup::updateStep(double p)
  {
    if (currentItem->progressBar != nullptr)
    {
      // Get the session lock.
      Wt::WApplication::UpdateLock uiLock(&application);

      if (uiLock)
      {
        currentItem->progressBar->setValue(p);
      }
    }
    else
    {
      ERRORMESSAGE("CProgressGroup: Commands out of order.");
    }
  }

  /// @brief      Updates the progress bar. This will normally be done from outside the GUI thread.
  /// @param[in]  p: The percentage complete.
  /// @throws
  /// @version    2024-04-29/GGB - Function created.

  void CProgressGroup::updateStep(double n, double d)
  {
    updateStep(n/d);
  }
