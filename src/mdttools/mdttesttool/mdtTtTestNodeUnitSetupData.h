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
#ifndef MDT_TT_TEST_NODE_UNIT_SETUP_DATA_H
#define MDT_TT_TEST_NODE_UNIT_SETUP_DATA_H

#include "mdtSqlRecord.h"
#include <QSqlDatabase>
#include <QString>

/*! \brief Storage class for test node unit
 */
class mdtTtTestNodeUnitSetupData : public mdtSqlRecord
{
 public:

  /*! \brief I/O type
   */
  enum io_type_t {
                  Unknown = 0,    /*!< Unknown I/O type */
                  AnalogInput,    /*!< Analog input */
                  AnalogOutput,   /*!< Analog output */
                  DigitalInput,   /*!< Digital input */
                  DigitalOutput   /*!< Digital output */
                 };

  /*! \brief Construct a empty test node data container
   */
  mdtTtTestNodeUnitSetupData();

  /*! \brief Construct a test node unit setup data based on a QSqlRecord
   *
   * \pre record must contain all fields from TestNodeUnitSetup_tbl
   */
  mdtTtTestNodeUnitSetupData(const QSqlRecord & record);

  /*! \brief Setup fields from TestNodeUnitSetup_tbl
   */
  bool setup(const QSqlDatabase & db);

  /*! \brief Get I/O type
   *
   * I/O type is only avaliable if data comes from TestNodeUnitSetup_view.
   */
  io_type_t ioType() const;

  /*! \brief Get I/O position
   *
   * I/O position is only avaliable if data comes from TestNodeUnitSetup_view.
   */
  int ioPosition() const;

  /*! \brief Get schema position
   *
   * Schema position is only avaliable if data comes from TestNodeUnitSetup_view.
   */
  QString schemaPosition() const;

};

#endif // #ifndef MDT_TT_TEST_NODE_UNIT_SETUP_DATA_H
