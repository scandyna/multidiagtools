#ifndef _MDTSQLTABLESELECTION_H
#define _MDTSQLTABLESELECTION_H


class mdtSqlTableSelectionRow;

class mdtSqlTableSelection
{
  private:
    QList<mdtSqlTableSelectionRow> pvRowList;


  public:
  mdtSqlTableSelection();

  void clear();

  void setIndexList(const QModelIndex & indexList, const QStringList & fieldList, const QSqlQueryModel & model);

  int rowCount() const;

  QVariant data(int row, const QString & fieldName) const;

  QVariant dataList(const QString & fieldName) const;

};

#endif // #ifndef MDTSQLTABLESELECTION_H
