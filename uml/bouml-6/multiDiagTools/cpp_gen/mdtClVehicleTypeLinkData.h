#ifndef _MDTCLVEHICLETYPELINKDATA_H
#define _MDTCLVEHICLETYPELINKDATA_H


class mdtClVehicleTypeLinkData
{
  private:
    QVariant pvVehicleTypeStartId;

    QVariant pvVehicleTypeEndId;

    QVariant pvUnitConnectionStartId;

    QVariant pvUnitConnectionEndId;


  public:
  inline const QVariant VehicleTypeStartId() const;

  void setVehicleTypeStartId(QVariant value);

  inline const QVariant VehicleTypeEndId() const;

  void setVehicleTypeEndId(QVariant value);

  inline const QVariant UnitConnectionStartId() const;

  void setUnitConnectionStartId(QVariant value);

  inline const QVariant UnitConnectionEndId() const;

  void setUnitConnectionEndId(QVariant value);

};
inline const QVariant mdtClVehicleTypeLinkData::VehicleTypeStartId() const 
{
  return pvVehicleTypeStartId;
}

inline const QVariant mdtClVehicleTypeLinkData::VehicleTypeEndId() const 
{
  return pvVehicleTypeEndId;
}

inline const QVariant mdtClVehicleTypeLinkData::UnitConnectionStartId() const 
{
  return pvUnitConnectionStartId;
}

inline const QVariant mdtClVehicleTypeLinkData::UnitConnectionEndId() const 
{
  return pvUnitConnectionEndId;
}


#endif // #ifndef MDTCLVEHICLETYPELINKDATA_H
