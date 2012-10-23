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
#include "mdtAbstractIoWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>

mdtAbstractIoWidget::mdtAbstractIoWidget(QWidget *parent)
 : QWidget(parent)
{
  pvAddress = 0;
  // Create label widgets
  lbLabel = new QLabel;
  lbLabel->setFrameStyle(QFrame::Panel | QFrame::Plain);
  lbLabel->setLineWidth(1);
  lbLabel->setAlignment(Qt::AlignHCenter);
  pbDetails = new QPushButton;
  pbDetails->setText("...");
  pbDetails->setToolTip(tr("Show details"));
  // Infomration box
  pvMessageBox = new QMessageBox;
  pvMessageBox->setText(tr("          I/O informations                           "));
  connect(pbDetails, SIGNAL(clicked()), this, SLOT(showIoInformations()));
}

mdtAbstractIoWidget::~mdtAbstractIoWidget()
{
  delete pvMessageBox;
}

void mdtAbstractIoWidget::setAddress(int address)
{
  pvAddress = address;
}

int mdtAbstractIoWidget::address() const
{
  return pvAddress;
}

void mdtAbstractIoWidget::setLabelShort(const QString &text)
{
  lbLabel->setText(text);
}

QString mdtAbstractIoWidget::labelShort() const
{
  return lbLabel->text();
}

void mdtAbstractIoWidget::setLabel(const QString & text)
{
  lbLabel->setToolTip(text);
}

QString mdtAbstractIoWidget::label() const
{
  return lbLabel->toolTip();
}

void mdtAbstractIoWidget::setDetails(const QString & text)
{
  pvMessageBox->setDetailedText(text);
}

QString mdtAbstractIoWidget::details() const
{
  return pvMessageBox->detailedText();
}

void mdtAbstractIoWidget::showIoInformations()
{
  Q_ASSERT(pvMessageBox != 0);

  QString str;

  // Labels
  str = tr("Short label: ");
  str += lbLabel->text();
  str += "\n";
  str += tr("Label: ");
  str += lbLabel->toolTip();
  str += "\n";
  str += tr("Address: 0x");
  str += QString::number(pvAddress, 16);
  str += " (dec: ";
  str += QString::number(pvAddress, 10);
  str += ")";
  pvMessageBox->setInformativeText(str);
  pvMessageBox->setIcon(QMessageBox::Information);
  pvMessageBox->exec();
}
