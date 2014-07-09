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
#include <cfloat>
#include <QtGlobal>

mdtTtTestNodeUnitSetupDialog::mdtTtTestNodeUnitSetupDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  setupUi(this);
  connect(cbState, SIGNAL(stateChanged(int)), this, SLOT(updateStateText(int)));
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

void mdtTtTestNodeUnitSetupDialog::updateStateText(int state)
{
  switch(state){
    case Qt::Checked:
      cbState->setText("On");
      break;
    case Qt::Unchecked:
      cbState->setText("Off");
      break;
    case Qt::PartiallyChecked:
      cbState->setText("Null");
      break;
  }
}

void mdtTtTestNodeUnitSetupDialog::displayData()
{
  QVariant val;

  // Update step number spinbox
  sbStepNumber->setValue(pvData.value("StepNumber").toInt());
  // Update state checkbox
  val = pvData.value("State");
  if(val.isNull()){
    cbState->setCheckState(Qt::PartiallyChecked);
  }else{
    cbState->setChecked(val.toBool());
  }
  // Update value spinbox
  val = pvData.value("Value");
  if(val.isNull()){
    sbValue->setValue(sbValue->minimum());
  }else{
    sbValue->setValue(val.toDouble());
  }
  // Update command text edit
  val = pvData.value("Command");
  teCommand->setPlainText(val.toString());
}

void mdtTtTestNodeUnitSetupDialog::updateData()
{
  // Update step number
  pvData.setValue("StepNumber", sbStepNumber->value());
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
  if(qAbs<double>(sbValue->value() - sbValue->minimum()) < DBL_EPSILON){
    pvData.setValue("Value", QVariant());
  }else{
    pvData.setValue("Value", sbValue->value());
  }
  // Update command
  pvData.setValue("Command", teCommand->toPlainText());
}
