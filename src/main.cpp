#include <QApplication>
#include "mdtParentChildTableModel.h"
#include <QStringList>
#include <QListView>
#include <QTableView>
#include <QTreeView>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QStringList lst;
/*
  QList<QVariant> header;
  QStringList line1, line2;
  QList<QStringList> data;

  header << "A" << "B" << "C" << "D";
  // Data
  line1 << "01" << "02" << "03";
  line2 << "11" << "12" << "13";
  data << line1 << line2;
*/
  // Modèle
  mdtParentChildTableModel m;
//  mdtTreeItem it(data);
//  it.setHeaderData(header);

  // Vues
  QListView lw;
  QTableView tw;
  QTreeView trw;

  //lw.setModel(&m);
  //lw.show();
  //tw.setModel(&m);
  //tw.show();
  trw.setModel(&m);
  trw.show();

/*
  lw.setModel(&m);
  lw.show();
  tw.setModel(&m);
  tw.show();
  trw.setModel(&m);
  trw.show();
*/

  return app.exec();
}
