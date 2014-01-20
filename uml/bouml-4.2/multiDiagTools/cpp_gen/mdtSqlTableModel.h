#ifndef _MDTSQLTABLEMODEL_H
#define _MDTSQLTABLEMODEL_H


class mdtSqlTableModel : public QSqlTableModel {
  public:
    mdtSqlTableModel(const QObject & parent, const QSqlDatabase & db);

};
#endif
