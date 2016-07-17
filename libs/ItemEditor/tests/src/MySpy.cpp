#include "MySpy.h"

void MySpy::display(const QString& v)
{
  qDebug() << "v: " << v;
}

void MySpy::display(const int& v)
{
  qDebug() << "v: " << v;
}
