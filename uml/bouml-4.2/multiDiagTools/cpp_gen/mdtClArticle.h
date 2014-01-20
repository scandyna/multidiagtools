#ifndef _MDTCLARTICLE_H
#define _MDTCLARTICLE_H


class mdtClArticle {
  private:
    QSqlDatabase pvDatabase;

    QSqlError pvLastError;


  public:
    mdtClArticle(const QSqlDatabase & db);

    ~mdtClArticle();

    QSqlError lastError();

    bool addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode);

    bool removeLink(const QVariant & linkId);

    bool removeLinks(const QVariant & linkIdList);

    bool removeLinks(const QModelIndexList & indexListOfSelectedRows);

    bool addResistor(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value);

    bool addDiode(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double Vf, const QVariant & directionCode);

    bool addBridge(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId,  );

};
#endif
