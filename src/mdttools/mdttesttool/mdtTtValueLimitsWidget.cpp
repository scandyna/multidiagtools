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
#include "mdtTtValueLimitsWidget.h"
#include "mdtValue.h"
#include "mdtDoubleEditDialog.h"
#include <QString>
#include <QToolButton>

mdtTtValueLimitsWidget::mdtTtValueLimitsWidget(QWidget* parent)
 : QWidget(parent)
{
  setupUi(this);
  connect(tbLeftLimitBottomValue, SIGNAL(clicked()), this, SLOT(editLeftBottomLimit()));
  connect(tbLeftLimitTopValue, SIGNAL(clicked()), this, SLOT(editLeftTopLimit()));
  connect(tbRightLimitBottomValue, SIGNAL(clicked()), this, SLOT(editRightBottomLimit()));
  connect(tbRightLimitTopValue, SIGNAL(clicked()), this, SLOT(editRightTopLimit()));
  displayLeftLimits();
  displayRightLimits();
}

void mdtTtValueLimitsWidget::editLeftBottomLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter left bottom limit:"));
  de.setMinimumToMinusInfinity();
  de.setMaximum(pvLimits.leftTopLimit());
  de.setValueDouble(pvLimits.leftBottomLimit());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setLeftBottomLimit(de.valueDouble());
  displayLeftLimits();
}

void mdtTtValueLimitsWidget::editLeftTopLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter left top limit:"));
  de.setMinimum(pvLimits.leftBottomLimit());
  de.setMaximum(pvLimits.rightBottomLimit());
  de.setValueDouble(pvLimits.leftTopLimit());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setLeftTopLimit(de.valueDouble());
  displayLeftLimits();
}

void mdtTtValueLimitsWidget::editRightBottomLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter right bottom limit:"));
  de.setMinimum(pvLimits.leftTopLimit());
  de.setMaximum(pvLimits.rightTopLimit());
  de.setValueDouble(pvLimits.rightBottomLimit());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setRightBottomLimit(de.valueDouble());
  displayRightLimits();
}

void mdtTtValueLimitsWidget::editRightTopLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter right top limit:"));
  de.setMinimum(pvLimits.rightBottomLimit());
  de.setMaximumToInfinity();
  de.setValueDouble(pvLimits.rightTopLimit());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setRightTopLimit(de.valueDouble());
  displayRightLimits();
}

void mdtTtValueLimitsWidget::displayLeftLimits()
{
  mdtValueDouble bottom = pvLimits.leftBottomLimit();
  mdtValueDouble top = pvLimits.leftTopLimit();
  QString str;

  // Left fail zone is only displayed if left bottom limit is > -infinity
  if(bottom.isMinusInfinity()){
    lbLeftFailZone->setVisible(false);
  }else{
    lbLeftFailZone->setVisible(true);
  }
  // If left bottom and top are the same value, left limit range is not displayed
  if(bottom == top){
    lbLeftLimitRange->setVisible(false);
  }else{
    lbLeftLimitRange->setVisible(true);
  }
  // Update left limits bottom button
  str = QString("%1").arg(bottom.value());
  tbLeftLimitBottomValue->setText(str);
  // Update left limits top button
  str = QString("%1").arg(top.value());
  tbLeftLimitTopValue->setText(str);
}

void mdtTtValueLimitsWidget::displayRightLimits()
{
  mdtValueDouble bottom = pvLimits.rightBottomLimit();
  mdtValueDouble top = pvLimits.rightTopLimit();
  QString str;

  // Right fail zone is only displayed if right top limit is < infinity
  if(top.isPlusInfinity()){
    lbRightFailZone->setVisible(false);
  }else{
    lbRightFailZone->setVisible(true);
  }
  // If right bottom and top are the same value, right limit range is not displayed
  if(bottom == top){
    lbRightLimitRange->setVisible(false);
  }else{
    lbRightLimitRange->setVisible(true);
  }
  // Update right limits bottom button
  str = QString("%1").arg(bottom.value());
  tbRightLimitBottomValue->setText(str);
  // Update right limits top button
  str = QString("%1").arg(top.value());
  tbRightLimitTopValue->setText(str);
}
