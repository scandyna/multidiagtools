#ifndef _MDTCLUNITVEHICLETYPE_H
#define _MDTCLUNITVEHICLETYPE_H


class mdtClUnitVehicleType {
  public:
    mdtClUnitVehicleType(const QSqlDatabase & db);

    ~mdtClUnitVehicleType();

    QSqlQueryModel vehicleTypeNotAssignedToUnitModel(const QVariant & unitId);

    bool addUnitVehicleTypeAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList);

    bool removeUnitVehicleAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList);


  private:
    QSqlQueryModel pvVehicleTypeNotAssignedToUnitModel;

    QSqlDatabase pvDatabase;


  public:
    QSqlError lastError();

};
#endif
