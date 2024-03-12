#include "include/progressText.h"

  // Standard C++ header files

#include <string>

  // Wt header files

#include <web/DomElement.h>
#include <Wt/WString.h>

CProgressText::CProgressText() : std::ostream(this), Wt::WText(), app(Wt::WApplication::instance())
  {
    setTextFormat(Wt::TextFormat::Plain);
    setStyleClass("textarea");
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
      doJavaScript(jsRef() + ".scrollTop = "+ jsRef() + ".scrollHeight;");
      app->triggerUpdate();
      return 0;
    };
  };
}

void CProgressText::updateDom(Wt::DomElement& element, bool all)
{
  Wt::WText::updateDom(element, all);
  element.setProperty(Wt::Property::StyleOverflowY, "auto");
}
