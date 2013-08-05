
#include "mdtClUnitVehicleType.h"

mdtClUnitVehicleType::mdtClUnitVehicleType(const QSqlDatabase & db) {
}

mdtClUnitVehicleType::~mdtClUnitVehicleType() {
}

QSqlQueryModel mdtClUnitVehicleType::vehicleTypeNotAssignedToUnitModel(const QVariant & unitId) {
}

QSqlError mdtClUnitVehicleType::addUnitVehicleTypeAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList) {
}

QSqlError mdtClUnitVehicleType::removeUnitVehicleAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList) {
}

