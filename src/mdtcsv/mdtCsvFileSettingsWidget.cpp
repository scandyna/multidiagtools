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
#include "mdtCsvFileSettingsWidget.h"
#include <QAction>
#include <QIcon>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QStringList>
#include <QTextCodec>
#include <QComboBox>
#include <QToolButton>

mdtCsvFileSettingsWidget::mdtCsvFileSettingsWidget(QWidget* parent)
 : QWidget(parent),
   pvSelectFileMode(SelectOpen)
{
  setupUi(this);
  // Create select file action
  QAction *selectFileAction = new QAction(this);
  selectFileAction->setIcon(QIcon::fromTheme("document-open"));
  leFilePath->addAction(selectFileAction, QLineEdit::TrailingPosition);
  connect(selectFileAction, &QAction::triggered, this, &mdtCsvFileSettingsWidget::selectFile);
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
  selectUtf8Encoding();
  connect(cbFileEncoding, &QComboBox::currentTextChanged, this, &mdtCsvFileSettingsWidget::onFileEncodingChanged);
  connect(tbSelectUtf8Encoding, &QToolButton::clicked, this, &mdtCsvFileSettingsWidget::selectUtf8Encoding);
  connect(tbSelectSystemEncoding, &QToolButton::clicked, this, &mdtCsvFileSettingsWidget::selectSystemEncoding);
}

void mdtCsvFileSettingsWidget::setFileSettings(const QString & path, const QByteArray & encoding)
{
  leFilePath->setText(path);
  const QString enc = QString::fromLatin1(encoding);
  cbFileEncoding->setCurrentText(enc);
  emit fileSettingsChanged(path, encoding);
}

QString mdtCsvFileSettingsWidget::filePath() const
{
  return leFilePath->text();
}

QByteArray mdtCsvFileSettingsWidget::fileEncoding() const
{
  return cbFileEncoding->currentText().toLatin1();
}

void mdtCsvFileSettingsWidget::setSelectFileMode(mdtCsvFileSettingsWidget::SelectFileMode mode)
{
  pvSelectFileMode = mode;
}

void mdtCsvFileSettingsWidget::selectUtf8Encoding()
{
  cbFileEncoding->setCurrentText("UTF-8");
}

void mdtCsvFileSettingsWidget::selectSystemEncoding()
{
  auto * codec = QTextCodec::codecForLocale();

  if(codec == nullptr){
    return;
  }
  cbFileEncoding->setCurrentText(QString::fromLatin1(codec->name()));
}

void mdtCsvFileSettingsWidget::selectFile()
{
  QFileDialog dialog(this);
  QFileInfo fileInfo;
  QStringList filters;

  // Setup and show dialog
  switch(pvSelectFileMode){
    case SelectOpen:
      dialog.setAcceptMode(QFileDialog::AcceptOpen);
      dialog.setFileMode(QFileDialog::ExistingFile);
      break;
    case SelectSave:
      dialog.setAcceptMode(QFileDialog::AcceptSave);
      dialog.setFileMode(QFileDialog::AnyFile);
      break;
  }
  if(!pvDefaultDirectory.isEmpty()){
    dialog.setDirectory(pvDefaultDirectory);
  }
  filters << tr("CSV file (*.csv)") << tr("Any file (*)");
  dialog.setNameFilters(filters);
//   dialog.setNameFilter(tr("CSV file (*.csv), Any file (*)"));
  dialog.setDefaultSuffix("csv");
  dialog.setOption(QFileDialog::DontConfirmOverwrite, false);
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(dialog.selectedFiles().size() == 1);
  fileInfo.setFile(dialog.selectedFiles().at(0));
  // Store file path
  leFilePath->setText(fileInfo.absoluteFilePath());
  emit fileSettingsChanged(fileInfo.absoluteFilePath(), cbFileEncoding->currentText().toLatin1());
}

void mdtCsvFileSettingsWidget::onFileEncodingChanged(const QString & text)
{
//   if(text.isEmpty()){
//     return;
//   }
//   if(leFilePath->text().isEmpty()){
//     return;
//   }
  emit fileSettingsChanged(leFilePath->text(), text.toLatin1());
}
