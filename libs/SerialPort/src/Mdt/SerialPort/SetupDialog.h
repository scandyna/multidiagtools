/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_SERIAL_PORT_SETUP_DIALOG_H
#define MDT_SERIAL_PORT_SETUP_DIALOG_H

#include <QDialog>
#include "ui_SetupDialog.h"

namespace Mdt{ namespace SerialPort{

  /*! \brief Dialog to setup a serial port
   */
  class SetupDialog : public QDialog, private Ui::SetupDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    SetupDialog(QWidget *parent = nullptr);
  };

}} // namespace mdt{ namespace SerialPort{

#endif // #ifndef MDT_SERIAL_PORT_SETUP_DIALOG_H
