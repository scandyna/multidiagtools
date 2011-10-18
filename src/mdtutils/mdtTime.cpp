#include "mdtTime.h"

mdtTime::mdtTime()
{
  pvElapsed = 0;
}

void mdtTime::stop()
{
  pvElapsed = QTime::elapsed();
}

int mdtTime::elapsed() const
{
  return pvElapsed;
}

double mdtTime::elapsedSec()
{
  return (double)pvElapsed / 1000.0;
}
