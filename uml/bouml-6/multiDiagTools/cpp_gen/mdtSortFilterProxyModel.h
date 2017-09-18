#ifndef _MDTSORTFILTERPROXYMODEL_H
#define _MDTSORTFILTERPROXYMODEL_H


class mdtSortFilterProxyModel : public QSortFilterProxyModel {
  public:
    mdtSortFilterProxyModel(const QObject & parent);

    ~mdtSortFilterProxyModel();

  void clearColumnsSortOrder();

  void addColumnToSortOrder(int column, const Qt::SortOrder & order);

  void addColumnToSortOrder(const QString & fieldName, const Qt::SortOrder & order);

  QVector sortedColumns();


  private:
    virtual bool lessThan(const QModelIndex & left, const QModelIndex & right);

};
#endif
