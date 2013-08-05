#ifndef _MDTCLUNITVEHICLETYPE_H
#define _MDTCLUNITVEHICLETYPE_H


class mdtClUnitVehicleType {
  public:
    mdtClUnitVehicleType(const QSqlDatabase & db);

    ~mdtClUnitVehicleType();

    QSqlQueryModel vehicleTypeNotAssignedToUnitModel(const QVariant & unitId);

    QSqlError addUnitVehicleTypeAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList);

    QSqlError removeUnitVehicleAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList);


  private:
    QSqlQueryModel pvVehicleTypeNotAssignedToUnitModel;

    QSqlDatabase pvDatabase;

    QSqlQueryModel pvUnitVehicleTypeModel;

};
#endif
