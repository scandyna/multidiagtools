/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtUicNumberWidget.h"
#include "mdtUicNumberValidator.h"
#include <QLocale>

#include <QDebug>

mdtUicNumberWidget::mdtUicNumberWidget(QWidget *parent)
 : QWidget(parent)
{
  pvUicNumber = 0;

  setupUi(this);

  // Diseable Details
  wDetails->hide();
  pbDetails->setEnabled(false);
  connect(leUicNumber, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(diseableDetailsButton()));
  connect(pbDetails, SIGNAL(toggled(bool)), this, SLOT(showDetails(bool)));

  // Setup input line edit
  leUicNumber->setValidator(new mdtUicNumberValidator(leUicNumber));
  connect(pbCheckUicNumber, SIGNAL(clicked()), this, SLOT(checkUicNumber()));
  connect(leUicNumber, SIGNAL(editingFinished()), this, SLOT(checkUicNumber()));
}

mdtUicNumberWidget::~mdtUicNumberWidget()
{
}

void mdtUicNumberWidget::setUicNumber(mdtUicNumber *uicNumber)
{
  Q_ASSERT(uicNumber != 0);

  pvUicNumber = uicNumber;
}

mdtUicNumber *mdtUicNumberWidget::uicNumber()
{
  Q_ASSERT(pvUicNumber != 0);

  return pvUicNumber;
}

void mdtUicNumberWidget::setUicDbsDir(const QString &dir)
{
  pvUicDbsDir = dir;
}

void mdtUicNumberWidget::diseableDetailsButton()
{
  if(!wDetails->isVisible()){
    pbDetails->setEnabled(false);
  }else{
    updateDetails(false);
  }
}

void mdtUicNumberWidget::checkUicNumber()
{
  if(pvUicNumber == 0){
    updateDetails(false);
    return;
  }
  // Check if input seems OK
  if(!leUicNumber->hasAcceptableInput()){
    updateDetails(false);
    return;
  }
  if(!pvUicNumber->setNumber(leUicNumber->text())){
    updateDetails(false);
    return;
  }
  leUicNumber->setText(pvUicNumber->formatedUicNumber());
  pbDetails->setEnabled(true);
  updateDetails(true);
}

void mdtUicNumberWidget::showDetails(bool show)
{
  if(!show){
    wDetails->hide();
    return;
  }
  // See if databases are loaded/loadable
  if(pvUicDbsDir.isEmpty()){
    pbDetails->setChecked(false);
    return;
  }
  if(!pvUicNumber->dbsLoaded()){
    if(!pvUicNumber->loadDbs(pvUicDbsDir)){
      pbDetails->setChecked(false);
      return;
    }
  }
  wDetails->setVisible(true);
  updateDetails(true);
}

void mdtUicNumberWidget::updateDetails(bool valid)
{
  if(!wDetails->isVisible()){
    return;
  }
  if(!pvUicNumber->dbsLoaded()){
    return;
  }
  if(!valid){
    lbType->setText("");
    lbUsage->setText("");
    lbCountry->setText("");
    lbSpeedAndHeat->setText("");
    return;
  }
  lbType->setText(pvUicNumber->type().name(QLocale::system()));
  lbUsage->setText(pvUicNumber->usage().name(QLocale::system()));
  lbCountry->setText(pvUicNumber->country().name(QLocale::system()));
  lbSpeedAndHeat->setText(pvUicNumber->speedAndHeat().name(QLocale::system()));
}
