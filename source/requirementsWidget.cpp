//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                requirementsWidget.cpp
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
// HISTORY:             2024-03-15 GGB - File Created
//
//*********************************************************************************************************************************

#include "include/requirementsWidget.h"

// Standard C++ library headers
#include <functional>
#include <list>

// Wt++ header files
#include <Wt/WApplication.h>

// Miscellaneous library header files
#include <boost/locale.hpp>
#include <GCL>

class CRequirementsModel final : public Wt::WAbstractItemModel
{
public:
  using ID_t = CRequirementsWidget::ID_t;


  CRequirementsModel() {}
  virtual ~CRequirementsModel() = default;

  [[nodiscard]] bool allMet() const noexcept
  {
    bool rv = true;
    for (auto const &r: records_)
    {
      if (r.second.requirementType == CRequirementsWidget::REQUIRED)
      {
        rv = rv && r.second.available;
      }
    }
    return rv;
  }

  void setData(ID_t ID, std::string &&st, CRequirementsWidget::requirementType_e rt)
  {
    auto iter = records_.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(records_.size()),
                                 std::forward_as_tuple(ID, std::move(st), rt)).first;
    byID.emplace(ID, std::ref(iter->second));
    reset();
  }
  void setData(ID_t ID, std::string const &st, CRequirementsWidget::requirementType_e rt)
  {
    auto iter = records_.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(records_.size()),
                                 std::forward_as_tuple(ID, st, rt)).first;
    byID.emplace(ID, std::ref(iter->second));
    reset();
  }

  void changeStatus(ID_t ID, bool enable)
  {
    byID.at(ID).get().available = enable;
    reset();
  }

  void resetAll() noexcept
  {
    for (auto &record: records_)
    {
      record.second.available = false;
    }
    reset();
  }

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
          // Column Headers. The column headers need to be reversed and the highest (last) value needs to be dropped.

          switch (section)
          {
            case 0:
            {
              returnValue = "Requirement";
              break;
            };
            case 1:
            {
              returnValue = "Optional";
              break;
            };
            case 2:
            {
              returnValue = "Available";
              break;
            }
            default:
            {
              CODE_ERROR();
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
  virtual int rowCount(const Wt::WModelIndex &) const override { return records_.size(); }
  virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex &parent = Wt::WModelIndex()) const override
  {
    RUNTIME_ASSERT(column < 3, "CRequirementsWidget::index: Column number out of range.");
    RUNTIME_ASSERT(row < records_.size(), "CRequirementsWidget::index: Row number out of range.");
    return createIndex(row, column, nullptr);
  }

  virtual Wt::WModelIndex parent(const Wt::WModelIndex &) const override {return Wt::WModelIndex(); }
  virtual int columnCount(const Wt::WModelIndex &) const override { return 3; }


  virtual std::any data(const Wt::WModelIndex &index, Wt::ItemDataRole role) const override
  {
    std::any rv;

    std::size_t row = static_cast<std::size_t>(index.row());
    std::size_t column = static_cast<std::size_t>(index.column());

    switch (column)
    {
      case 0:
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            rv = records_.at(row).shortText;
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
            if (records_.at(row).requirementType == CRequirementsWidget::REQUIRED)
            {
              rv = boost::locale::translate("Required").str();
            }
            else if (records_.at(row).requirementType == CRequirementsWidget::OPTIONAL)
            {
              rv = boost::locale::translate("Optional").str();
            }
            else
            {
              CODE_ERROR();
              // Does not return.
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
      case 2:
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            if (records_.at(row).available)
            {
              rv = boost::locale::translate("Available").str();
            }
            else
            {
              rv = boost::locale::translate("Pending").str();
            }
            break;
          }
          default:
          {
            break;
          }
        }
        break;
      }
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
  CRequirementsModel(int rows, int columns) = delete;
  CRequirementsModel(CRequirementsModel const &) = delete;
  CRequirementsModel(CRequirementsModel &&) = delete;
  CRequirementsModel &operator=(CRequirementsModel const &) = delete;
  CRequirementsModel &operator=(CRequirementsModel &&) = delete;

  struct record_t
  {
    ID_t ID;
    std::string shortText;
    CRequirementsWidget::requirementType_e requirementType;
    bool available = false;;
  };
  std::map<std::size_t, record_t> records_;
  std::map<ID_t, std::reference_wrapper<record_t>> byID;

};


/// @brief      Class constructor.
/// @throws
/// @version    2024-03-15/GGB - Function created.

CRequirementsWidget::CRequirementsWidget(Wt::WApplication &a) : application(a)
{
  model = std::make_shared<CRequirementsModel>();
  createUI();
}

bool CRequirementsWidget::allMet() const noexcept
{
  return std::dynamic_pointer_cast<CRequirementsModel>(model)->allMet();
}

/// @brief      Sets up the UI aspects of the widget.
/// @throws
/// @version    2024-03-19/GGB - Function created.

void CRequirementsWidget::createUI()
{
  setLayoutSizeAware(true);
  setOverflow(Wt::Overflow::Scroll);

  setModel(model);
  setColumnResizeEnabled(true);
  setColumnAlignment(0, Wt::AlignmentFlag::Center);
  setHeaderAlignment(0, Wt::AlignmentFlag::Center);
  setAlternatingRowColors(true);
  setSelectionMode(Wt::SelectionMode::Single);
  setEditTriggers(Wt::EditTrigger::None);
}

/// @brief      Change the enabled status of the specified items.
/// @param[in]  ID: The item to update
/// @param[in]  enable: The desired status.

void CRequirementsWidget::changeStatus(ID_t ID, bool enable)
{
  if (Wt::WApplication::instance() == nullptr)
  {
    // Not in GUI thread.
    Wt::WApplication::UpdateLock uiLock(&application);

    if (uiLock)
    {
      std::dynamic_pointer_cast<CRequirementsModel>(model)->changeStatus(ID, enable);
    };
    application.triggerUpdate();
  }
  else
  {
    // IN GUI thread.
    std::dynamic_pointer_cast<CRequirementsModel>(model)->changeStatus(ID, enable);
  };

}


/// @brief      Inserts a new item into the model.
/// @param[in]  id: The ID of the item.
/// @param[in]  txt: The text to associate with the item.
/// @throws
/// @version    2024-03-15/GGB - Function created.

void CRequirementsWidget::insert(ID_t id, std::string &&txt, requirementType_e rt)
{
  std::dynamic_pointer_cast<CRequirementsModel>(model)->setData(id, std::move(txt), rt);

}

/// @brief      Inserts a new item into the model.
/// @param[in]  id: The ID of the item.
/// @param[in]  txt: The text to associate with the item.
/// @throws
/// @version    2024-03-15/GGB - Function created.

void CRequirementsWidget::insert(ID_t id, std::string const &txt, requirementType_e rt)
{
  std::dynamic_pointer_cast<CRequirementsModel>(model)->setData(id, txt, rt);
}

/// @brief      Inserts a list of items into the model.
/// @param[in]  id: The ID of the item.
/// @param[in]  txt: The text to associate with the item.
/// @throws
/// @version    2024-04-15/GGB - Function created.

void CRequirementsWidget::insert(std::initializer_list<std::tuple<ID_t, std::string, requirementType_e>> il)
{
  for (auto i : il)
  {
    std::dynamic_pointer_cast<CRequirementsModel>(model)->setData(std::get<0>(i), std::get<1>(i), std::get<2>(i));
  }
}

void CRequirementsWidget::resetAll() noexcept
{
  std::dynamic_pointer_cast<CRequirementsModel>(model)->resetAll();
}

