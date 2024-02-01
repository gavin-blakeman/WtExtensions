#include "include/progressText.h"

  // Standard C++ header files

#include <string>

  // Wt header files

#include <Wt/WString.h>

int CProgressText::overflow(int c)
{
  setText(text() + std::string{static_cast<char>(c)});
  return 0;
}
