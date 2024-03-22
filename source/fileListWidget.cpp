//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                fileListWidget.cpp
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

#include "include/fileListWidget.h"

// Standard C++ l;ibrary
#include <map>

// Miscellaneous library header files
#include <boost/locale.hpp>
#include <GCL>



class CFileListModel final : public Wt::WAbstractItemModel
{
public:
  using ID_t = CFileListWidget::ID_t;


  CFileListModel() {}
  virtual ~CFileListModel() = default;

  void setData(ID_t ID, std::filesystem::path &&st, std::string &&ft)
  {
    auto iter = records_.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(records_.size()),
                                 std::forward_as_tuple(ID, std::move(st), std::move(ft))).first;
    byID.emplace(ID, std::ref(iter->second));
    reset();
  }
  void setData(ID_t ID, std::filesystem::path const &st, std::string const &ft)
  {
    auto iter = records_.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(records_.size()),
                                 std::forward_as_tuple(ID, st, ft)).first;
    byID.emplace(ID, std::ref(iter->second));
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
              returnValue = boost::locale::translate("File Name").str();
              break;
            };
            case 1:
            {
              returnValue = boost::locale::translate("File Type").str();
              break;
            };
            case 2:
            {
              //returnValue = boost::locale::translate("Available").str();
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
            rv = static_cast<std::string>(records_.at(row).fileName.filename());
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
            rv = records_.at(row).fileType;
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
  //virtual bool setData(Wt::WModelIndex const &index, std::any const &value, Wt::ItemDataRole role) override;

private:
  CFileListModel(int rows, int columns) = delete;
  CFileListModel(CFileListModel const &) = delete;
  CFileListModel(CFileListModel &&) = delete;
  CFileListModel &operator=(CFileListModel const &) = delete;
  CFileListModel &operator=(CFileListModel &&) = delete;

  struct record_t
  {
    ID_t ID;
    std::filesystem::path fileName;
    std::string fileType;
  };
  std::map<std::size_t, record_t> records_;
  std::map<ID_t, std::reference_wrapper<record_t>> byID;

};


CFileListWidget::CFileListWidget()
{
  model = std::make_shared<CFileListModel>();
  createUI();
}

/// @brief      Sets up the UI aspects of the widget.
/// @throws
/// @version    2024-03-19/GGB - Function created.

void CFileListWidget::createUI()
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

/// @brief      Inserts a new item into the list.
/// @param[in]  fn: The filename to insert.
/// @param[in]  ft: The file type.

void CFileListWidget::insert(ID_t ID, std::filesystem::path const &fn, std::string const &ft)
{
  std::dynamic_pointer_cast<CFileListModel>(model)->setData(ID, fn, ft);
}

/// @brief      Inserts a new item into the list.
/// @param[in]  fn: The filename to insert.
/// @param[in]  ft: The file type.

void CFileListWidget::insert(ID_t ID, std::filesystem::path &&fn, std::string &&ft)
{
  std::dynamic_pointer_cast<CFileListModel>(model)->setData(ID, std::move(fn), std::move(ft));
}
