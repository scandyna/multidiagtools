
#include "mdtClUnitVehicleType.h"

mdtClUnitVehicleType::mdtClUnitVehicleType(const QSqlDatabase & db) {
}

mdtClUnitVehicleType::~mdtClUnitVehicleType() {
}

QSqlQueryModel mdtClUnitVehicleType::vehicleTypeNotAssignedToUnitModel(const QVariant & unitId) {
}

bool mdtClUnitVehicleType::addUnitVehicleTypeAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList) {
}

bool mdtClUnitVehicleType::removeUnitVehicleAssignments(const QVariant & unitId, const QList<QVariant> & vehicleTypeIdList) {
}

QSqlError mdtClUnitVehicleType::lastError() {
}

