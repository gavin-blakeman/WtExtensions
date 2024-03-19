#include "include/requirementsWidget.h"

// Standard C++ library headers
#include <vector>


class CRequirementsModel final : public Wt::WAbstractItemModel
{
public:
  using ID_t = CRequirementsWidget::ID_t;


  CRequirementsModel(std::string const &ic1, std::string const &ic2): icon1(ic1), icon2(ic2) {}
  virtual ~CRequirementsModel() = default;

  void setData(ID_t ID, std::string &&st) { records_.emplace_back(ID, std::move(st), icon1, icon2); }
  void setData(ID_t ID, std::string const &st) { records_.emplace_back(ID, st, icon1, icon2); }

  virtual void reload();

protected:
  virtual std::any headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const override;
  virtual Wt::WModelIndex index(int row, int column, const Wt::WModelIndex &parent = Wt::WModelIndex()) const override;
  virtual Wt::WModelIndex parent(const Wt::WModelIndex &) const override {return Wt::WModelIndex(); }
  virtual int columnCount(const Wt::WModelIndex &) const override;
  virtual int rowCount(const Wt::WModelIndex &) const override { return records_.size(); }
  virtual std::any data(const Wt::WModelIndex &, Wt::ItemDataRole) const override;
  virtual Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex &index) const override;
  //virtual bool setData(Wt::WModelIndex const &index, std::any const &value, Wt::ItemDataRole role) override;

private:
  CRequirementsModel() = delete;
  CRequirementsModel(int rows, int columns) = delete;
  CRequirementsModel(CRequirementsModel const &) = delete;
  CRequirementsModel(CRequirementsModel &&) = delete;
  CRequirementsModel &operator=(CRequirementsModel const &) = delete;
  CRequirementsModel &operator=(CRequirementsModel &&) = delete;

  struct record_t
  {
    ID_t ID;
    std::string shortText;
    std::string const &icon1;
    std::string const &icon2;
  };
  std::vector<record_t> records_;
  std::string icon1;
  std::string icon2;
};


/// @brief      Class constructor.
/// @param[in]  i1: The icon to use when the requirement is not met.
/// @param[in]  i2: The icon to use when the requirement is met.
/// @throws
/// @version    2024-03-15/GGB - Function created.

CRequirementsWidget::CRequirementsWidget(std::string const &i1, std::string const &i2)
{
  model = std::make_shared<CRequirementsModel>(i1, i2);
}

/// @brief      Inserts a new item into the model.
/// @param[in]  id: The ID of the item.
/// @param[in]  txt: The text to associate with the item.
/// @throws
/// @version    2024-04-15/GGB - Function created.

void CRequirementsWidget::insert(ID_t id, std::string &&txt)
{
  std::dynamic_pointer_cast<CRequirementsModel>(model)->setData(id, std::move(txt));
}

/// @brief      Inserts a new item into the model.
/// @param[in]  id: The ID of the item.
/// @param[in]  txt: The text to associate with the item.
/// @throws
/// @version    2024-04-15/GGB - Function created.

void CRequirementsWidget::insert(ID_t id, std::string const &txt)
{
  std::dynamic_pointer_cast<CRequirementsModel>(model)->setData(id, txt);
}

void CRequirementsWidget::insert(std::initializer_list<std::tuple<ID_t, std::string>> &)
{

}
