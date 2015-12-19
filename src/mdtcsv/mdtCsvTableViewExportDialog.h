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
#ifndef MDT_CSV_TABLE_VIEW_EXPORT_DIALOG_H
#define MDT_CSV_TABLE_VIEW_EXPORT_DIALOG_H

#include "ui_mdtCsvTableViewExportDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <memory>

class QTableView;
class mdtCsvTableViewDataMapper;

/*! \brief Dialog to export data comming from a QTableView to a CSV file
 *
 * This dialog can be used to export what the user see on a QTableView.
 */
class mdtCsvTableViewExportDialog : public QDialog, Ui::mdtCsvTableViewExportDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvTableViewExportDialog(QWidget *parent = nullptr);

  /*! \brief Destructor
   */
  ~mdtCsvTableViewExportDialog();

  mdtCsvTableViewExportDialog(const mdtCsvTableViewExportDialog &) = delete;
  mdtCsvTableViewExportDialog & operator=(const mdtCsvTableViewExportDialog &) = delete;

  /*! \brief Set table view
   *
   * \pre view must not be a null pointer
   * \pre view must have a model
   * \pre view must have horizontal and vertical header views
   */
  void setView(QTableView *view);

 private:

  /*! \brief Export
   */
  void exportToCsv();

  /*! \brief Display error
   */
  void displayError(const mdtError & error);

  std::unique_ptr<mdtCsvTableViewDataMapper> pvMapper;
};

#endif // #ifndef MDT_CSV_TABLE_VIEW_EXPORT_DIALOG_H
