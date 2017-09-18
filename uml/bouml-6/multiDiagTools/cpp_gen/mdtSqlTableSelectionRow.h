#ifndef _MDTSQLTABLESELECTIONROW_H
#define _MDTSQLTABLESELECTIONROW_H


class mdtSqlTableSelectionItem;

class mdtSqlTableSelectionRow
{
  private:
    QList<mdtSqlTableSelectionItem> ItemList;


  public:
  mdtSqlTableSelectionRow();

  void clear();

  void addIndex(const QModelIndex & index, const QString & fieldName);

  QModelIndex index(int internalColumnIndex) const;

  QModelIndex index(const QString & fieldName) const;

};

#endif // #ifndef MDTSQLTABLESELECTIONROW_H
