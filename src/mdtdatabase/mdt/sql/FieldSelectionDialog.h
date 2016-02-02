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
#ifndef MDT_SQL_FIELD_SELECTION_DIALOG_H
#define MDT_SQL_FIELD_SELECTION_DIALOG_H

#include "ui_FieldSelectionDialog.h"
#include "FieldIndexList.h"
#include "FieldSelectionMode.h"
#include "mdtExpected.h"
#include <QDialog>
#include <QString>

class mdtSqlTableSchemaModel;
class QSqlDatabase;

namespace mdt{ namespace sql{

  class FieldSelectionProxyModel;

  /*! \brief Dialog to select field of a database table
   */
  class FieldSelectionDialog : public QDialog, Ui::FieldSelectionDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    FieldSelectionDialog(QWidget* parent = nullptr);

    /*! \brief Set message
     */
    void setMessage(const QString & msg);

    /*! \brief Set table
     *
     * \pre db must be valid (a driver must be loaded, see QSqlDatabase::isValid() documentation for details)
     * \pre db must be open
     */
    mdtExpected<bool> setTable(const QString & tableName, const QSqlDatabase & db);

    /*! \brief Sort fields
     */
    void sort(Qt::SortOrder sortOrder = Qt::AscendingOrder);

    /*! \brief Set list of field indexes to hide
     */
    void setFieldIndexesToHide(const FieldIndexList & indexes);

    /*! \brief Show all field indexes
     */
    void showAllFieldIndexes();

    /*! \brief Set field selection mode
     *
     * Will also clear current field selection.
     */
    void setFieldSelectionMode(FieldSelectionMode mode);

    /*! \brief Get list of selected field indexes
     */
    FieldIndexList getSelectedFieldIndexList() const;

    /*! \brief Get a list of selected field names
     */
    QStringList getSelectedFieldNameList() const;

   private:

    /*! \brief Resize table view to contents
     */
    void resizeTvToContents();

    mdtSqlTableSchemaModel *pvSchemaModel;
    FieldSelectionProxyModel *pvProxyModel;
  };

}} // namespace mdt{ namespace sql{

#endif // #ifndef MDT_SQL_FIELD_SELECTION_DIALOG_H

