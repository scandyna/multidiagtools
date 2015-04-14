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

#include <QVariant>

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
  de.setMaximum(pvLimits.leftTopLimit().value());
  de.setValue(pvLimits.leftBottomLimitVar());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setLeftBottomLimitVar(de.value());
  displayLeftLimits();
}

void mdtTtValueLimitsWidget::editLeftTopLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter left top limit:"));
  de.setMinimum(pvLimits.leftBottomLimit().value());
  de.setMaximum(pvLimits.rightBottomLimit().value());
  de.setValue(pvLimits.leftTopLimitVar());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setLeftTopLimitVar(de.value());
  displayLeftLimits();
}

void mdtTtValueLimitsWidget::editRightBottomLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter right bottom limit:"));
  de.setMinimum(pvLimits.leftTopLimit().value());
  de.setMaximum(pvLimits.rightTopLimit().value());
  de.setValue(pvLimits.rightBottomLimitVar());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setRightBottomLimitVar(de.value());
  displayRightLimits();
}

void mdtTtValueLimitsWidget::editRightTopLimit()
{
  mdtDoubleEditDialog de(this);

  de.setLabelText(tr("Enter right top limit:"));
  de.setMinimum(pvLimits.rightBottomLimit().value());
  de.setMaximumToInfinity();
  de.setValue(pvLimits.rightTopLimitVar());
  if(de.exec() != QDialog::Accepted){
    return;
  }
  pvLimits.setRightTopLimitVar(de.value());
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
  if(bottom.isNull()){
    tbLeftLimitBottomValue->setText("...");
  }else{
    str = QString("%1").arg(bottom.value());
    tbLeftLimitBottomValue->setText(str);
  }
  // Update left limits top button
  if(top.isNull()){
    tbLeftLimitTopValue->setText("...");
  }else{
    str = QString("%1").arg(top.value());
    tbLeftLimitTopValue->setText(str);
  }
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
  if(bottom.isNull()){
    tbRightLimitBottomValue->setText("...");
  }else{
    str = QString("%1").arg(bottom.value());
    tbRightLimitBottomValue->setText(str);
  }
  // Update right limits top button
  if(top.isNull()){
    tbRightLimitTopValue->setText("...");
  }else{
    str = QString("%1").arg(top.value());
    tbRightLimitTopValue->setText(str);
  }
}
