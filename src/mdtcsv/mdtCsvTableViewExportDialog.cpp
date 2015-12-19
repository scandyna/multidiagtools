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
#include "mdtCsvTableViewExportDialog.h"
#include "mdtCsvTableViewDataMapper.h"
#include "mdtCsvFileGenerator.h"
#include "mdtErrorDialog.h"
#include "mdtCsvData.h"
#include <QPushButton>
#include <QMessageBox>
#include <QString>

#include <QDebug>

mdtCsvTableViewExportDialog::mdtCsvTableViewExportDialog(QWidget* parent)
 : QDialog(parent),
   pvMapper(new mdtCsvTableViewDataMapper)
{
  setupUi(this);
  connect(pbExport, &QPushButton::clicked, this, &mdtCsvTableViewExportDialog::exportToCsv);
}

// Let destructor implementation here, so shared_ptr know how to delete mapper and generator
mdtCsvTableViewExportDialog::~mdtCsvTableViewExportDialog()
{
}

void mdtCsvTableViewExportDialog::setView(QTableView* view)
{
  Q_ASSERT(view != nullptr);
  pvMapper->setView(view);
}

void mdtCsvTableViewExportDialog::exportToCsv()
{
  mdtCsvFileGenerator generator(wCsvSettings->getSettings());
  QString csvFilePath;

  // Open target file
  csvFilePath = wFileSettings->filePath();
  if(csvFilePath.isEmpty()){
    auto error = mdtErrorNewQ(tr("Please select a file."), mdtError::Warning, this);
    displayError(error);
    return;
  }
  if(!generator.openFile(csvFilePath, wFileSettings->fileEncoding())){
    displayError(generator.lastError());
    return;
  }
  // Proceed..
  pvMapper->reset();
  while(pvMapper->next()){
    auto record = pvMapper->getCurrentRecord();
    if(!generator.writeLine(record)){
      displayError(generator.lastError());
      return;
    }
  }
  generator.closeFile();
  // Inform user about success
  QMessageBox msgBox(this);
  msgBox.setText(tr("CSV export OK"));
  msgBox.exec();
}

void mdtCsvTableViewExportDialog::displayError(const mdtError& error)
{
  mdtErrorDialog dialog(error, this);
  dialog.setWindowTitle(tr("CSV export"));
  dialog.exec();
}
