#ifndef _MDTSORTFILTERPROXYMODEL_H
#define _MDTSORTFILTERPROXYMODEL_H


class mdtSortFilterProxyModel : public QSortFilterProxyModel {
  public:
    mdtSortFilterProxyModel(const QObject & parent);

    ~mdtSortFilterProxyModel();


  private:
    virtual bool lessThan(const QModelIndex & left, const QModelIndex & right);

};
#endif
