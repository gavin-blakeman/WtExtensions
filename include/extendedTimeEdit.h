#ifndef EXTENDEDTIMEEDIT_H
#define EXTENDEDTIMEEDIT_H

  // Standard C++ library

#include <cstdint>

  // Wt++ headers

#include <Wt/WTimeEdit.h>
#include <Wt/WSignal.h>

class extendedTimeEdit: public Wt::WTimeEdit
{
private:
  std::size_t userData;
  Wt::Signal<std::size_t> timeChanged_;

public:
  extendedTimeEdit(std::size_t ud) : WTimeEdit(), userData(ud)
  {
    changed().connect([this]{ timeChanged_.emit(userData); });
  }

  void setUID(std::size_t uid) { userData = uid; }


  Wt::Signal<std::size_t> &timeChanged()
  {
    return timeChanged_;
  }


};

#endif // EXTENDEDTIMEEDIT_H
