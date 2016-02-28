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
#ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_TABLE_MAPPING_DIALOG_H
#define MDT_SQL_COPIER_CSV_FILE_IMPORT_TABLE_MAPPING_DIALOG_H

#include "TableMappingDialog.h"
#include "CsvFileImportTableMapping.h"
#include <QSqlDatabase>

class QLabel;
class QToolButton;

namespace mdt{ namespace sql{ namespace copier{

  class CsvFileImportTableMappingModel;

  /*! \brief SQL database copier table mapping dialog
   */
  class CsvFileImportTableMappingDialog : public TableMappingDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    CsvFileImportTableMappingDialog(QWidget *parent = nullptr);

   private slots:

    /*! \brief Setup source CSV file
     */
    void setupSourceFile();

   private:

    /*! \brief Reference internal table mapping (read only version)
     */
    const std::shared_ptr<const TableMapping> mappingBase() const override;

    /*! \brief Reference internal table mapping
     */
    const std::shared_ptr<TableMapping> mappingBase() override;

    /*! \brief Get table mapping model base
     */
    TableMappingModel *mappingModelBase() override;

    CsvFileImportTableMappingModel *pvMappingModel;
    QLabel *lbSourceTable;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_CSV_FILE_IMPORT_TABLE_MAPPING_DIALOG_H
