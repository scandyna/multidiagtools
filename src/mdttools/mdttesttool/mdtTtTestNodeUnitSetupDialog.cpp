/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtTestNodeUnitSetupDialog.h"
#include <QVariant>

mdtTtTestNodeUnitSetupDialog::mdtTtTestNodeUnitSetupDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  setupUi(this);
}

void mdtTtTestNodeUnitSetupDialog::setData(const mdtTtTestNodeUnitSetupData & data)
{
  pvData = data;
  displayData();
}

void mdtTtTestNodeUnitSetupDialog::accept()
{
  updateData();
  QDialog::accept();
}

void mdtTtTestNodeUnitSetupDialog::displayData()
{
  QVariant val;
  bool on;

  // Update state checkbox
  val = pvData.value("State");
  if(val.isNull()){
    cbState->setCheckState(Qt::PartiallyChecked);
    cbState->setText("Null");
  }else{
    on = val.toBool();
    cbState->setChecked(on);
    if(on){
      cbState->setText("On");
    }else{
      cbState->setText("Off");
    }
  }
  // Update value spinbox
  val = pvData.value("Value");
  sbValue->setValue(val.toDouble());
}

void mdtTtTestNodeUnitSetupDialog::updateData()
{
  // Update state
  switch(cbState->checkState()){
    case Qt::Checked:
      pvData.setValue("State", true);
      break;
    case Qt::Unchecked:
      pvData.setValue("State", false);
      break;
    case Qt::PartiallyChecked:
      pvData.setValue("State", QVariant());
      break;
  }
  // Update value
  pvData.setValue("Value", sbValue->value());
}
