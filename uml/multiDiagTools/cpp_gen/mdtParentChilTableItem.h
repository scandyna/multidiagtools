#ifndef _MDTPARENTCHILTABLEITEM_H
#define _MDTPARENTCHILTABLEITEM_H


class mdtParentChildTableModel;

class mdtParentChilTableItem {
  public:
     mdtParentChilTableItem();

     ~mdtParentChilTableItem();

    void appendChild(const mdtParentChilTableItem & child);

    mdtParentChilTableItem child(int row);

    int childCount();

    int coloumnCount();

    QVariant data(int coloumn);

    int row();

    mdtParentChilTableItem parent();


  private:
    <mdtParentChilTableItem> pvParentItem;

    QList<mdtParentChilTableItem> pvChildItems;

    <QAbstractTableModel *> pvModel;

    int pvModelRow;

    mdtParentChildTableModel * ;

};
#endif
