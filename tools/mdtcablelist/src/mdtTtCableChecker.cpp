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
#include "mdtTtCableChecker.h"
#include "ui_mdtTtCableChecker.h"
///#include "mdtTtTestModel.h"
#include "mdtTtTest.h"
#include "mdtDevice.h"
#include <mdtDeviceIos.h>
#include "mdtDeviceModbusWago.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlRelation.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtDeviceU3606A.h"
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtDeviceInfo.h"
#include "mdtPortStatusWidget.h"
#include "mdtPortManager.h"
#include "mdtAbstractPort.h"
#include "mdtValue.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
///#include <QString>
///#include <QItemSelectionModel>
#include <QMessageBox>
///#include <QInputDialog>
#include <QWidget>
#include <QDataWidgetMapper>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>

using namespace mdtTtCableCheckerPrivate;

struct mdtTtCableCheckerPrivate::deviceStatusWidget
{
  QLabel *label;
  mdtPortStatusWidget *statusWidget;
};



mdtTtCableChecker::mdtTtCableChecker(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
  pvMultimeter = 0;
  pvDeviceStatusWidgetsLayout = 0;
  pvTest = new mdtTtTest(this, db);
}

mdtTtCableChecker::~mdtTtCableChecker()
{
  delete pvTest;  /// \todo Check if OK (now, mdtTtTest inherits QObject, via mdtTtBase)
  removeMultimeter();
  removeIoNodes();
}

bool mdtTtCableChecker::setupTables()
{
  if(!setupTestTable()){
    return false;
  }
  if(!setupTestItemTable()){
    return false;
  }
  createMultimeter();
  createIoNodes();
  return true;
}

void mdtTtCableChecker::setTestModel()
{
  ///mdtTtTest t(database());
  QVariant testResultId;
  QVariant baseTestId;

  // Get current test result ID
  testResultId = currentData("Test_tbl", "Id_PK");
  if(testResultId.isNull()){
    return;
  }
  // Select a test model
  baseTestId = selectTestModel();
  if(baseTestId.isNull()){
    return;
  }
  // Set test model
  if(!pvTest->setTestModel(testResultId, baseTestId)){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }
  /**
  if(!t.setTestModel(testResultId, baseTestId)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  */
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  /**
  if(!setCurrentData("Test_tbl", "TestModel_Id_FK", baseTestId)){
    return;
  }
  */
}

void mdtTtCableChecker::removeTestResult()
{
  ///mdtTtTest t(database());
  QMessageBox msgBox;
  QVariant testResultId;

  // Get current test result ID
  testResultId = currentData("Test_tbl", "Id_PK");
  if(testResultId.isNull()){
    return;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to remove test result."));
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  if(msgBox.exec() != QMessageBox::Yes){
    return;
  }
  // Delete test result items
  if(!pvTest->removeData("TestItem_tbl", "Test_Id_FK", testResultId)){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }
  /**
  if(!t.removeData("TestItem_tbl", "Test_Id_FK", testResultId)){
    pvLastError = t.lastError();
    displayLastError();
    return;
  }
  */
  // Delete test result
  mainSqlWidget()->remove();
  // Update connections table
  ///form()->select("TestItem_view");
}

void mdtTtCableChecker::runTest()
{
  QVariant testId;
  QVariant testItemId;
  QList<QVariant> testItemIdList;
  int i;
  mdtValue expectedValue;
  mdtValue measuredValue;

  // Get current test ID
  testId = currentData("Test_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  // Get items for current test
  testItemIdList = pvTest->getTestItemIdListForTestId(testId);
  if(testItemIdList.isEmpty()){
    return;
  }
  // Connect to instruments
  if(!connectToInstruments()){
    displayLastError();
    return;
  }
  /// \todo Tester absence de tension
  
  // Setup multimeter as Ohmmeter
  pvMultimeter->sendCommand("CONF:RES 100, 1e-3");
  if(pvMultimeter->getMeasureConfiguration() != mdtFrameCodecScpiU3606A::MT_RESISTANCE){
    pvLastError.setError(tr("Setup U3606A as ohmmeter failed"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
    pvLastError.commit();
    displayLastError();
    disconnectFromInstruments();
    return;
  }
  // Run items
  for(i = 0; i < testItemIdList.size(); ++i){
    testItemId = testItemIdList.at(i);
    qDebug() << "Running item " << testItemId;
    /// \todo Setup coupling nodes
    qDebug() << "Setup coupling nodes ...";
    if(!setupNodes(testItemId)){
      displayLastError();
      disconnectFromInstruments();
      return;
    }
    // Measure value
    qDebug() << "Running measure ...";
    measuredValue = pvMultimeter->getMeasureValue();
    qDebug() << "-> DONE: " << measuredValue;
    // Store value
    /**
    if(!pvTest->setMeasuredValue(testItemId, measuredValue)){
      pvLastError = pvTest->lastError();
      displayLastError();
      disconnectFromInstruments();
      return;
    }
    */
  }
  // Disconnect from instruments
  disconnectFromInstruments();
  // Save results to database
  /**
  if(!pvTest->submitTestItemSqlModelData()){
    pvLastError = pvTest->lastError();
    displayLastError();
    return;
  }
  */
}

QVariant mdtTtCableChecker::selectTestModel()
{
  mdtSqlSelectionDialog selectionDialog;
  ///QSqlError sqlError;
  ///QSqlQueryModel model;
  QString sql;

  // Setup model
  sql = "SELECT * FROM TestModel_tbl";
  /**
  model.setQuery(sql, database());
  sqlError = model.lastError();
  if(sqlError.isValid()){
    pvLastError.setError(tr("Unable to get list of tests."), mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
    pvLastError.commit();
    displayLastError();
    return QVariant();
  }
  */
  // Setup and show dialog
  selectionDialog.setMessage(tr("Please select test"));
  ///selectionDialog.setModel(&model, false);
  selectionDialog.setQuery(sql, database(), false);
  selectionDialog.setColumnHidden("Id_PK", true);
  ///selectionDialog.setHeaderData("Unit_Id_FK", tr("Variant"));
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.resize(700, 300);
  if(selectionDialog.exec() != QDialog::Accepted){
    return QVariant();
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);

  return selectionDialog.selectionResult().at(0);
}

bool mdtTtCableChecker::setupTestTable() 
{
  Ui::mdtTtCableChecker cc;
  QSqlTableModel *testResultModel;
  QDataWidgetMapper *baseTestMapper;
  QSqlTableModel *baseTestModel;
  mdtSqlRelation *baseTestRelation;

  // Setup main form widget
  cc.setupUi(mainSqlWidget());
  // Setup form
  if(!setMainTable("Test_tbl", "Test result"/*, database()*/)){
    return false;
  }
  /*
   * Setup base test widget mapping
   */
  testResultModel = model("Test_tbl");
  Q_ASSERT(testResultModel != 0);
  // Setup base test model
  baseTestModel = new QSqlTableModel(this, database());
  baseTestModel->setTable("TestModel_tbl");
  if(!baseTestModel->select()){
    return false;
  }
  // Setup base test widget mapper
  baseTestMapper = new QDataWidgetMapper(this);
  baseTestMapper->setModel(baseTestModel);
  baseTestMapper->addMapping(cc.leTestDesignationEN, baseTestModel->fieldIndex("DesignationEN"));
  // Setup base test relation
  baseTestRelation = new mdtSqlRelation(this);
  baseTestRelation->setParentModel(testResultModel);
  baseTestRelation->setChildModel(baseTestModel);
  if(!baseTestRelation->addRelation("TestModel_Id_FK", "Id_PK", false)){
    return false;
  }
  connect(mainSqlWidget(), SIGNAL(currentRowChanged(int)), baseTestRelation, SLOT(setParentCurrentIndex(int)));
  connect(baseTestRelation, SIGNAL(childModelFilterApplied()), baseTestMapper, SLOT(toFirst()));
  connect(baseTestRelation, SIGNAL(childModelIsEmpty()), baseTestMapper, SLOT(revert()));
  // Force a update
  mainSqlWidget()->setCurrentIndex(mainSqlWidget()->currentRow());

  return true;
}

bool mdtTtCableChecker::setupTestItemTable() 
{
  mdtSqlTableWidget *widget;
  QSqlTableModel *m;

  if(!addChildTable("TestItem_view", tr("Test result items") /*,database()*/)){
    return false;
  }
  if(!addRelation("Id_PK", "TestItem_view", "Test_Id_FK")){
    return false;
  }
  widget = sqlTableWidget("TestItem_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("TestModel_Id_FK", true);
  widget->setColumnHidden("Test_Id_FK", true);
  widget->setColumnHidden("TestModelItem_Id_FK", true);
  widget->setColumnHidden("TestConnectorNameBusA", true);
  widget->setColumnHidden("TestContactNameBusA", true);
  widget->setColumnHidden("TestConnectorNameBusB", true);
  widget->setColumnHidden("TestContactNameBusB", true);
  // Set fields a user friendly name
  widget->setHeaderData("SequenceNumber", tr("Seq. #"));
  widget->setHeaderData("ExpectedValue", tr("Value\nExpected"));
  widget->setHeaderData("MeasuredValue", tr("Value\nMeasured"));
  widget->setHeaderData("TestConnectorNameBusA", tr("Test\nConnector\nBus A"));
  widget->setHeaderData("TestConnectorNameBusB", tr("Test\nConnector\nBus B"));
  widget->setHeaderData("TestContactNameBusA", tr("Test\nContact\nBus A"));
  widget->setHeaderData("TestContactNameBusB", tr("Test\nContact\nBus B"));
  widget->setHeaderData("DutUnitSchemaPositionBusA", tr("DUT BUS A"));
  widget->setHeaderData("DutUnitSchemaPositionBusB", tr("DUT BUS B"));
  widget->setHeaderData("DutConnectorNameBusA", tr("DUT\nConnector\nBus A"));
  widget->setHeaderData("DutConnectorNameBusB", tr("DUT\nConnector\nBus B"));
  widget->setHeaderData("DutContactNameBusA", tr("DUT\nContact\nBus A"));
  widget->setHeaderData("DutContactNameBusB", tr("DUT\nContact\nBus B"));
  // Set some attributes on table view
  widget->tableView()->resizeColumnsToContents();
  // Add buttons
  /**
  pbSetTestLink = new QPushButton(tr("Set test link ..."));
  connect(pbSetTestLink, SIGNAL(clicked()), this, SLOT(setTestLink()));
  widget->addWidgetToLocalBar(pbSetTestLink);
  widget->addStretchToLocalBar();
  */
  // Register model to test helper object
  m = model("TestItem_view");
  Q_ASSERT(m != 0);
  /**
  if(!pvTest->setTestItemSqlModel(m)){
    pvLastError = pvTest->lastError();
    displayLastError();
    return false;
  }
  */

  return true;
}

void mdtTtCableChecker::createMultimeter()
{
  pvMultimeter = new mdtDeviceU3606A;
  pvMultimeter->setName("U3606A");
  addDeviceStatusWidget(pvMultimeter, "U3606A");
}

void mdtTtCableChecker::removeMultimeter()
{
  if(pvMultimeter != 0){
    pvMultimeter->disconnectFromDevice();
    removeDeviceStatusWidget(pvMultimeter);
    delete pvMultimeter;
    pvMultimeter = 0;
  }
}

void mdtTtCableChecker::createIoNodes()
{
  QList<QVariant> hwNodeIdList;
  QVariant testId;
  mdtDeviceModbusWago *device;
  ///mdtDeviceIos *nodeIos;
  QString name;
  int i;

  // Get test ID and HW node ID list
  testId = currentData("Test_tbl", "Id_PK");
  if(testId.isNull()){
    return;
  }
  hwNodeIdList = pvTest->getHardwareNodeIdListForTestId(testId);
  // Create nodes
  for(i = 0; i < hwNodeIdList.size(); ++i){
    device = new mdtDeviceModbusWago;
    ///nodeIos = new mdtDeviceIos(this);
    name = "I/O node ID " + hwNodeIdList.at(i).toString();
    device->setName(name);
    device->setHardwareNodeId(hwNodeIdList.at(i).toInt(), 8, 0);
    ///device->setIos(nodeIos); \todo Corriger !!
    pvModbusIoNodes.append(device);
    addDeviceStatusWidget(device, name);
  }
}

void mdtTtCableChecker::removeIoNodes()
{
  mdtDeviceModbusWago *device;
  int i;

  for(i = 0; i < pvModbusIoNodes.size(); ++i){
    device = pvModbusIoNodes.at(i);
    Q_ASSERT(device != 0);
    device->disconnectFromDevice();
    removeDeviceStatusWidget(device);
    delete device;
  }
  pvModbusIoNodes.clear();
}

bool mdtTtCableChecker::connectToInstruments()
{
  Q_ASSERT(pvMultimeter != 0);

  QList<QVariant> hwNodeIdListVar;
  QList<int> hwNodeIdList;
  QVariant testId;
  mdtDeviceModbusWago *ioNode;
  int i;

  // Get test ID and HW node ID list
  testId = currentData("Test_tbl", "Id_PK");
  if(testId.isNull()){
    return false;
  }
  hwNodeIdListVar = pvTest->getHardwareNodeIdListForTestId(testId);
  for(i = 0; i < hwNodeIdListVar.size(); ++i){
    hwNodeIdList.append(hwNodeIdListVar.at(i).toInt());
  }
  // Connect to multimeter
  if(pvMultimeter->connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){
    pvLastError.setError(tr("Cannot connect to U3606A"), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
    pvLastError.commit();
    return false;
  }
  // Connect to nodes
  for(i = 0; i < pvModbusIoNodes.size(); ++i){
    ioNode = pvModbusIoNodes.at(i);
    Q_ASSERT(ioNode != 0);
    // Connect
    if(ioNode->connectToDevice(hwNodeIdList) != mdtAbstractPort::NoError){
      pvLastError.setError(tr("Cannot connect to ") + ioNode->name(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
      pvLastError.commit();
      disconnectFromInstruments();
      return false;
    }
    // Setup I/Os
    if(!ioNode->detectIos()){
      pvLastError.setError(tr("Cannot detect I/Os in ") + ioNode->name(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
      pvLastError.commit();
      disconnectFromInstruments();
      return false;
    }
    ///ioNode->setIos(ioNode->ios(), true); \todo Corriger
  }

  return true;
}

void mdtTtCableChecker::disconnectFromInstruments()
{
  Q_ASSERT(pvMultimeter != 0);

  mdtDeviceModbusWago *ioNode;
  int i;

  // Disconnect from multimeter
  pvMultimeter->disconnectFromDevice();
  // Disconnect from nodes
  for(i = 0; i < pvModbusIoNodes.size(); ++i){
    ioNode = pvModbusIoNodes.at(i);
    Q_ASSERT(ioNode != 0);
    ioNode->disconnectFromDevice();
  }
}

bool mdtTtCableChecker::setupNodes(const QVariant & testItemId)
{
  ///QList<mdtTtTestNodeUnitSetupData> setupDataList;
  QList<QSqlRecord> setupDataList;
  ///mdtTtTestNodeUnitSetupData setupData;
  mdtDeviceModbusWago *ioNode = 0;
  int i, k;

  for(i = 0; i < pvModbusIoNodes.size(); ++i){
    ioNode = pvModbusIoNodes.at(i);
    Q_ASSERT(ioNode != 0);
    // Get setup data
    setupDataList = pvTest->getNodeUnitSetupList(testItemId, ioNode->hardwareNodeId());
    // Set all outputs OFF
    Q_ASSERT(ioNode->ios() != 0);
    ioNode->ios()->setDigitalOutputsValue(false);
    // Setup all specified node units
    for(k = 0; k < setupDataList.size(); ++k){
      ///mdtTtTestNodeUnitSetupData setupData;
      ///setupData = setupDataList.at(k);
      ///QSqlRecord rec = setupData;
      QSqlRecord rec = setupDataList.at(k);
      qDebug() << "CC, rec: " << rec;
      ///setupData = setupDataList.at(k);
      /// \todo Check data validity !
      if(rec.value("NodeId") == ioNode->hardwareNodeId()){
        if(ioNode->setDigitalOutputValueAt(rec.value("IoPosition").toInt(), rec.value("State").toBool(), false, false) < 0){
          pvLastError.setError(tr("Cannot set digital output ") + rec.value("SchemaPosition").toString() + tr(" for ") + ioNode->name(), mdtError::Error);
          MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
          pvLastError.commit();
          return false;
        }
      }
      /**
      if(setupData.nodeId() == ioNode->hardwareNodeId()){
        if(ioNode->setDigitalOutputValue(setupData.schemaPosition().toString(), false, false, false) < 0){
          pvLastError.setError(tr("Cannot set digital output ") + setupData.schemaPosition().toString() + tr(" for ") + ioNode->name(), mdtError::Error);
          MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
          pvLastError.commit();
          return false;
        }
      }
      */
    }
    // Send query to node
    if(ioNode->setDigitalOutputs(true) < 0){
      pvLastError.setError(tr("Cannot set digital outputs to ") + ioNode->name(), mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtTtCableChecker");
      pvLastError.commit();
      return false;
    }
  }
  // We wait some time to be shure that relays are set
  if(ioNode != 0){
    ioNode->wait(50);
  }

  return true;
}

void mdtTtCableChecker::addDeviceStatusWidget(mdtDevice *device, const QString & label)
{
  Q_ASSERT(device != 0);

  deviceStatusWidget *dsw;
  int row;

  // Create status widget page, if needed
  if(pvDeviceStatusWidgetsLayout == 0){
    QWidget *w = new QWidget;
    QVBoxLayout *l = new QVBoxLayout;
    pvDeviceStatusWidgetsLayout = new QGridLayout;
    l->addLayout(pvDeviceStatusWidgetsLayout);
    l->addStretch();
    w->setLayout(l);
    addChildWidget(w, tr("Instruments states"));
  }
  Q_ASSERT(pvDeviceStatusWidgetsLayout != 0);
  // Create status widget
  dsw = new deviceStatusWidget;
  dsw->label = new QLabel(label);
  dsw->statusWidget = new mdtPortStatusWidget;
  pvDeviceStatusWidgets.insert(device, dsw);
  // Layout status widget and label
  row = pvDeviceStatusWidgetsLayout->rowCount();
  pvDeviceStatusWidgetsLayout->addWidget(dsw->label, row, 0, Qt::AlignVCenter);
  pvDeviceStatusWidgetsLayout->addWidget(dsw->statusWidget, row, 1, Qt::AlignVCenter);
  // make connections
  connect(device, SIGNAL(statusMessageChanged(const QString&, const QString&, int)), dsw->statusWidget, SLOT(showMessage(const QString&, const QString&, int)));
  Q_ASSERT(device->portManager() != 0);
  connect(device->portManager(), SIGNAL(stateChangedForUi(int, const QString&, int, bool)), dsw->statusWidget, SLOT(setState(int, const QString&, int, bool)));
  device->portManager()->notifyCurrentState();
}

void mdtTtCableChecker::removeDeviceStatusWidget(mdtDevice *device)
{
  deviceStatusWidget *dsw;

  dsw = pvDeviceStatusWidgets.value(device, 0);
  if(dsw != 0){
    delete dsw->label;
    delete dsw->statusWidget;
    delete dsw;
    pvDeviceStatusWidgets.remove(device);
  }
}
