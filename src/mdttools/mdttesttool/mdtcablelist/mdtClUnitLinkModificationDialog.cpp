/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtClUnitLinkModificationDialog.h"
#include "mdtClModificationModel.h"
#include "mdtClLinkVersionModel.h"
#include "mdtClLink.h"
#include "mdtError.h"
#include <QWidget>
#include <QComboBox>
#include <QString>
#include <QMessageBox>

#include <QDebug>

mdtClUnitLinkModificationDialog::mdtClUnitLinkModificationDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  pvDatabase = db;
  setupUi(this);
  // Setup Modification combobox
  pvLinkModificationModel = new mdtClModificationModel(this, db);
  cbLinkModification->setModel(pvLinkModificationModel);
  cbLinkModification->setModelColumn(1);
  // Setup link version combobox
  pvLinkVersionModel = new mdtClLinkVersionModel(this, db);
  cbLinkVersion->setModel(pvLinkVersionModel);
  cbLinkVersion->setModelColumn(1);
}

void mdtClUnitLinkModificationDialog::setLinkVersion(const mdtClLinkVersionData &v)
{
  int row;

  row = pvLinkVersionModel->row(v.pk());
  cbLinkVersion->setCurrentIndex(row);
}

void mdtClUnitLinkModificationDialog::setLinkModification(const mdtClModificationPkData &m)
{
  int row;

  row = pvLinkModificationModel->row(m);
  cbLinkModification->setCurrentIndex(row);
}

void mdtClUnitLinkModificationDialog::setLinkModification(mdtClModification_t m)
{
  mdtClModificationPkData pk;

  pk.setModification(m);
  setLinkModification(pk);
}

mdtClLinkModificationKeyData mdtClUnitLinkModificationDialog::linkModificationKeyData() const
{
  mdtClLinkModificationKeyData key;

  ///key.setLinkFk(pvLinkData.pk());
  key.setLinkVersionFk(pvLinkVersionModel->currentVersionPk(cbLinkVersion));
  key.setModificationFk(pvLinkModificationModel->currentModificationPk(cbLinkModification));
  ///Q_ASSERT(!key.isNull());

  return key;
}
