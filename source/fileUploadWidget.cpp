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
#include <Wt/WApplication.h>
#include <Wt/WAbstractItemDelegate.h>
#include <Wt/WAbstractItemModel.h>
#include <Wt/WModelIndex.h>
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
  using mutex_type = CFileUploadWidget::mutex_type;
  using unique_lock = CFileUploadWidget::unique_lock;
  using shared_lock = CFileUploadWidget::shared_lock;
  using ID_t = CFileUploadWidget::ID_t;
  using fileData_t = CFileUploadWidget::fileData_t;
  using value_type = CFileUploadWidget::value_type;
  using reference = CFileUploadWidget::reference;
  using const_reference = CFileUploadWidget::const_reference;
  using pointer = CFileUploadWidget::pointer;
  using const_pointer = CFileUploadWidget::const_pointer;
  using value_list = CFileUploadWidget::value_list;
  using value_ref = CFileUploadWidget::value_ref;


  CFileListModel(fileData_t &fd) : fileData(fd) {}
  virtual ~CFileListModel() = default;

  /*! @brief      Insert a new file into the model.
   *  @param[in]  file: pointer to the file.
   *  *throws
   */

  void insert_back(Wt::WFileDropWidget::File *file)
  {
    /* Called from within the GUI thread. */

    shared_lock sl{fileData.mData};
    ID_t ID = ++fileData.lastID;
    fileData.records.emplace_back(ID, fileData.byRow.size(), file, CFileUploadWidget::S_PENDING);
    fileData.byID.emplace(ID, std::ref(fileData.records.back()));
    fileData.byPointer.emplace(file, std::ref(fileData.records.back()));
    fileData.byRow.emplace_back(std::ref(fileData.records.back()));
    Wt::WModelIndex modelIndex;
    rowsInserted().emit(modelIndex, fileData.byRow.size() - 1, fileData.byRow.size() - 1);
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

  /*! @brief      Sets the file's completed text.
   *  @param[in]  fileID: the file's ID.
   *  @param[in]  ct: completedText
   *  @throws
   */
  void setCompletedText(ID_t fileID, std::string const &ct)
  {
    shared_lock sl{fileData.mData};
    if (fileData.byID.contains(fileID))
    {
      fileData.byID.at(fileID).get().completedText = ct;
      Wt::WModelIndex modelIndex = createIndex(fileData.byID.at(fileID).get().row, 1,  nullptr);
      dataChanged().emit(modelIndex, modelIndex);
    }
    else
    {
      CODE_ERROR();
      // Does not return.
    }
  }

  void setCurrentFile(Wt::WFileDropWidget::File *cf)
  {
    /* This function is called from the GUI thread. */

    shared_lock sl{fileData.mData};
    if (fileData.currentFile && fileData.byPointer.contains(fileData.currentFile))
    {
      reference temp = fileData.byPointer.at(fileData.currentFile).get();
      sl.unlock();
      temp.dataReceivedConnection.disconnect();
      temp.uploadCompleteConnection.disconnect();
      temp.status = CFileUploadWidget::S_COMPLETE;
      Wt::WModelIndex modelIndex = createIndex(fileData.byPointer.at(fileData.currentFile).get().row, 1,  nullptr);
      dataChanged().emit(modelIndex, modelIndex);
    }

    fileData.currentFile = cf;    // May be setting to nullptr.
    if (!sl)
    {
      sl.lock();
    }
    if (fileData.currentFile && fileData.byPointer.contains(fileData.currentFile))
    {
      reference temp = fileData.byPointer.at(fileData.currentFile).get();
      sl.unlock();
      temp.dataReceivedConnection = fileData.currentFile->dataReceived().connect(this, &CFileListModel::dataReceived);
      temp.uploadCompleteConnection = fileData.currentFile->uploaded().connect(this, &CFileListModel::uploadFinished);
      temp.status = CFileUploadWidget::S_UPLOADING;
      Wt::WModelIndex modelIndex = createIndex(fileData.byPointer.at(fileData.currentFile).get().row, 1,  nullptr);
      dataChanged().emit(modelIndex, modelIndex);
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
  virtual int rowCount(const Wt::WModelIndex &) const override { return fileData.records.size(); }
  virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex &parent = Wt::WModelIndex()) const override
  {
    RUNTIME_ASSERT(column < 3, "CRequirementsWidget::index: Column number out of range.");
    RUNTIME_ASSERT(row < fileData.records.size(), "CRequirementsWidget::index: Row number out of range.");
    return createIndex(row, column, nullptr);
  }

  virtual Wt::WModelIndex parent(const Wt::WModelIndex &) const override {return Wt::WModelIndex(); }
  virtual int columnCount(const Wt::WModelIndex &) const override { return 3; }


  virtual std::any data(const Wt::WModelIndex &index, Wt::ItemDataRole role) const override
  {
    std::any rv;

    std::size_t row = static_cast<std::size_t>(index.row());
    std::size_t column = static_cast<std::size_t>(index.column());

    RUNTIME_ASSERT(fileData.byRow.size() > row, "CFileListModel::data: Incorrect row number.");

    switch (column)
    {
      case 0:
      {
        switch (role.value())
        {
          case Wt::ItemDataRole::Display:
          {
            rv = static_cast<std::string>(fileData.byRow[row].get().file->clientFileName());
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
            rv = fileData.byRow[row].get().fileType;
            break;
          }
          case Wt::ItemDataRole::User:
          {
            rv = &(fileData.byRow[row].get());
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
    /* This is called by GUI thread. */

    shared_lock sl{fileData.mData};
    if (fileData.currentFile && fileData.byPointer.contains(fileData.currentFile))
    {
      if (fileData.byPointer.at(fileData.currentFile).get().progressBar)
      {
        double temp = static_cast<double>(num)/static_cast<double>(denom) * 100;

        if ( ((temp - fileData.byPointer.at(fileData.currentFile).get().lastUpdate)) >= 10)
        {
          fileData.byPointer.at(fileData.currentFile).get().progressBar->setValue(temp);
          fileData.byPointer.at(fileData.currentFile).get().lastUpdate = temp;
        }

      }
    }
    else
    {
      CODE_ERROR();
      // Does not return.
    }
  }

  void uploadFinished()
  {
    shared_lock sl{fileData.mData};
    if (fileData.currentFile && fileData.byPointer.contains(fileData.currentFile))
    {
      setCurrentFile(nullptr);
    }
    else
    {
      CODE_ERROR();
      // Does not return.
    }
  }

private:
  CFileListModel() = delete;
  CFileListModel(int rows, int columns) = delete;
  CFileListModel(CFileListModel const &) = delete;
  CFileListModel(CFileListModel &&) = delete;
  CFileListModel &operator=(CFileListModel const &) = delete;
  CFileListModel &operator=(CFileListModel &&) = delete;

  fileData_t &fileData;
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
    /* This function should only be called from the GUI thread. */

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
          data->progressBar->setRange(0, 100);
        }
        else if (data->progressBar)
        {
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
        if (data->progressBar != nullptr)
        {
          data->progressBar = nullptr;
          rv = std::make_unique<Wt::WText>(data->completedText);
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
  CFileListDelegate(CFileListDelegate const &) = delete;
  CFileListDelegate(CFileListDelegate &&) = delete;
  CFileListDelegate &operator=(CFileListDelegate const &) = delete;
  CFileListDelegate &operator=(CFileListDelegate &&) = delete;
};

/**********************************************************************************************************************************/

CFileUploadWidget::CFileUploadWidget() : Wt::WFileDropWidget()
{
  model = std::make_shared<CFileListModel>(fileData);
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

  drop().connect(this, &CFileUploadWidget::filesDropped);
  newUpload().connect(this, &CFileUploadWidget::fileUploadStarting);
}

void CFileUploadWidget::fileUploadStarting(Wt::WFileDropWidget::File *file)
{
  /* This function is called from the GUI thread. */

  model->setCurrentFile(file);
}

void CFileUploadWidget::filesDropped(std::vector<Wt::WFileDropWidget::File*> const& files)
{
  model->insert_back(files.begin(), files.end());

  // If the maximum number of files has been met or exceeded, don't allow any more drops.
  if (uploads().size() >= maxFiles_)
  {
    setAcceptDrops(false);
  }
}

void CFileUploadWidget::setCompletedText(ID_t fileID, std::string const &ct)
{
  model->setCompletedText(fileID, ct);
}



