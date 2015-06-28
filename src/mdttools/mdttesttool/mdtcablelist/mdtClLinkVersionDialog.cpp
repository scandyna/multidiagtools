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
#include "mdtClLinkVersionDialog.h"
#include <QLabel>

mdtClLinkVersionDialog::mdtClLinkVersionDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
}

void mdtClLinkVersionDialog::setLastVersionData(mdtClLinkVersionData data)
{
  if(data.isNull()){
    lbLastVersion->clear();
  }else{
    lbLastVersion->setText(QString::number(data.version(), 'f', 3));
  }
  data.setVersion(data.version() + 0.1);
  setVersionData(data);
}

void mdtClLinkVersionDialog::setVersionData(const mdtClLinkVersionData & data)
{
  pvVersionData = data;
  sbVersion->setValue(data.version());
}

void mdtClLinkVersionDialog::accept()
{
  if(sbVersion->value() < 0.001){
    return;
  }
  pvVersionData.setVersion(sbVersion->value());
  QDialog::accept();
}
