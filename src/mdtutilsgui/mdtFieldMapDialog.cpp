/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtFieldMapDialog.h"
#include "mdtFieldMapItemDialog.h"
#include "mdtFieldMap.h"

mdtFieldMapDialog::mdtFieldMapDialog(QWidget *parent) 
 : QDialog(parent)
{
  setupUi(this);
  connect(pbAdd, SIGNAL(clicked()), this, SLOT(addMapItem()));
  connect(pbEdit, SIGNAL(clicked()), this, SLOT(editMapItem()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(removeMapItem()));
}

mdtFieldMapDialog::~mdtFieldMapDialog()
{
}

void mdtFieldMapDialog::setFieldMap(mdtFieldMap *map)
{
  Q_ASSERT(map != 0);

  pvFieldMap = map;
  ///pvInternalFieldMap.
}

/**
mdtFieldMap mdtFieldMapDialog::fieldMap() 
{
}
*/

void mdtFieldMapDialog::addMapItem()
{
  mdtFieldMapItemDialog dialog(this);

  dialog.exec();
}

void mdtFieldMapDialog::editMapItem()
{
  mdtFieldMapItemDialog dialog(this);

  dialog.exec();
}

void mdtFieldMapDialog::removeMapItem()
{
  mdtFieldMapItemDialog dialog(this);

  dialog.exec();
}

void mdtFieldMapDialog::updateSourcePreview() 
{
}

void mdtFieldMapDialog::updateDestinationPreview() 
{
}

