#include "include/progressText.h"

  // Standard C++ header files

#include <string>

  // Wt header files

#include <Wt/WString.h>

CProgressText::CProgressText() : std::ostream(this), Wt::WText(), app(Wt::WApplication::instance())
  {
    setTextFormat(Wt::TextFormat::Plain);
    app->enableUpdates(true);
  }

std::streambuf::int_type CProgressText::overflow(int ch)
{
  Wt::WApplication::UpdateLock uiLock(app);

  if (uiLock)
  {
    if (ch != Traits::eof())
    {
      setText(text() + std::string{static_cast<char>(ch)});
      app->triggerUpdate();
      return 0;
    };
  };
}
