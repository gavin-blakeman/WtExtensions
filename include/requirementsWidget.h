#ifndef WTEXTENSIONS_REQUIREMENTSWIDGET_H
#define WTEXTENSIONS_REQUIREMENTSWIDGET_H

/* The file match widget is used to display a two column table. Where the second column represents a file (resource) needed
 * by a WebApp, and the first column indicates whether the need is met.
 * The widget is positioned in a container. and can be placed within other widgets.
 * The widget is designed to be called from the main event thread, not another thread.
 */

// Standard C++ library
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>

// Wt library
#include <Wt/WAbstractItemModel.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WIconPair.h>

class CRequirementsWidget : public Wt::WContainerWidget
{
public:
  using ID_t = std::uint_fast8_t;       // Identifier type to identify the specific requirement.

  CRequirementsWidget(std::string const &, std::string const &);
  virtual ~CRequirementsWidget() = default;

  void insert(ID_t, std::string &&);
  void insert(ID_t, std::string const &);
  void insert(std::initializer_list<std::tuple<ID_t, std::string>> &);

  void changeStatus(ID_t, bool = true);


private:
  CRequirementsWidget() = delete;
  CRequirementsWidget(CRequirementsWidget const &) = delete;
  CRequirementsWidget(CRequirementsWidget &&) = delete;
  CRequirementsWidget &operator=(CRequirementsWidget const &) = delete;
  CRequirementsWidget &operator==(CRequirementsWidget &&) = delete;

  using record_t = std::pair<bool, std::string>;
  std::list<record_t> inOrder;  // Ordered copy of the data.
//  std::map<T, std::reference_wrapper<record_t>> byID;
  std::shared_ptr<Wt::WAbstractItemModel> model;

};


#endif // WTEXTENSIONS_REQUIREMENTSWIDGET_H
