
#include "mdtDataTableModel.h"

mdtDataTableModel::mdtDataTableModel(const QObject & parent, const QSqlDatabase & db) {
}

mdtDataTableModel::~mdtDataTableModel() {
}

bool mdtDataTableModel::createDataSet(const QString & name, const QStringList & fields, const create_mode_t & mode) {
}

bool mdtDataTableModel::addRow(const QMap<QString,QVariant> & data,  ) {
}

bool mdtDataTableModel::setData(int row, int column, const QVariant & value, const QT::EditRole & role) {
}

bool mdtDataTableModel::setData(int row, const QString & field, const QVariant & value, const QT::EditRole & role) {
}

bool mdtDataTableModel::exportToCsvFile(const QString & filePath) {
}

bool mdtDataTableModel::importFromCsvFile(const QString & filePath) {
}

