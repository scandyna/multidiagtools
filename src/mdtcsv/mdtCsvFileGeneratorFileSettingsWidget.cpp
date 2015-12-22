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
#include "mdtCsvFileGeneratorFileSettingsWidget.h"
#include <QAction>
#include <QIcon>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QStringList>
#include <QTextCodec>

mdtCsvFileGeneratorFileSettingsWidget::mdtCsvFileGeneratorFileSettingsWidget(QWidget* parent)
 : QWidget(parent)
{
  setupUi(this);
  // Create select file action
  QAction *selectFileAction = new QAction(this);
  selectFileAction->setIcon(QIcon::fromTheme("document-open"));
  leFilePath->addAction(selectFileAction, QLineEdit::TrailingPosition);
  connect(selectFileAction, &QAction::triggered, this, &mdtCsvFileGeneratorFileSettingsWidget::selectFile);
  // Fill encoding dialog
  auto baCodecList = QTextCodec::availableCodecs();
  QStringList codecList;
  for(const auto & bac : baCodecList){
    const QString sc = bac;
    if(!codecList.contains(sc)){
      codecList.append(sc);
    }
  }
  codecList.sort();
  cbFileEncoding->addItems(codecList);
  cbFileEncoding->setCurrentText("UTF-8");
}

void mdtCsvFileGeneratorFileSettingsWidget::setFileSettings(const QString & path, const QByteArray & encoding)
{
  leFilePath->setText(path);
  const QString enc = QString::fromLatin1(encoding);
  cbFileEncoding->setCurrentText(enc);
}

QString mdtCsvFileGeneratorFileSettingsWidget::filePath() const
{
  return leFilePath->text();
}

QByteArray mdtCsvFileGeneratorFileSettingsWidget::fileEncoding() const
{
  return cbFileEncoding->currentText().toLatin1();
}

void mdtCsvFileGeneratorFileSettingsWidget::selectFile()
{
  QFileDialog dialog(this);
  QFileInfo fileInfo;

  // Setup and show dialog
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if(!pvDefaultDirectory.isEmpty()){
    dialog.setDirectory(pvDefaultDirectory);
  }
  dialog.setNameFilter(tr("CSV file (*.csv)"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("csv");
  dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(dialog.selectedFiles().size() == 1);
  fileInfo.setFile(dialog.selectedFiles().at(0));
  // Store file path
  leFilePath->setText(fileInfo.absoluteFilePath());
}
