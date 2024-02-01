#ifndef EXTENDEDPUSHBUTTON_H
#define EXTENDEDPUSHBUTTON_H

  // Standard C++ library

#include <cstdint>

  // Wt++ headers

#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>

class extendedPushButton : public Wt::WPushButton
{
private:
  std::size_t userData;
  Wt::Signal<std::size_t> buttonClicked_;

public:
  extendedPushButton(Wt::WString const &st, std::size_t ud) : WPushButton(st), userData(ud)
  {
    clicked().connect([this]{ buttonClicked_.emit(userData);});
  }

  void setUID(std::size_t uid) { userData = uid; }


  Wt::Signal<std::size_t> &buttonClicked()
  {
    return buttonClicked_;
  }


};

#endif // EXTENDEDPUSHBUTTON_H
