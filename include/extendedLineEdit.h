#ifndef EXTENDEDLINEEDIT_H
#define EXTENDEDLINEEDIT_H

#include <cstdint>

#include <Wt/WLineEdit.h>
#include <Wt/WSignal.h>

class extendedLineEdit: public Wt::WLineEdit
{
private:
  std::size_t userData;
  Wt::Signal<std::size_t> selectionChanged_;

public:
  extendedLineEdit(std::size_t ud) : WLineEdit(), userData(ud)
  {
    changed().connect([this]{ selectionChanged_.emit(userData); });
  }
  void setUID(std::size_t uid) { userData = uid; }


  Wt::Signal<std::size_t> &changed()
  {
    return selectionChanged_;
  }


};

#endif // EXTENDEDLINEEDIT_H
