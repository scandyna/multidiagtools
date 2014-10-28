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
#ifndef MDT_TT_TEST_CABLE_OFFSET_TOOL_H
#define MDT_TT_TEST_CABLE_OFFSET_TOOL_H

#include "mdtError.h"
#include "mdtTtTest.h"
#include "mdtTtTestNodeManager.h"
#include "mdtValue.h"
///#include "mdtSqlDataWidgetController.h"
#include "mdtSqlTableViewController.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QObject>
#include <QList>
#include <QSqlRecord>
#include <QMessageBox>
#include <memory>
#include <limits>

class QWidget;
///class mdtTtTestItemNodeSetupData;
class mdtTtTestNodeSetupData;

/*! \biref Tool to reset offset of test connection cables
 */
class mdtTtTestCableOffsetTool : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestCableOffsetTool(QSqlDatabase db, QObject *parent = 0);

  /*! \brief Set intance of controller that acts on TestLink_view
   */
  void setTestLinkTableController(std::shared_ptr<mdtSqlTableViewController> controller);

 ///public slots:

  /*! \brief Run offset reset
   */
  bool runOffsetReset(const QVariant & testModelId);

 private:

  /*! \brief Get logical test link data for givent test model item ID
   */
  QList<QSqlRecord> getLogicalTestLinkData(const QVariant & testModelItemId, bool & ok);

  /*! \brief Save offset values to database
   */
  bool saveOffsetValues();

  /*! \brief Setup test nodes
   */
  bool setupTestNodes(const QVariant & testModelItemId);

  /*! \brief Setup I/O coupler
   */
  bool setupIoCoupler(const mdtTtTestNodeSetupData & nodeSetupData/**, const QString & nodeIdentification*/);

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

  /*! \brief Add instruments
   */
  void addInstruments();

  /*! \brief Connect to instruments
   */
  bool connectToInstruments();

  /*! \brief Disconnect from instruments
   */
  void disconnectFromInstruments();

  /*! \brief Beginn manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool beginTransaction();

  /*! \brief Rollback manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool rollbackTransaction();

  /*! \brief Commit manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool commitTransaction();

  Q_DISABLE_COPY(mdtTtTestCableOffsetTool);

  QSqlDatabase pvDatabase;
  std::shared_ptr<mdtTtTestNodeManager> pvTestNodeManager;
  std::shared_ptr<mdtTtTest> pvTest;
  std::shared_ptr<mdtSqlTableViewController> pvTestLinkTableController;
  QWidget *pvParentWidget;        // For dialogs
};

#endif // #ifndef MDT_TT_TEST_CABLE_OFFSET_TOOL_H
