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
// HISTORY:             2024-03-01 GGB - File Created
//
//*********************************************************************************************************************************

#ifndef WTEXTENSIONS_WFILEUPLOADWIDGET_H_
#define WTEXTENSIONS_WFILEUPLOADWIDGET_H_

// Standard C++ library header files
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <semaphore>
#include <shared_mutex>
#include <string>
#include <thread>

// Wt++ header files
#include <Wt/WFileDropWidget.h>
#include <Wt/WSignal.h>

class CFileListModel;

/*! @class    CFileUploadWidget is an extension of WFileDropWidget. It functions as a file drop area. But when the files are dropped
 *            a scrolling list of the files is displayed with a progress bar showing the current progress of the current file.
 *            Once the file load has completed, the progress bar is replace with specific text provided by the application.
 *  @details  The intended usage is when loading a number of files. Each file is loaded in turn, with progress being displayed.
 *            The application can then determine the file type and display the file type in the space vacated by the progress
 *            bar.
 *            Three columns are provided, with the third column allowing a delete button to delete the file from the upload list.
 */

class CFileUploadWidget : public Wt::WFileDropWidget
{
public:
  using mutex_type = std::shared_mutex;
  using unique_lock = std::unique_lock<mutex_type>;
  using shared_lock = std::shared_lock<mutex_type>;
  enum status_e
  {
    S_NONE,         //
    S_PENDING,      // The pending text is displayed. IE this is a future action. The initialText is displayed.
    S_UPLOADING,    // The file is uploading.
    S_COMPLETE,     // The action is complete. The finalText is displayed.
  };
  using ID_t = std::uint32_t;
  using row_t = std::size_t;
  struct record_t
  {
    ID_t ID;
    row_t row;
    Wt::WFileDropWidget::File *file;
    CFileUploadWidget::status_e status;
    std::string fileType;
    Wt::WText *textEditStatus = nullptr;
    Wt::WProgressBar *progressBar = nullptr;
    double lastUpdate = 0;
    std::atomic_flag recordUpdated;
    Wt::Signals::connection dataReceivedConnection;
    Wt::Signals::connection uploadCompleteConnection;
    mutex_type mRecord;
    std::string completedText = "Upload Completed";
  };
  using value_type = record_t;
  using reference = value_type &;
  using const_reference = value_type const &;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using value_list = std::list<value_type>;
  using value_ref = std::reference_wrapper<value_type>;
  typedef void (*updateProgress_f)(double, double);


  // Callback function allowing the application to set the type text for the uploaded file.
  typedef std::string (*fileType_f)(std::filesystem::path const &, std::filesystem::path const &);

  struct fileData_t
  {
    mutex_type mData;                                 // To read or update any of the fields in data this mutex must be held.
    value_list files;
    std::atomic_flag filesUpdated;
    value_list records;     // All the records.
    std::map<ID_t, value_ref> byID;
    std::map<Wt::WFileDropWidget::File *, value_ref> byPointer;
    std::vector<value_ref> byRow;
    Wt::WFileDropWidget::File *currentFile = nullptr;
    ID_t lastID = 0;
    std::atomic_flag progressUpdate;
  };

  /*! @brief      Constructor
   *  @throws
   */
  CFileUploadWidget(Wt::WApplication &a);
  virtual ~CFileUploadWidget() = default;

  /*! @brief      Clears all the data. All the files should also be deleted.
   *  @throws     nothrow
   */
  void clearData() noexcept;

  void erase(std::filesystem::path const &);

  /*! @brief      Sets the maximum number of files that can be uploaded by the widget.
   *  @param[in]  mf: The maximum number of files.
   *  @throws     noexcept
   */
  void maxFiles(std::size_t mf) noexcept { maxFiles_ = mf;}

  /*! @brief      Sets the file's completed text.
   *  @param[in]  fileID: the file's ID.
   *  @param[in]  ct: completedText
   *  @throws
   */
  void setCompletedText(ID_t fileID, std::string const &ct);

protected:
  Wt::WTableView *tableView = nullptr;
  std::shared_ptr<CFileListModel> model;

  /*! @brief      Sets up the UI aspects of the widget.
   *  @throws
   * @version    2024-03-19/GGB - Function created.
   */
  virtual void createUI();

  /*! @brief      Returns the maximum number of files that can be uploaded.
   *  @returns    The maximum number of files.
   */
  std::size_t maxFiles() const noexcept { return maxFiles_; }

  /*! @brief      Responds to the signal when a new upload is going to start.
   *  @param[in]  file: The file being uploaded.
   */
  void fileUploadStarting(Wt::WFileDropWidget::File *file);

  /*! @brief      Function called when files are dropped in the control.
   *  @param[in]  files: The files being dropped.
   */
 void filesDropped(std::vector<Wt::WFileDropWidget::File *> const& files);

private:
 CFileUploadWidget() = delete;
  CFileUploadWidget(CFileUploadWidget const &) = delete;
  CFileUploadWidget(CFileUploadWidget &&) = delete;
  CFileUploadWidget &operator=(CFileUploadWidget const &) = delete;
  CFileUploadWidget &operator=(CFileUploadWidget &&) = delete;

  Wt::WApplication &application;
  fileData_t fileData;
  std::uint16_t maxFiles_ = 50;
  Wt::Signal<ID_t, Wt::WFileDropWidget::File *> fileUploadSignal;
};


#endif // WTEXTENSIONS_WFILEUPLOADWIDGET_H_
