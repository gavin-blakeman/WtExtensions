//*********************************************************************************************************************************
//
// PROJECT:             Wt Extensions
// FILE:                requirementsText.h
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
// HISTORY:             2024-03-15 GGB - File Created
//
//*********************************************************************************************************************************

#ifndef WTEXTENSIONS_REQUIREMENTSWIDGET_H
#define WTEXTENSIONS_REQUIREMENTSWIDGET_H

/* The Requirements Widget is used to display a list of requirements. This is implemented as a tree table with thre columns
 *  The first column lists the requirement.
 *  The second column indicates if the requirement is "required" or "optional"
 *  The third column displays text indicating whether the requirement has been met.
 */

// Standard C++ library
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>

// Wt library
#include <Wt/WAbstractItemModel.h>
#include <Wt/WTableView.h>

class CRequirementsWidget : public Wt::WTableView
{
public:
  using ID_t = std::uint64_t;       // Identifier type to identify the specific requirement.
  enum requirementType_e
  {
    REQUIRED,
    OPTIONAL,
  };

  CRequirementsWidget();
  virtual ~CRequirementsWidget() = default;

  void insert(ID_t, std::string &&, requirementType_e);
  void insert(ID_t, std::string const &, requirementType_e);
  void insert(std::initializer_list<std::tuple<ID_t, std::string, requirementType_e>>);

  void changeStatus(ID_t, bool = true);

  [[nodiscard]] bool allMet() const noexcept;

protected:
  void createUI();


private:
  CRequirementsWidget(CRequirementsWidget const &) = delete;
  CRequirementsWidget(CRequirementsWidget &&) = delete;
  CRequirementsWidget &operator=(CRequirementsWidget const &) = delete;
  CRequirementsWidget &operator==(CRequirementsWidget &&) = delete;

  std::shared_ptr<Wt::WAbstractItemModel> model;
};


#endif // WTEXTENSIONS_REQUIREMENTSWIDGET_H
