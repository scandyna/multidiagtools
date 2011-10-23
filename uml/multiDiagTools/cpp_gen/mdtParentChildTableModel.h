#ifndef _MDTPARENTCHILDTABLEMODEL_H
#define _MDTPARENTCHILDTABLEMODEL_H


class mdtParentChilTableItem;

class mdtParentChildTableModel : public QAbstractItemModel {
  private:
    QStringList pvStringList;


  public:
     mdtParentChildTableModel(const QStringList & strings, const QObject & parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex());

    int columnCount(const QModelIndex & parent = QModelIndex());

    QVariant data(const QModelIndex & index, int role);

    void setHeaderData(const QList<QVariant> & headerData);

    QVariant headerData(int section, int orientation, int role);

    int flags(const QModelIndex & index);

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex());

    QModelIndex parent(const QModelIndex & index);


  private:
    <mdtParentChilTableItem> pvRootItem;

    mdtParentChilTableItem * ;

};
#endif
