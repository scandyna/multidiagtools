#ifndef _MDTCLLINKDATA_H
#define _MDTCLLINKDATA_H


class mdtClLinkData {
  private:
    QVariant pvArticleLinkId;

    QVariant pvUnitConnectionStartId;

    QVariant pvUnitConnectionEndId;

    QVariant pvSinceVersion;

    QVariant pvModification;

    QVariant pvIdentification;

    QVariant pvLinkDirectionCode;

    QVariant pvLinkTypeCode;

    QVariant pvValue;

    QList<QVariant> pvVehicleTypeStartIdList;

    QList<QVariant> pvVehicleTypeEndIdList;


  public:
  void setArticleLinkId(const QVariant & value);

  QVariant articleLinkId() const;

  void setUnitConnectionStartId(const QVariant & value);

  QVariant unitConnectionStartId() const;

  void setUnitConnectionEndId(const QVariant & value);

  QVariant unitConnectionEndId() const;

  void setSinceVersion(const QVariant & value);

  QVariant sinceVersion() const;

  void setModification(const QVariant & value);

  QVariant modification() const;

  void setIdentification(const QVariant & value);

  QVariant identification() const;

  void setLinkDirectionCode(const QVariant & value);

  QVariant linkDirectionCode() const;

  void setLinkTypeCode(const QVariant & value);

  QVariant linkTypeCode() const;

  void setValue(const QVariant & value);

  QVariant value() const;

  void setVehicleTypeStartIdList(const QList<QVariant> & idList);

  const QList<QVariant> &vehicleTypeStartIdList() const;

  void setVehicleTypeEndIdList(const QList<QVariant> & idList);

  const QList<QVariant> &vehicleTypeEndIdList() const;

};
#endif
