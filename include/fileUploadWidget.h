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
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

// Wt++ header files
#include <Wt/WFileDropWidget.h>

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
  enum status_e
  {
    S_NONE,         //
    S_PENDING,      // The pending text is displayed. IE this is a future action. The initialText is displayed.
    S_UPLOADING,    // The file is uploading.
    S_COMPLETE,     // The action is complete. The finalText is displayed.
  };
  using ID_t = std::uint32_t;  typedef void (*updateProgress_f)(double, double);
  // Callback function allowing the application to set the type text for the uploaded file.
  typedef std::string (*fileType_f)(std::filesystem::path const &, std::filesystem::path const &);

  /*! @brief      Constructor
   *  @throws
   */
  CFileUploadWidget();
  virtual ~CFileUploadWidget() = default;

  void erase(std::filesystem::path const &);

  /*! @brief      Sets the maximum number of files that can be uploaded by the widget.
   *  @param[in]  mf: The maximum number of files.
   *  @throws     noexcept
   */

  void maxFiles(std::size_t mf) noexcept { maxFiles_ = mf;}

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

  /*! @brief      Function called when a file has been uploaded.
   *  @throws
   */
  void fileUploaded(Wt::WFileDropWidget::File *file);

  /*! @brief      Responds to the signal when a new upload is going to start.
   *  @param[in]  file: The file being uploaded.
   */
  void fileUploadStarting(Wt::WFileDropWidget::File *file);


private:
  CFileUploadWidget(CFileUploadWidget const &) = delete;
  CFileUploadWidget(CFileUploadWidget &&) = delete;
  CFileUploadWidget &operator=(CFileUploadWidget const &) = delete;
  CFileUploadWidget &operator=(CFileUploadWidget &&) = delete;

  std::uint16_t maxFiles_ = 50;


};


#endif // WTEXTENSIONS_WFILEUPLOADWIDGET_H_
