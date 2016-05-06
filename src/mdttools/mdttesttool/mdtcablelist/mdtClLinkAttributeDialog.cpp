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
#include "mdtClLinkAttributeDialog.h"
#include "Mdt/CableList/ModificationModel.h"  /// \todo Update once migrated
#include "mdtClLinkVersionModel.h"
#include "mdtClVehicleTypeLinkAssignationWidget.h"
#include "mdtClLink.h"
#include "mdtError.h"
#include <QWidget>
#include <QComboBox>
#include <QString>
#include <QMessageBox>

//#include <QDebug>

using Mdt::CableList::ModificationModel;   /// \todo Remove once migrated

mdtClLinkAttributeDialog::mdtClLinkAttributeDialog(QWidget *parent, QSqlDatabase db, bool enableVehicleTypeAssignations)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  // Setup Modification combobox
  pvModificationModel = new ModificationModel(this, db);
  cbModification->setModel(pvModificationModel);
  cbModification->setModelColumn(1);
  // Setup link version combobox
  pvLinkVersionModel = new mdtClLinkVersionModel(this, db);
  cbLinkVersion->setModel(pvLinkVersionModel);
  cbLinkVersion->setModelColumn(1);
  // Setup vehicle type assignations widget if requested
  if(enableVehicleTypeAssignations){
    pvVehicleTypeAssignationWidget = new mdtClVehicleTypeLinkAssignationWidget(this, pvDatabase);
    saVehicleTypeAssignation->setWidget(pvVehicleTypeAssignationWidget);
    setWindowTitle(tr("Link versionning and vehicle type assignations edition"));
  }else{
    gbVehicleTypeAssignation->setVisible(false);
    pvVehicleTypeAssignationWidget = nullptr;
    setWindowTitle(tr("Link versionning edition"));
  }
}

void mdtClLinkAttributeDialog::setMessage(const QString & msg)
{
  lbMessage->setText(msg);
}

bool mdtClLinkAttributeDialog::buildVehicleTypeList(const QVariant & unitId)
{
  Q_ASSERT(pvVehicleTypeAssignationWidget != nullptr);

  if(!pvVehicleTypeAssignationWidget->buildVehicleTypeList(unitId, unitId)){
    pvLastError = pvVehicleTypeAssignationWidget->lastError();
    return false;
  }
  pvVehicleTypeAssignationWidget->selectAllVehicleType();

  return true;
}

QList<mdtClVehicleTypeStartEndKeyData> mdtClLinkAttributeDialog::selectedVehicleTypeList() const
{
  Q_ASSERT(pvVehicleTypeAssignationWidget != nullptr);

  return pvVehicleTypeAssignationWidget->getSelectedVehicleTypeList();
}

void mdtClLinkAttributeDialog::setLinkVersion(const mdtClLinkVersionData &v)
{
  int row;

  row = pvLinkVersionModel->row(v.pk());
  cbLinkVersion->setCurrentIndex(row);
}

LinkVersionPk mdtClLinkAttributeDialog::linkVersionPkData() const
{
  return pvLinkVersionModel->currentVersionPk(cbLinkVersion);
}

void mdtClLinkAttributeDialog::setModification(ModificationPk m)
{
  int row;

  row = pvModificationModel->row(m);
  cbModification->setCurrentIndex(row);
}

void mdtClLinkAttributeDialog::setModification(ModificationType m)
{
  setModification(ModificationPk(m));
}

ModificationPk mdtClLinkAttributeDialog::modificationKeyData() const
{
  return pvModificationModel->currentModificationPk(cbModification);
}
