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
#include "mdtSqlCopierCodecSettingsWidget.h"
#include "mdtSqlDatabaseDialogSqlite.h"
#include <QComboBox>
#include <QSqlDatabase>
#include <QFileInfo>
#include <QDir>

mdtSqlCopierCodecSettingsWidget::mdtSqlCopierCodecSettingsWidget(QWidget* parent)
 : QWidget(parent)
{
  setupUi(this);
  // Setup and populate type combobox
  cbType->addItem(tr("SQLite database table"), mdtSqlCopierCodecSettings::SqliteCodec);
  cbType->addItem(tr("CSV file"), mdtSqlCopierCodecSettings::CsvCodec);
  cbType->addItem(tr("Clipboard"), mdtSqlCopierCodecSettings::ClipboardCodec);
  connect(cbType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &mdtSqlCopierCodecSettingsWidget::updateCodecType);
  cbType->setCurrentIndex(-1);  // Without passing by index -1, setCurrentIndex(0) will not emit currentIndexChanged() signal
  cbType->setCurrentIndex(0);
  // Setup tbSelect
  connect(tbSelect, &QToolButton::clicked, this, &mdtSqlCopierCodecSettingsWidget::selectTarget);
}

void mdtSqlCopierCodecSettingsWidget::updateCodecType(int cbTypeIndex)
{
  int codecType = static_cast<mdtSqlCopierCodecSettings::CodecType>(cbType->itemData(cbTypeIndex).toInt());

  switch(codecType){
    case mdtSqlCopierCodecSettings::SqliteCodec:
      setCodecTypeSqlite();
      break;
    case mdtSqlCopierCodecSettings::ClipboardCodec:
    case mdtSqlCopierCodecSettings::CsvCodec:
    case mdtSqlCopierCodecSettings::UnknownCodec:
      setCodecTypeUnknown();
      break;
  }
}

void mdtSqlCopierCodecSettingsWidget::selectTarget()
{
  Q_ASSERT(pvSettings);

  switch(pvSettings->type()){
    case mdtSqlCopierCodecSettings::SqliteCodec:
      selectSqliteDatabase();
      break;
    case mdtSqlCopierCodecSettings::ClipboardCodec:
    case mdtSqlCopierCodecSettings::CsvCodec:
    case mdtSqlCopierCodecSettings::UnknownCodec:
      break;
  }
}

void mdtSqlCopierCodecSettingsWidget::setCodecTypeUnknown()
{
  // Update widgets data
  pvSettings.reset();
  updateWidgetsData();
  // Update widgets visibility/enable state
  tbSelect->setEnabled(false);
  lbConnectionNameLabel->setVisible(false);
  lbConnection->setVisible(false);
  lbFileLabel->setVisible(false);
  lbFile->setVisible(false);
  lbDirectoryLabel->setVisible(false);
  lbDirectory->setVisible(false);
  lbTable->setVisible(false);
  cbTable->setVisible(false);
}

void mdtSqlCopierCodecSettingsWidget::setCodecTypeSqlite()
{
  // Update widgets data
  pvSettings.reset(new mdtSqlCopierSqliteDatabaseTableCodecSettings);
  updateWidgetsData();
  // Update widgets visibility
  tbSelect->setEnabled(true);
  lbConnectionNameLabel->setVisible(true);
  lbConnection->setVisible(true);
  lbFileLabel->setVisible(true);
  lbFile->setVisible(true);
  lbDirectoryLabel->setVisible(true);
  lbDirectory->setVisible(true);
  lbTable->setVisible(true);
  cbTable->setVisible(true);
}

void mdtSqlCopierCodecSettingsWidget::updateWidgetsData()
{
  if(pvSettings){
    lbConnection->setText(pvSettings->connectionName());
    QFileInfo fileInfo(pvSettings->filePath());
    if(fileInfo.fileName().isEmpty()){
      lbFile->clear();
      lbDirectory->clear();
    }else{
      lbFile->setText(fileInfo.fileName());
      lbDirectory->setText(fileInfo.absoluteDir().path());
    }
  }else{
    lbConnection->clear();
    lbFile->clear();
    lbDirectory->clear();
    cbTable->clear();
  }
}

void mdtSqlCopierCodecSettingsWidget::selectSqliteDatabase()
{
  Q_ASSERT(pvSettings);

  mdtSqlDatabaseDialogSqlite dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  QSqlDatabase db = dialog.database().database();
  pvSettings->setConnectionName(db.connectionName());
  pvSettings->setFilePath(db.databaseName());
  cbTable->clear();
  cbTable->addItems(db.tables(QSql::Tables));
  updateWidgetsData();
}
