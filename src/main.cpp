#include <QApplication>
#include "mdtStringListModel.h"
#include <QStringList>
#include <QListView>
#include <QTableView>
#include <QTreeView>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QStringList lst;
  
  // Donées
  lst << "A" << "B" << "C";
  
  // Modèle
  mdtStringListModel m(lst);
  
  // Vues
  QListView lw;
  QTableView tw;
  QTreeView trw;
  
  lw.setModel(&m);
  lw.show();
  tw.setModel(&m);
  tw.show();
  trw.setModel(&m);
  trw.show();
  
  return app.exec();
}
