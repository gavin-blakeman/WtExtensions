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
#include <map>
#include <vector>


class CRequirementsModel final : public Wt::WAbstractItemModel
{
public:
  using ID_t = CRequirementsWidget::ID_t;


  CRequirementsModel() {}
  virtual ~CRequirementsModel() = default;

  void setData(ID_t ID, std::string &&st, CRequirementsWidget::requirementType_e rt)
  {
    records_.emplace_back(ID, std::move(st), rt);
    byID.emplace(ID, std::ref(records_.back()));
  }
  void setData(ID_t ID, std::string const &st, CRequirementsWidget::requirementType_e rt)
  {
    records_.emplace_back(ID, st, rt);
    byID.emplace(ID, std::ref(records_.back()));
  }

protected:
  virtual std::any headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const override {};
  virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex &parent = Wt::WModelIndex()) const override {};
  virtual Wt::WModelIndex parent(const Wt::WModelIndex &) const override {return Wt::WModelIndex(); }
  virtual int columnCount(const Wt::WModelIndex &) const override {};
  virtual int rowCount(const Wt::WModelIndex &) const override { return records_.size(); }
  virtual std::any data(const Wt::WModelIndex &, Wt::ItemDataRole) const override {};
  virtual Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex &index) const override {};
  //virtual bool setData(Wt::WModelIndex const &index, std::any const &value, Wt::ItemDataRole role) override;

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
    CRequirementsWidget::requirementMet_e requirementMet;
  };
  std::vector<record_t> records_;
  std::map<ID_t, std::reference_wrapper<record_t>> byID;

};


/// @brief      Class constructor.
/// @throws
/// @version    2024-03-15/GGB - Function created.

CRequirementsWidget::CRequirementsWidget()
{
  model = std::make_shared<CRequirementsModel>();
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


/// @brief      Sets up the UI aspects of the widget.
/// @throws
/// @version    2024-03-19/GGB - Function created.

void CRequirementsWidget::setupUI()
{

}
