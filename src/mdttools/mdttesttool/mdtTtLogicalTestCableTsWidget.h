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
#ifndef MDT_TT_LOGICAL_TEST_CABLE_TS_WIDGET_H
#define MDT_TT_LOGICAL_TEST_CABLE_TS_WIDGET_H

#include "ui_mdtTtLogicalTestCableTsWidget.h"
#include "mdtTtLogicalTestCableDialog.h"
#include <QGroupBox>
#include <QSqlDatabase>
#include <QString>

class mdtError;
class mdtClConnectableCriteria;

/*! \brief Helper class for logical test cable generation
 */
class mdtTtLogicalTestCableTsWidget : public QGroupBox, Ui::mdtTtLogicalTestCableTsWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtLogicalTestCableTsWidget(QWidget * parent, QSqlDatabase db);

  /*! \brief Set test cable connector
   */
  void setTestCableConnector(const QVariant & connectorId, const QString & name);

  /*! \brief Set test cable connection
   */
  void setTestCableConnection(const QVariant & connectionId, const QString & name);

  /*! \brief Set test system
   */
  void setTestSystem(const QVariant & tsVtId);

  /*! \brief Get cn type
   */
  inline mdtTtLogicalTestCableDialog::cnType_t cnType() const
  {
    return pvCnType;
  }

  /*! \brief Get cn ID
   */
  inline QVariant cnId() const{
    return pvCnId;
  }

  /*! \brief Get test cable cn name
   */
  QString testCableCnName() const
  {
    return lbTestCableCn->text();
  }

  /*! \brief Check if this affectation contains given (physical) test cable connection
   */
  bool containsTestCableConnection(const QVariant & testCableConnectionId, bool & ok);

  /*! \brief Get list of unit connections
   *
   * If affectation's type is a connector,
   *  all connections that are part of affected test system connector will be returned.
   * If affectation's type is a connection,
   *  test system affected connection will be returned.
   */
  QList<QVariant> getAffectedTsConnections(bool & ok);

 private slots:

  /*! \brief Select test system
   */
  void selectTestSystem();

  /*! \brief Select connector or connection
   */
  void selectCn();

 private:

  /*! \brief Select test system connector
   */
  void selectTsConnector();

  /*! \brief Select test system connection
   */
  void selectTsConnection();

  /*! \brief Get list of unit ID that are test connectors
   */
  QList<QVariant> getTestConnectorUnitIdList(bool & ok);

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtTtLogicalTestCableTsWidget);

  QVariant pvTestCableCnId;
  QVariant pvTestSystemId;
  mdtTtLogicalTestCableDialog::cnType_t pvCnType;
  QVariant pvCnId;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_TT_LOGICAL_TEST_CABLE_TS_WIDGET_H
