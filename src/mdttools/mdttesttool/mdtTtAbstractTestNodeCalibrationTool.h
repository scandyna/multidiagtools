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
#ifndef MDT_TT_ABSTRACT_TEST_NODE_CALIBRATION_TOOL_H
#define MDT_TT_ABSTRACT_TEST_NODE_CALIBRATION_TOOL_H

///#include "mdtTtAbstractTester.h"
#include "mdtError.h"
#include "mdtTtTest.h"
#include "mdtTtTestNodeManager.h"
#include "mdtValue.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QObject>
#include <QMessageBox>
#include <memory>
#include <limits>

class mdtSqlFormWidget;
class mdtSqlRelation;
class QWidget;

/*! \biref Test node calibration tool base class
 */
class mdtTtAbstractTestNodeCalibrationTool : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtAbstractTestNodeCalibrationTool(QSqlDatabase db, QObject *parent = 0);

  /*! \brief Do some initialization
   */
  virtual bool init();

  /*! \brief
   *
   * All childs contained in widget that have a name prefixed fld_ will be mapped
   *  to corresponding fields in TestNode_view.
   *  Note that this a read only access.
   *
   * Given widget will also be used as parent for dialogs
   *  that are displayed (selection dialogs, message boxes).
   */
  void setTestNodeUiWidget(QWidget *widget);

  /*! \brief Get database instance
   */
  QSqlDatabase database() { return pvDatabase; }

  /*! \brief Get table model to access data in TestNodeUnit_view
   */
  std::shared_ptr<QSqlTableModel> testNodeUnitViewTableModel() { return pvTestNodeUnitViewTableModel; }

  /*! \brief Get test node manager object
   */
  std::shared_ptr<mdtTtTestNodeManager> testNodeManager() { return pvTestNodeManager; }

  /*! \brief Get test object
   */
  std::shared_ptr<mdtTtTest> test() { return pvTest; }

  /*! \brief Get current test node data for given field
   */
  QVariant currentTestNodeData(const QString & fieldName);

  /*! \brief Get test node unit data for given test node unit ID and field name
   */
  QVariant testNodeUnitData(int testNodeUnitId, const QString & fieldName);

  /*! \brief Get test node unit data for given test node unit schema position and field name
   *
   * Will return the data for first test node unit that matches given schemaPosition,
   *  in current test node.
   */
  QVariant testNodeUnitData(const QString & schemaPosition, const QString & fieldName);

  /*! \brief Set test node unit data for given test node unit ID and field name
   *
   * Note: data are cached in models only, not sent to database.
   */
  bool setTestNodeUnitData(int testNodeUnitId, const QString & fieldName, const QVariant & data);

  /*! \brief Set test node unit data for given test node unit schemaPosition and field name
   *
   * Notes:
   *  - Data are cached in models only, not sent to database.
   *  - The first test node unit that matches schemaPosition in current test node is considered.
   */
  bool setTestNodeUnitData(const QString & schemaPosition, const QString & fieldName, const QVariant & data);

  /*! \brief Set calibration offset for given testNodeUnitId
   */
  
 protected:

  /*! \brief Contains last error that occured
   */
  mdtError pvLastError;

  /*! \brief Display last error to the user
   */
  void displayLastError();

  /*! \brief Display a message to the user
   */
  void displayMessage(const QString & text, const QString & informativeText = QString(), QMessageBox::Icon icon = QMessageBox::Information);

  /*! \brief Prompt the user to do a action
   *
   * \return True if user accepted the message box (typically if he clicked Ok, Yes, ..),
   *          false if he rejected it (typically if Cancel, No, ... was clicked).
   */
  bool promptUser(const QString & text, const QString & informativeText = QString(), QMessageBox::Icon icon = QMessageBox::Information, QMessageBox::StandardButtons buttons = (QMessageBox::Ok | QMessageBox::Cancel));

  /*! \brief Ask a question to the user
   *
   * \return True if user accepted the message box (typically if he clicked Ok, Yes, ..),
   *          false if he rejected it (typically if Cancel, No, ... was clicked).
   */
  bool askUser(const QString & text, const QString & informativeText = QString(), QMessageBox::Icon icon = QMessageBox::Question, QMessageBox::StandardButtons buttons = (QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel));

  /*! \brief Check if x is in range
   *
   * Return true if value is in [min;max]
   *  If value is out of range, a descriptive text is stored to pvLastError
   *  You can use pvLastError.updateText() to set the main error text.
   *
   * Note: if value has -OL or +OL flag set, -infinity, respecively +infinity value is considered.
   */
  bool isInRange(const mdtValue & value, double min, double max = std::numeric_limits<double>::max());

 private:

  /*! \brief Get row in pvTestNodeUnitViewTableModel that matches given testNodeUnitId
   */
  int testNodeUnitViewTableModelRow(int testNodeUnitId);

  /*! \brief Get row in pvTestNodeUnitViewTableModel that matches given schemaPosition
   *
   * Note: will return first match for current test node
   */
  int testNodeUnitViewTableModelRow(const QString & schemaPosition);

  /*! \brief Get row in pvTestNodeUnitTableModel that matches given testNodeUnitId
   */
  int testNodeUnitTableModelRow(int testNodeUnitId);

  Q_DISABLE_COPY(mdtTtAbstractTestNodeCalibrationTool);

  QSqlDatabase pvDatabase;
  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
  std::shared_ptr<mdtTtTest> pvTest;
  std::shared_ptr<QSqlTableModel> pvTestNodeTableModel;
  std::shared_ptr<mdtSqlFormWidget> pvTestNodeForm;
  std::shared_ptr<QSqlTableModel> pvTestNodeUnitViewTableModel; // Access data in TestNodeUnit_view (RO)
  std::shared_ptr<mdtSqlRelation> pvTestNodeUnitViewRelation;
  std::shared_ptr<QSqlTableModel> pvTestNodeUnitTableModel;     // Access data in TestNodeUnit_tbl (R/W)
  std::shared_ptr<mdtSqlRelation> pvTestNodeUnitRelation;
  QWidget *pvParentWidget;        // For dialogs
};

#endif  // #ifndef MDT_TT_ABSTRACT_TEST_NODE_CALIBRATION_TOOL_H
