//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                fileUploadWidget.h
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
// HISTORY:             2024-05-07 GGB - File Created
//
//*********************************************************************************************************************************

#include "include/fileUploadWidget.h"

// Standard C++ library header files
#include <list>
#include <map>

// Wt++ header files
#include <Wt/WAbstractItemDelegate.h>
#include <Wt/WAbstractItemModel.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTableView.h>
#include <Wt/WText.h>

// Miscellaneous library header files
#include <fmt/format.h>
#include <GCL>
#include <boost/locale.hpp>

/* NOTES: 1. The File member of Wt::WFileDropWdget has a private member id_ that can be accesssed through a public (but undocumented
 *           function) uploadID().
 *        2. The File member is stored in the vector of Files as a std::unique_ptr. So the address of the file object should not
 *           change.
 */

class CFileListModel final : public Wt::WAbstractItemModel
{
public:
  using row_t = std::size_t;
  using ID_t = CFileUploadWidget::ID_t;
  struct record_t
  {
    ID_t ID;
    row_t row;
    Wt::WFileDropWidget::File *file;
    CFileUploadWidget::status_e status;
    std::string fileType;
    Wt::WText *textEditStatus = nullptr;
    Wt::WProgressBar *progressBar = nullptr;
    std::uint64_t dataReceived[2] = {0, 0};
    Wt::Signals::connection dataReceivedConnection;
  };
  using value_type = record_t;
  using reference = value_type &;
  using const_reference = value_type const &;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using value_list = std::list<value_type>;
  using value_ref = std::reference_wrapper<value_type>;

  CFileListModel() {}
  virtual ~CFileListModel() = default;

  /*! @brief      Insert a new file into the model.
   *  @param[in]  file: pointer to the file.
   *  *throws
   */

  void insert_back(Wt::WFileDropWidget::File *file)
  {
    ID_t ID = ++lastID;
    records_.emplace_back(ID, byRow.size(), file, CFileUploadWidget::S_PENDING);
    byID.emplace(ID, std::ref(records_.back()));
    byPointer.emplace(file, std::ref(records_.back()));
    byRow.emplace_back(std::ref(records_.back()));
    reset();
  }

  /*! @brief      Insert a range of files into the model.
   *  @param[in]  first: Iterator to the beginning of the range.
   *  @param[in]  last: Iterator to the end of the range.
   *  @throws
   */

  template<typename iter>
  void insert_back(iter first, iter last)
  {
    for (; first != last; first++)
    {
      insert_back(*first);
    }
  }

  void setCurrentFile(Wt::WFileDropWidget::File *cf) noexcept
  {
    if (currentFile && byPointer.contains(currentFile))
    {
      byPointer.at(currentFile).get().dataReceivedConnection.disconnect();
    }

    currentFile = cf;
    if (currentFile && byPointer.contains(currentFile))
    {
      byPointer.at(currentFile).get().dataReceivedConnection = currentFile->dataReceived().connect(this,
                                                                                                   &CFileListModel::dataReceived);
      byPointer.at(currentFile).get().status = CFileUploadWidget::S_UPLOADING;
    }
    else
    {
      CODE_ERROR();

      // Does not return.
    }
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

    RUNTIME_ASSERT(byRow.size() > row, "CFileListModel::data: Incorrect row number.");

    switch (column)
    {
      case 0:
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            rv = static_cast<std::string>(byRow[row].get().file->clientFileName());
          }
          default:
          {
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
            rv = byRow[row].get().fileType;
            break;
          }
          case Wt::ItemDataRole::User:
          {
              rv = &(byRow[row].get());
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

  void dataReceived(std::uint64_t num, std::uint64_t denom)
  {
    TRACE_ENTER();
    if (currentFile && byPointer.contains(currentFile))
    {
      byPointer.at(currentFile).get().dataReceived[0] = num;
      byPointer.at(currentFile).get().dataReceived[1] = denom;
    }
    else
    {
      CODE_ERROR();
      // Does not return.
    }
    reset();
    TRACE_EXIT();
  }

private:
  CFileListModel(int rows, int columns) = delete;
  CFileListModel(CFileListModel const &) = delete;
  CFileListModel(CFileListModel &&) = delete;
  CFileListModel &operator=(CFileListModel const &) = delete;
  CFileListModel &operator=(CFileListModel &&) = delete;

  value_list records_;     // All the records.
  std::map<ID_t, value_ref> byID;
  std::map<Wt::WFileDropWidget::File *, value_ref> byPointer;
  std::vector<value_ref> byRow;
  Wt::WFileDropWidget::File *currentFile = nullptr;
  ID_t lastID = 0;

};

/// Implements the delegate to display the progress bar or the text.
class CFileListDelegate : public Wt::WAbstractItemDelegate
{
public:
  CFileListDelegate() = default;
  virtual ~CFileListDelegate() = default;

protected:
  virtual std::unique_ptr<Wt::WWidget> update(Wt::WWidget *widget, Wt::WModelIndex const &index, Wt::WFlags<Wt::ViewItemRenderFlag>)
  {
    std::unique_ptr<Wt::WWidget> rv;

    CFileListModel::pointer data = std::any_cast<CFileListModel::pointer>(index.data(Wt::ItemDataRole::User));

    CFileListModel const *model = dynamic_cast<CFileListModel const *>(index.model());

    switch(data->status)
    {
      case CFileUploadWidget::S_PENDING:
      {
        if (data->textEditStatus == nullptr)
        {
          rv = std::make_unique<Wt::WText>("Pending");
          data->textEditStatus = dynamic_cast<Wt::WText *>(rv.get());
        }
        break;
      }
      case CFileUploadWidget::S_UPLOADING:
      {
        if (data->textEditStatus != nullptr)
        {
          data->textEditStatus = nullptr;
          rv = std::make_unique<Wt::WProgressBar>();
          data->progressBar = dynamic_cast<Wt::WProgressBar *>(rv.get());
        }
        else if (data->progressBar)
        {
          data->progressBar->setRange(0, data->dataReceived[1]);
          data->progressBar->setValue(data->dataReceived[0]);
        }
        else
        {
          CODE_ERROR();
          // Does not return.
        }
        break;
      }
      case CFileUploadWidget::S_COMPLETE:
      {
//        if (data->progressBar != nullptr)
//        {
//          data->progressBar = nullptr;
//          rv = std::make_unique<Wt::WText>(completeText);
//          data->textEditStatus = dynamic_cast<Wt::WText *>(rv.get());
//        }
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
  CFileListDelegate(CFileListDelegate const &) = delete;
  CFileListDelegate(CFileListDelegate &&) = delete;
  CFileListDelegate &operator=(CFileListDelegate const &) = delete;
  CFileListDelegate &operator=(CFileListDelegate &&) = delete;
};

/**********************************************************************************************************************************/

CFileUploadWidget::CFileUploadWidget() : Wt::WFileDropWidget()
{
  model = std::make_shared<CFileListModel>();
  createUI();
}

void CFileUploadWidget::createUI()
{
  setLayoutSizeAware(true);
  setOverflow(Wt::Overflow::Scroll);

  tableView = addWidget(std::make_unique<Wt::WTableView>());
  tableView->setModel(model);
  tableView->setColumnResizeEnabled(true);
  tableView->setColumnAlignment(0, Wt::AlignmentFlag::Center);
  tableView->setHeaderAlignment(0, Wt::AlignmentFlag::Center);
  tableView->setAlternatingRowColors(true);
  tableView->setSelectionMode(Wt::SelectionMode::Single);
  tableView->setEditTriggers(Wt::EditTrigger::None);
  tableView->setItemDelegateForColumn(1, std::make_shared<CFileListDelegate>());

  drop().connect([&](const std::vector<Wt::WFileDropWidget::File*>& files)
  {
    model->insert_back(files.begin(), files.end());

    // If the maximum number of files has been met or exceeded, don't allow any more drops.
    if (uploads().size() >= maxFiles_)
    {
      setAcceptDrops(false);
    }
 });
 newUpload().connect(this, &CFileUploadWidget::fileUploadStarting);
 uploaded().connect(this, &CFileUploadWidget::fileUploaded);
}

void CFileUploadWidget::fileUploadStarting(Wt::WFileDropWidget::File *file)
{
  model->setCurrentFile(file);
  tableView->refresh();
}


void CFileUploadWidget::fileUploaded(Wt::WFileDropWidget::File* file)
{

}
