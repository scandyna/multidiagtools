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
#ifndef MDT_FIELD_MAPPING_DIALOG_H
#define MDT_FIELD_MAPPING_DIALOG_H

#include "ui_mdtFieldMappingDialog.h"
#include <QDialog>

/*! \brief Field mapping dialog
 */
class mdtFieldMappingDialog : public QDialog, Ui::mdtFieldMappingDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtFieldMappingDialog(QWidget *parent = nullptr);
};

#endif // #ifndef MDT_FIELD_MAPPING_DIALOG_H
