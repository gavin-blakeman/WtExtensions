#include "include/progressGroup.h"

// Miscellaneous libraries
#include <GCL>

void CProgressGroup::beginStep()
{
  if (currentItem->status != S_ACTIVE)
  {
    if (currentItem->status == S_PENDING)
    {
      currentItem->status = S_ACTIVE;
    }
    else if (currentItem->status == S_COMPLETE)
    {
      currentItem++;
      currentItem->status = S_ACTIVE;
    }
    else
    {
      CODE_ERROR();
    }
  }

  // Delete the text and replace with a progress bar

//  currentItem->container->clear();
//  currentItem->container->addNew(std::make_unique());
}

void CProgressGroup::completeStep()
{
  if (currentItem->status != S_ACTIVE)
  {
    CODE_ERROR();
  }
  currentItem->status = S_COMPLETE;

  // Delete the progress bar and replace with text.

}

void CProgressGroup::setRange(double dMin, double dMax)
{
  RUNTIME_ASSERT(currentItem->progress != nullptr, "Calling setMinMax before current bar is active.");

  currentItem->progress->setRange(dMin, dMax);
}

void CProgressGroup::updateProgress(double p)
{
  // Get the session lock.


}
