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
#include "mdtCsvFileParserSettingsDialog.h"

#include "mdt/csv/RecordFormatSetupWidget.h"

#include "mdt/csv/ParserFormatSetupProxyModel.h"
#include "mdt/csv/ParserFormatSetupDelegate.h"
#include "mdtCsvFileParserModel.h"
#include "mdtErrorDialog.h"
#include <QTableView>
#include <QScrollArea>
// #include <QState>

#include <QDebug>

mdtCsvFileParserSettingsDialog::mdtCsvFileParserSettingsDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  wFileSettings->setSelectFileMode(mdtCsvFileSettingsWidget::SelectOpen);
  // Setup record format widget
//   pvRecordFormatWidget = new mdt::csv::RecordFormatSetupWidget;
//   saRecordFormat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//   saRecordFormat->setWidget(pvRecordFormatWidget);
//   connect(pvRecordFormatWidget, &mdt::csv::RecordFormatSetupWidget::fieldTypeChanged, this, &mdtCsvFileParserSettingsDialog::onFieldTypeChanged);
  // Setup data preview
  pvDataPreviewModel = new mdtCsvFileParserModel(tvDataPreview);
  auto *proxyModel = new mdt::csv::ParserFormatSetupProxyModel(tvDataPreview);
  proxyModel->setSourceModel(pvDataPreviewModel);
  tvDataPreview->setModel(proxyModel);
  ///tvDataPreview->setModel(pvDataPreviewModel);
  auto *delegate = new mdt::csv::ParserFormatSetupDelegate(tvDataPreview);
  tvDataPreview->setItemDelegateForRow(0, delegate);
  connect(wFileSettings, &mdtCsvFileSettingsWidget::fileSettingsChanged, this, &mdtCsvFileParserSettingsDialog::onFileSettingsChanged);
  connect(wCsvSettings, &mdtCsvParserSettingsWidget::csvSettingsChanged, this, &mdtCsvFileParserSettingsDialog::onCsvSettingsChanged);
//   setupStateMachine();
}

void mdtCsvFileParserSettingsDialog::setFileSettings(const QString & path, const QByteArray & encoding)
{
  wFileSettings->setFileSettings(path, encoding);
}

QString mdtCsvFileParserSettingsDialog::filePath() const
{
  return wFileSettings->filePath();
}

QByteArray mdtCsvFileParserSettingsDialog::fileEncoding() const
{
  return wFileSettings->fileEncoding();
}

void mdtCsvFileParserSettingsDialog::setCsvSettings(const mdtCsvParserSettings &settings)
{
  wCsvSettings->setSettings(settings);
}

mdtCsvParserSettings mdtCsvFileParserSettingsDialog::getCsvSettings() const
{
  return wCsvSettings->getSettings();
}

void mdtCsvFileParserSettingsDialog::onFileSettingsChanged(const QString & path, const QByteArray & encoding)
{
  if(path.isEmpty()){
    return;
  }
  if(encoding.isEmpty()){
    return;
  }
  setControlsEnabled(false);
  bool ok = pvDataPreviewModel->setFile(path, encoding);
  setControlsEnabled(true);
  if(!ok){
    mdtErrorDialog dialog(pvDataPreviewModel->lastError(), this);
    dialog.exec();
    return;
  }
//   pvRecordFormatWidget->setHeader(pvDataPreviewModel->header());
  resizeViewToContents();
}

void mdtCsvFileParserSettingsDialog::onCsvSettingsChanged(const mdtCsvParserSettings & settings)
{
  if(!settings.isValid()){
    return;
  }
  setControlsEnabled(false);
  bool ok = pvDataPreviewModel->setCsvSettings(settings);
  setControlsEnabled(true);
  if(!ok){
    mdtErrorDialog dialog(pvDataPreviewModel->lastError(), this);
    dialog.exec();
    return;
  }
//   pvRecordFormatWidget->setHeader(pvDataPreviewModel->header());
  resizeViewToContents();
}

// void mdtCsvFileParserSettingsDialog::onFieldTypeChanged(int fieldIndex, int type)
// {
//   if( !pvDataPreviewModel->reformatColumnData(fieldIndex, static_cast<QMetaType::Type>(type) ) ){
//     mdtErrorDialog dialog(pvDataPreviewModel->lastError(), this);
//     dialog.exec();
//     return;
//   }
//   resizeViewToContents();
// }

// void mdtCsvFileParserSettingsDialog::onStateIdleEntered()
// {
//   qDebug() << "SM: Idle state entered";
//   setControlsEnabled(true);
// }

// void mdtCsvFileParserSettingsDialog::onStateParsingFileEntered()
// {
//   qDebug() << "SM: ParsingFile state entered";
//   setControlsEnabled(false);
// }

void mdtCsvFileParserSettingsDialog::setControlsEnabled(bool enable)
{
  wFileSettings->setEnabled(enable);
  wCsvSettings->setEnabled(enable);
//   pvRecordFormatWidget->setEnabled(enable);
}

void mdtCsvFileParserSettingsDialog::resizeViewToContents()
{
  tvDataPreview->resizeColumnsToContents();
  tvDataPreview->resizeRowsToContents();
}

// void mdtCsvFileParserSettingsDialog::setupStateMachine()
// {
//   auto *IdleState = new QState;
//   auto *ParsingFileState = new QState;
//   
//   connect(IdleState, &QState::entered, this, &mdtCsvFileParserSettingsDialog::onStateIdleEntered);
//   connect(ParsingFileState, &QState::entered, this, &mdtCsvFileParserSettingsDialog::onStateParsingFileEntered);
//   
//   IdleState->addTransition(pvDataPreviewModel, SIGNAL(fetchingStarted()), ParsingFileState);
//   ParsingFileState->addTransition(pvDataPreviewModel, SIGNAL(fetchingDone()), IdleState);
//   
//   pvStateMachine.addState(IdleState);
//   pvStateMachine.addState(ParsingFileState);
//   pvStateMachine.setInitialState(IdleState);
//   pvStateMachine.start();
// }
