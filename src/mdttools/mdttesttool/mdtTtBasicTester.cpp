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
#include "mdtTtBasicTester.h"
#include "mdtTtTest.h"
#include "mdtTtTestModelData.h"
#include "mdtTtTestItemNodeSetupData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtSqlTableWidget.h"
#include "mdtDevice.h"
#include "mdtDeviceContainer.h"
///#include "mdtDeviceContainerWidget.h"
#include "mdtDevice.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceModbusWago.h"
#include <QString>
#include <QVariant>
#include <QMessageBox>
#include <QDate>
///#include <QVBoxLayout>
#include <QSqlTableModel>
#include <QSqlError>
#include <QTableView>
#include <memory>

#include <QDebug>

using namespace std;

mdtTtBasicTester::mdtTtBasicTester(QSqlDatabase db, QWidget *parent)
 : mdtTtAbstractTestWidget(db, parent)
{
  ///QVBoxLayout *l;
  ///mdtDeviceContainerWidget *deviceContainerWidget;

  ///database() = db;
  ///pvTestItemWidget = new mdtSqlTableWidget(this);
  ///setupUi(this);
  ///connectActions();
  // Setup instruments state widget
  /**
  l = new QVBoxLayout;
  deviceContainerWidget = new mdtDeviceContainerWidget;
  deviceContainerWidget->setContainer(testNodeManager()->container());
  l->addWidget(deviceContainerWidget);
  l->addStretch();
  */
  ///tbInstruments->setLayout(l);
}

bool mdtTtBasicTester::setup()
{
  if(!init()){
    return false;
  }
  if(!setupTables()){
    return false;
  }
  if(!addInstruments()){
    return false;
  }
  return true;
}

/**
void mdtTtBasicTester::createTest()
{
  ///pvTestData.clearValues();
  ///displayTestData(false);
  ///displayTestItemData();
  setTestModel();
}
*/

void mdtTtBasicTester::setTestModel()
{
  mdtSqlSelectionDialog dialog(this);
  mdtSqlTableSelection s;
  QString sql;

  /// \todo Check save, create new
  
  /// \todo Lorsqu'un test a déjà été créé, décider selon état (juste créé, déjà exécuté, .....)
  
  //

  // Setup and show dialog to select test model
  sql = "SELECT * FROM TestModel_tbl";
  dialog.setMessage(tr("Please select a test model:"));
  dialog.setQuery(sql, database(), false);
  
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Set test data
  s = dialog.selection("Id_PK");
  if(s.isEmpty()){
    return;
  }
  Q_ASSERT(s.rowCount() == 1);
  setTestDataValue("TestModel_Id_FK", s.data(0, "Id_PK"));
  
  ///qDebug() << "Test data: " << pvTestData;
  
  // Save data and update GUI
  if(testData().value("Id_PK").isNull()){
    ///createNewTest();
  }else{
    saveTestData();
  }
  ///displayTestData(true);
  ///displayTestItemData();
}

/**
void mdtTtBasicTester::saveTest()
{
  if(testData().value("Id_PK").isNull()){
    return;
  }
  if(!saveTestData()){
    return;
  }
  ///displayTestData(false);
  ///displayTestItemData();
}
*/

void mdtTtBasicTester::runTest()
{
  QVariant testItemId;
  shared_ptr<mdtDeviceU3606A> multimeter;
  mdtValue measuredValue;

  multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);

  if(!connectToInstruments()){
    return;
  }

  // Setup multimeter as Ohmmeter
  ///multimeter->sendCommand("CONF:RES 100, 1e-3");
  multimeter->sendCommand("CONF:RES");
  if(multimeter->getMeasureConfiguration() != mdtFrameCodecScpiU3606A::MT_RESISTANCE){
    pvLastError.setError(tr("Setup U3606A as ohmmeter failed"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
    pvLastError.commit();
    displayLastError();
    disconnectFromInstruments();
    return;
  }

  
  qDebug() << "Running test ...";
  test()->resetTestItemCursor();
  while(test()->hasMoreTestItem()){
    testItemId = test()->nextTestItem();
    qDebug() << "Curremt item: " << testItemId;
    ///setTestItemData(testItemId, "Result", "Running ...");
    test()->setCurrentTestItemData("Result", "Running ...");
    if(!setupInstruments(testItemId)){
      disconnectFromInstruments();
      return;
    }
    
    measuredValue = multimeter->getMeasureValue();
    
    test()->setCurrentTestItemData("MeasuredValue", measuredValue.valueDouble());
    test()->setCurrentTestItemData("Result", "Finished");
  }

  disconnectFromInstruments();
}

/**
bool mdtTtBasicTester::createNewTest()
{
  QVariant testId;

  // Update somes values
  setTestDataValue("Date", QDate::currentDate());
  ///pvTestData.setValue("DutSerialNumber", leSN->text());
  // Add test to DB
  testId = test()->addTest(testData());
  if(testId.isNull()){
    pvLastError = test()->lastError();
    displayLastError();
    return false;
  }
  setTestDataValue("Id_PK", testId);

  return true;
}
*/

bool mdtTtBasicTester::saveTestData()
{
  mdtTtTestData data;

  // Update somes values
  ///pvTestData.setValue("DutSerialNumber", leSN->text());
  data = testData();
  /**
  if(!test()->updateTest(data.value("Id_PK"), data)){
    pvLastError = test()->lastError();
    displayLastError();
    return false;
  }
  */
  setTestData(data);

  return true;
}

/**
void mdtTtBasicTester::displayTestData(bool getFromDatabase)
{
  bool ok;
  QVariant testId;
  QDate date;

  // Get data from DB
  testId = pvTestData.value("Id_PK");
  if(getFromDatabase && (!testId.isNull())){
    pvTestData = test()->getTestData(testId, true, &ok);
    if(!ok){
      pvLastError = test()->lastError();
      displayLastError();
      return;
    }
  }
  // Update widgets
  date = pvTestData.value("Date").toDate();
  ///lbDate->setText(date.toString(Qt::SystemLocaleLongDate));
  ///lbTestDesignationEN->setText(pvTestData.modelData().value("DesignationEN").toString());
  ///leSN->setText(pvTestData.value("DutSerialNumber").toString());
  

  
}
*/

/**
void mdtTtBasicTester::setTestItemData(const QVariant & testItemId, const QString & fieldName, const QVariant & data)
{
  ///Q_ASSERT(pvTestItemWidget != 0);

  ///QSqlTableModel *m;
  shared_ptr<QSqlTableModel> m;
  QModelIndex index;
  int col, row;

  ///m = pvTestItemWidget->model();
  ///m = testItemTableModel();
  ///Q_ASSERT(m);

  col = m->fieldIndex("Id_PK");
  if(col < 0){
    return;
  }
  // Make shure we have all rows cached in model
  if(m->rowCount() < 1){
    return;
  }
  while(m->canFetchMore()){
    m->fetchMore();
  }
  // Search test item ID and edit
  for(row = 0; row < m->rowCount(); ++row){
    index = m->index(row, col);
    if(m->data(index) == testItemId){
      col = m->fieldIndex(fieldName);
      index = m->index(row, col);
      m->setData(index, data);
      return;
    }
  }
}
*/

bool mdtTtBasicTester::addInstruments()
{
  testNodeManager()->addDevice<mdtDeviceU3606A>("U3606A", "", "U3606A Multimeter");
  testNodeManager()->addDevice<mdtDeviceModbusWago>("W750", "0", "Wago 750 coupling node");

  return true;
}

bool mdtTtBasicTester::connectToInstruments()
{
  shared_ptr<mdtDeviceU3606A> multimeter;
  shared_ptr<mdtDeviceModbusWago> coupler;

  multimeter = testNodeManager()->device<mdtDeviceU3606A>("U3606A");
  Q_ASSERT(multimeter);
  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);

  // Connect to multimeter
  if(multimeter->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + multimeter->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTester");
    pvLastError.commit();
    displayLastError();
    return false;
  }
  
  // Connect to coupler
  if(coupler->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to ") + coupler->name(), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTester");
    pvLastError.commit();
    displayLastError();
    return false;
  }
  if(!coupler->detectIos()){
    pvLastError = coupler->lastError();
    displayLastError();
    return false;
  }

  return true;
}

bool mdtTtBasicTester::disconnectFromInstruments()
{
  Q_ASSERT(testNodeManager()->container());

  testNodeManager()->container()->disconnectFromDevices();
}

bool mdtTtBasicTester::setupInstruments(const QVariant & testItemId)
{
  mdtTtTestItemNodeSetupData setupData;
  mdtTtTestNodeSetupData nodeSetupData;
  bool ok;

  setupData = test()->getSetupData(testItemId, ok);
  if(!ok){
    pvLastError = test()->lastError();
    displayLastError();
    return false;
  }
  while(setupData.hasMoreStep()){
    nodeSetupData = setupData.getNextStep();
    qDebug() << " Has setup for node " << nodeSetupData.nodeIdentification();
    if(!setupTestNode(nodeSetupData)){
      return false;
    }
  }

  return true;
}

bool mdtTtBasicTester::setupTestNode(const mdtTtTestNodeSetupData & setupData)
{
  qDebug() << "Setup node " << setupData.nodeIdentification();

  if(setupData.nodeIdentification() == "0"){
    return setupIoCoupler0(setupData);
  }
  
  return true;
}

bool mdtTtBasicTester::setupIoCoupler0(const mdtTtTestNodeSetupData & setupData)
{
  Q_ASSERT(setupData.nodeIdentification() == "0");

  QList<mdtTtTestNodeUnitSetupData> setupDataList;
  mdtTtTestNodeUnitSetupData unitSetupData;
  shared_ptr<mdtDeviceModbusWago> coupler;
  int i;

  coupler = testNodeManager()->device<mdtDeviceModbusWago>("W750");
  Q_ASSERT(coupler);
  // Set all outputs to 0
  Q_ASSERT(coupler->ios() != 0);
  coupler->ios()->setAnalogOutputsValue(0);
  coupler->ios()->setDigitalOutputsValue(false);
  // Set values of each I/O
  setupDataList = setupData.unitSetupList();
  for(i = 0; i < setupDataList.size(); ++i){
    unitSetupData = setupDataList.at(i);
    qDebug() << " Unit: " << unitSetupData.schemaPosition() << ", I/O pos: " << unitSetupData.ioPosition() << " , type: " << unitSetupData.ioType();
    switch(unitSetupData.ioType()){
      case mdtTtTestNodeUnitSetupData::AnalogOutput:
        if(coupler->setAnalogOutputValueAt(unitSetupData.ioPosition(), unitSetupData.value("Value").toDouble(), false, false) < 0){
          pvLastError = coupler->lastError();
          displayLastError();
          return false;
        }
        break;
      case mdtTtTestNodeUnitSetupData::DigitalOutput:
        if(coupler->setDigitalOutputValueAt(unitSetupData.ioPosition(), unitSetupData.value("State").toBool(), false, false) < 0){
          pvLastError = coupler->lastError();
          displayLastError();
          return false;
        }
        break;
    }
  }
  // Send to coupler
  if(coupler->setAnalogOutputs(true) < 0){
    pvLastError = coupler->lastError();
    displayLastError();
    return false;
  }
  if(coupler->setDigitalOutputs(true) < 0){
    pvLastError = coupler->lastError();
    displayLastError();
    return false;
  }
  // Let relays some time to switch
  coupler->wait(50);
  ///coupler->wait(300);

  return true;
}

bool mdtTtBasicTester::setupTables()
{
  /**
  if(!pvTestData.setup(database(), true)){
    pvLastError = pvTestData.lastError();
    return false;
  }
  */
  /**
  if(!setupTestItemTable()){
    return false;
  }
  */
  /**
  if(!test()->setupTestItemModel()){
    pvLastError = test()->lastError();
    return false;
  }
  */
  return true;
}

/**
bool mdtTtBasicTester::setupTestItemTable()
{
  ///Q_ASSERT(pvTestItemWidget != 0);

  ///QVBoxLayout *l;
  shared_ptr<QSqlTableModel> m;
  QSqlError sqlError;

  // Setup table widget in Tab widget
  ///l = new QVBoxLayout;
  ///l->addWidget(pvTestItemWidget);
  ///tbTestItem->setLayout(l);
  // Setup table widget
  ///m = new QSqlTableModel(pvTestItemWidget, database());
  m = testItemTableModel();
  m->setTable("TestItem_view");
  if(!m->select()){
    sqlError = m->lastError();
    pvLastError.setError(tr("Cannot select data from table 'TestItem_view'"), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTester");
    pvLastError.commit();
    return false;
  }
  pvTestItemWidget->setModel(m.get());
  // Hide technical fields
  pvTestItemWidget->setColumnHidden("Id_PK", true);
  pvTestItemWidget->setColumnHidden("Test_Id_FK", true);
  pvTestItemWidget->setColumnHidden("TestModelItem_Id_FK", true);
  // Set fields some user friendly names
  pvTestItemWidget->setHeaderData("SequenceNumber", tr("Seq. #"));
  pvTestItemWidget->setHeaderData("DesignationEN", tr("Designation (English)"));
  pvTestItemWidget->setHeaderData("ExpectedValue", tr("Value\nexpected"));
  pvTestItemWidget->setHeaderData("MeasuredValue", tr("Value\nmeasured"));
  // Other things
  pvTestItemWidget->addColumnToSortOrder("SequenceNumber", Qt::AscendingOrder);
  pvTestItemWidget->sort();
  pvTestItemWidget->tableView()->resizeColumnsToContents();
  pvTestItemWidget->tableView()->resizeRowsToContents();
  displayTestItemData();

  return true;
}
*/
