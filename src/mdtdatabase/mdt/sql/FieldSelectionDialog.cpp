/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "FieldSelectionDialog.h"
#include "FieldSelectionProxyModel.h"
#include "mdtSqlTableSchema.h"
#include "mdtSqlTableSchemaModel.h"
#include "mdtSqlDriverType.h"

namespace mdt{ namespace sql{

FieldSelectionDialog::FieldSelectionDialog(QWidget* parent)
 : QDialog(parent),
   pvSchemaModel(new mdtSqlTableSchemaModel(this)),
   pvProxyModel(new FieldSelectionProxyModel(this))
{
  setupUi(this);
  // Setup field table
  pvProxyModel->setSourceModel(pvSchemaModel);
  tvFields->setModel(pvProxyModel);
}

void FieldSelectionDialog::setMessage(const QString& msg)
{
  lbMessage->setText(msg);
}

mdtExpected<bool> FieldSelectionDialog::setTable(const QString & tableName, const QSqlDatabase & db)
{
  Q_ASSERT(db.isValid());
  Q_ASSERT(db.isOpen());

  mdtSqlTableSchema ts;

  if(!ts.setupFromTable(tableName, db)){
    return ts.lastError();
  }
  pvSchemaModel->setTableSchema(ts, mdtSqlDriverType::typeFromName(db.driverName()));
  resizeTvToContents();

  return true;
}

void FieldSelectionDialog::sort(Qt::SortOrder sortOrder)
{
  pvProxyModel->sort(0, sortOrder);
}

void FieldSelectionDialog::setFieldIndexesToHide(const FieldIndexList& indexes)
{
  pvProxyModel->setFieldIndexesToHide(indexes);
}

void FieldSelectionDialog::showAllFieldIndexes()
{
  pvProxyModel->showAllFieldIndexes();
}

void FieldSelectionDialog::setFieldSelectionMode(FieldSelectionMode mode)
{
  pvProxyModel->setFieldSelectionMode(mode);
}

FieldIndexList FieldSelectionDialog::getSelectedFieldIndexList() const
{
  return pvProxyModel->getSelectedFieldIndexList();
}

void FieldSelectionDialog::resizeTvToContents()
{
  tvFields->resizeRowsToContents();
  tvFields->resizeColumnsToContents();
}

}} // namespace mdt{ namespace sql{

