extern void fonctionEssais();

#include <QApplication>
#include "mdtTest.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  mdtTest t;

  return app.exec();
}
