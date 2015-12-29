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
#include "mdtCsvFileGeneratorSettingsDialog.h"
#include "mdtCsvGeneratorSettingsWidget.h"
#include <QAction>
#include <QIcon>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QStringList>
#include <QTextCodec>

mdtCsvFileGeneratorSettingsDialog::mdtCsvFileGeneratorSettingsDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  wFileSettings->setSelectFileMode(mdtCsvFileSettingsWidget::SelectSave);
}

void mdtCsvFileGeneratorSettingsDialog::setFileSettings(const QString & path, const QByteArray & encoding)
{
  wFileSettings->setFileSettings(path, encoding);
}

QString mdtCsvFileGeneratorSettingsDialog::filePath() const
{
  return wFileSettings->filePath();
//   return leFilePath->text();
}

QByteArray mdtCsvFileGeneratorSettingsDialog::fileEncoding() const
{
  return wFileSettings->fileEncoding();
//   return cbFileEncoding->currentText().toLatin1();
}

void mdtCsvFileGeneratorSettingsDialog::setCsvSettings(const mdtCsvGeneratorSettings& settings)
{
  wCsvSettings->setSettings(settings);
}

mdtCsvGeneratorSettings mdtCsvFileGeneratorSettingsDialog::getCsvSettings() const
{
  return wCsvSettings->getSettings();
}
