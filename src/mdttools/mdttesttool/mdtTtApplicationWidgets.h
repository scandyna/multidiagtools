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
#ifndef MDT_TT_APPLICATION_WIDGETS_H
#define MDT_TT_APPLICATION_WIDGETS_H

#include "mdtSqlApplicationWidgets.h"

// Editors
class mdtTtTestSystemEditor;
class mdtTtTestSystemComponentEditor;
class mdtTtTestSystemUnitEditor;

class mdtTtTestCableEditor;
class mdtTtTestNodeEditor;
// Tools
class mdtTtTestNodeModbusIoTool;

/*! \brief Container for test tool editors and views
 *
 * Using this class helps to create editors and views.
 *  Most of time, only one instance of each editore or view is needed,
 *  but must be accessible from multiple places in application.
 */
class mdtTtApplicationWidgets : public mdtSqlApplicationWidgets<mdtTtApplicationWidgets>
{
 Q_OBJECT

 // Needed because mdtSqlApplicationWidgets must access some private function from mdtTtApplicationWidgets
 friend class mdtSqlApplicationWidgets<mdtTtApplicationWidgets>;

 public:

  /*! \brief Edit test systems
   */
  static void editTestSystems();

  /*! \brief Edit test system components
   */
  static void editTestSystemComponents();

  /*! \brief Edit specified test system unit
   */
  static void editTestSystemUnit(const QVariant & unitId);


  /*! \brief Edit a specific test cable
   */
  static void editTestCable(const QVariant & testCableId);

  /*! \brief Edit test cables
   */
  static void editTestCables();

  /*! \brief Show test node MODBUS I/O tool
   */
  static void showTestNodeModbusIoTool(const QString & deviceAlias);

 public slots:

  /*! \brief Edit test systems
   */
  void slotEditTestSystems();

  /*! \brief Edit test system components
   */
  void slotEditTestSystemComponents();

  /*! \brief Edit test system units
   */
  void slotEditTestSystemUnits();


  /*! \brief Edit test cables
   */
  void slotEditTestCables();

  /*! \brief Edit test nodes
   */
  void slotEditTestNodes();

 private:

  /*! \brief Create test system editor
   */
  bool createTestSystemEditor();

  /*! \brief Create test system component editor
   */
  bool createTestSystemComponentEditor();

  /*! \brief Create test system unit editor
   */
  bool createTestSystemUnitEditor();


  /*! \brief Create test cable editor
   */
  bool createTestCableEditor();

  /*! \brief Create test node editor
   */
  bool createTestNodeEditor();

  /*! \brief Create test node MODBUS I/O tool
   */
  bool createTestNodeModbusIoTool();

  /*! \brief Close custom widgets
   */
  bool closeCustomWidgets();

  /*! \brief Clear all widgets
   */
  void clearAllWidgets();

  /*! \brief Prevent multiple object instances
   */
  mdtTtApplicationWidgets() {}

  // We define no destructer, it will never be called

  Q_DISABLE_COPY(mdtTtApplicationWidgets);

  // Editors
  std::shared_ptr<mdtTtTestSystemEditor> pvTestSystemEditor;
  std::shared_ptr<mdtTtTestSystemComponentEditor> pvTestSystemComponentEditor;
  std::shared_ptr<mdtTtTestSystemUnitEditor> pvTestSystemUnitEditor;
  
  std::shared_ptr<mdtTtTestCableEditor> pvTestCableEditor;
  std::shared_ptr<mdtTtTestNodeEditor> pvTestNodeEditor;
  // Tools
  std::shared_ptr<mdtTtTestNodeModbusIoTool> pvTestNodeModbusIoTool;
};

#endif // #ifndef MDT_TT_APPLICATION_WIDGETS_H
