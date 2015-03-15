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
#ifndef MDT_TT_TEST_NODE_MODBUS_IO_TOOL_H
#define MDT_TT_TEST_NODE_MODBUS_IO_TOOL_H

#include "mdtModbusIoTool.h"
#include <QSqlDatabase>
#include <QString>

class mdtTtTestNodeManager;

/*! \brief Tool to view multi I/O MODBUS device
 *
 * This tool is similar to mdtModbusIoTool,
 *  but offers some facility when interacting
 *  with test tool database.
 *  Internally, mdtTtTestNodeManager is used.
 */
class mdtTtTestNodeModbusIoTool : public mdtModbusIoTool
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestNodeModbusIoTool(QWidget *parent, QSqlDatabase db);

  /*! \brief Set MODBUS device
   *
   * \param deviceAlias The alias of device, as given in TestNode_tbl
   */
  bool setModbusDevice(const QString & deviceAlias);

  /*! \brief Connect to MODBUS device
   */
  bool connectToDevice();

 private:

  Q_DISABLE_COPY(mdtTtTestNodeModbusIoTool);

  mdtTtTestNodeManager *pvTestNodeManager;
};

#endif // #ifndef MDT_TT_TEST_NODE_MODBUS_IO_TOOL_H
