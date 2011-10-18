
#include <QApplication>
#include "essais.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  essais e;
  e.show();

  return app.exec();
}
