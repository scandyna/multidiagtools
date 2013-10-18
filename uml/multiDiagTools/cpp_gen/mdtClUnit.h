#ifndef _MDTCLUNIT_H
#define _MDTCLUNIT_H


#include "mdtClUnitConnectionData.h"

class mdtClUnit {
  public:
    mdtClUnit(const QSqlDatabase & db);

    ~mdtClUnit();

    QSqlError lastError();

    int toUnitRelatedArticleConnectionCount(const QVariant & unitId);

    QList<QVarian> toUnitRelatedArticleConnectionIds(const QVariant & unitId);

    bool unitConnectionIsRelatedToArticleConnection(const QVariant & unitConnectionId);

    QVariant toUnitConnectionRelatedArticleConnectionData(const QVariant & unitConnectionId, const QString & field);


  private:
    QSqlQueryModel ;


  public:
    bool addUnitConnection(const mdtClUnitConnectionData & data);

    bool removeUnitConnection(const QVariant & unitConnectionId);

    bool editUnitConnection(const mdtClUnitConnectionData & data);

    mdtClUnitConnectionData getUnitConnectionData(const QVariant & unitConnectionId);


  private:
    QSqlDatabase pvDatabase;


  public:
    QSqlQueryModel unitConnectionRangesModel(const QVariant & unitConnectionId);

    bool addUnitConnectionRange(const QVariant & baseUnitConnectionId, const mdtClUnitConnectionData & rangeData);

    bool updateUnitConnectionRanges(const QVariant & baseUnitConnectionId, const QStandardItemModel & ranges);

    bool removeUnitConnectionRange(const QVariant & unitConnectionId);

};
#endif
