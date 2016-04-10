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
#include "mdt/csv/ParserFormatSetupProxyModel.h"
#include "mdt/csv/ParserFormatSetupDelegate.h"
#include "mdtCsvFileParserModel.h"
#include "mdtErrorDialog.h"
#include <QTableView>
#include <QScrollArea>

#include <QDebug>

mdtCsvFileParserSettingsDialog::mdtCsvFileParserSettingsDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  wFileSettings->setSelectFileMode(mdtCsvFileSettingsWidget::SelectOpen);
  // Setup data preview
  pvDataPreviewModel = new mdtCsvFileParserModel(tvDataPreview);
  auto *proxyModel = new mdt::csv::ParserFormatSetupProxyModel(tvDataPreview);
  proxyModel->setSourceModel(pvDataPreviewModel);
  tvDataPreview->setModel(proxyModel);
  auto *delegate = new mdt::csv::ParserFormatSetupDelegate(tvDataPreview);
  tvDataPreview->setItemDelegateForRow(0, delegate);
  connect(wFileSettings, &mdtCsvFileSettingsWidget::fileSettingsChanged, this, &mdtCsvFileParserSettingsDialog::onFileSettingsChanged);
  connect(wCsvSettings, &mdtCsvParserSettingsWidget::csvSettingsChanged, this, &mdtCsvFileParserSettingsDialog::onCsvSettingsChanged);
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
  resizeViewToContents();
}

void mdtCsvFileParserSettingsDialog::setControlsEnabled(bool enable)
{
  wFileSettings->setEnabled(enable);
  wCsvSettings->setEnabled(enable);
}

void mdtCsvFileParserSettingsDialog::resizeViewToContents()
{
  tvDataPreview->resizeColumnsToContents();
  tvDataPreview->resizeRowsToContents();
}
