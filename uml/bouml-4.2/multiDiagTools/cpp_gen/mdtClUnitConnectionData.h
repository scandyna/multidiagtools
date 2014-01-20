#ifndef _MDTCLUNITCONNECTIONDATA_H
#define _MDTCLUNITCONNECTIONDATA_H


class mdtClUnitConnectionData {
  private:
    QVariant PvId;

    QVariant pvUintId;

    QVariant pvArticleConnectionId;

    QVariant pvSchemaPage;

    QVariant pvFunctionEN;

    QVariant pvFunctionFR;

    QVariant pvFunctionDE;

    QVariant pvFunctionIT;

    QVariant pvSignalName;

    QVariant pvSwAddress;

    QVariant pvUnitConnectorName;

    QVariant pvUnitContactName;


  public:
    mdtClUnitConnectionData();

     ~mdtClUnitConnectionData();

    bool isValid();

    void setId(const QVariant & id);

    QVariant id();

    void setUnitId(const QVariant & id);

    QVariant unitId() const;

    void setArticleConnectionId(const QVariant & id);

    QVariant articleConnectionId() const;

    void setSchemaPage(const QVariant & schemaPage);

    QVariant schemaPage() const;

    void setFunctionEN(const QVariant & function);

    QVariant functionEN() const;

    void setFunctionFR(const QVariant & function);

    QVariant functionFR() const;

    void setFunctionDE(const QVariant & function);

    QVariant functionDE() const;

    void setFunctionIT(const QVariant & function);

    QVariant functionIT() const;

    void setSignalName(const QVariant & name);

    QVariant signalName() const;

    void setSwAddress(const QVariant & address);

    QVariant swAddress() const;

    void setUnitConnectorName(const QVariant & name);

    QVariant unitConnectorName() const;

    void setUnitContactName(const QVariant & name);

    QVariant unitContactName() const;

};
#endif
