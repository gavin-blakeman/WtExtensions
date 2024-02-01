#ifndef EXTENDEDDOUBLESPINBOX_H
#define EXTENDEDDOUBLESPINBOX_H

  // Standard C++ library

#include <cstdint>

  // Wt++ library

#include <Wt/WDoubleSpinBox.h>
#include <Wt/WSignal.h>

class extendedDoubleSpinBox : public Wt::WDoubleSpinBox
{
private:
  std::size_t userData;
  Wt::Signal<std::size_t> valueChanged_;

public:
  extendedDoubleSpinBox(std::size_t ud) : Wt::WDoubleSpinBox(), userData(ud)
  {

    valueChanged().connect([this](double){ valueChanged_.emit(userData);});
  }
  void setUID(std::size_t uid) { userData = uid; }


  Wt::Signal<std::size_t> &changed()
  {
    return valueChanged_;
  }

};


#endif // EXTENDEDDOUBLESPINBOX_H
