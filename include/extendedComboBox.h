#ifndef EXTENDEDCOMBOBOX_H
#define EXTENDEDCOMBOBOX_H

#include <cstdint>

#include <Wt/WComboBox.h>
#include <Wt/WSignal.h>

class extendedComboBox : public Wt::WComboBox
{
private:
  std::size_t userData;
  Wt::Signal<std::size_t> selectionChanged_;

public:
  extendedComboBox(std::size_t ud) : WComboBox(), userData(ud)
  {
    changed().connect([this]{ selectionChanged_.emit(userData);});
  }
  void setUID(std::size_t uid) { userData = uid; }


  Wt::Signal<std::size_t> &changed()
  {
    return selectionChanged_;
  }


};

#endif // EXTENDEDCOMBOBOX_H
