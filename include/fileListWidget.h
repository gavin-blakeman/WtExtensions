//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                fileListWidget.h
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


#ifndef WTEXTENSIONS_FILELISTWIDGET_H
#define WTEXTENSIONS_FILELISTWIDGET_H

// Standard C++ library
#include <cstdint>
#include <filesystem>

// Wt Library

#include <Wt/WAbstractItemModel.h>
#include <Wt/WTableView.h>

/* Implements a list that displays three columns. The first column is the file name. (Not the full path), the
 * second column displays the type of file. The last column displays an icon that allows 'deletion' of the file from the list.
 * This is implemented as a container with a MVC table view holding the information.
 * The icon needs to be supplied and be part of the resource pack.
 * A signal is emitted when the delete button is pressed.
 */

class CFileListWidget : public Wt::WTableView
{
public:
  using ID_t = std::uint32_t;

  CFileListWidget();
  virtual ~CFileListWidget() = default;

  void insert(ID_t, std::filesystem::path const &, std::string const &);
  void insert(ID_t, std::filesystem::path  &&, std::string &&);

  void erase(std::filesystem::path const &);

private:
  CFileListWidget(CFileListWidget const &) = delete;
  CFileListWidget(CFileListWidget &&) = delete;
  CFileListWidget &operator=(CFileListWidget const &) = delete;
  CFileListWidget &operator=(CFileListWidget &&) = delete;

  std::shared_ptr<Wt::WAbstractItemModel> model;

  void createUI();
};

#endif // WTEXTENSIONS_FILELISTWIDGET_H
