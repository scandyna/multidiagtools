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
#ifndef MDT_SQL_COPIER_EXPRESSION_KEY_FIELD_SELECTION_DIALOG_H
#define MDT_SQL_COPIER_EXPRESSION_KEY_FIELD_SELECTION_DIALOG_H

#include "ui_ExpressionKeyFieldSelectionDialog.h"
#include "FieldIndexList.h"
#include <QDialog>
#include <QStringList>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  class TableMapping;

  /*! \brief Dialog to select a field to add to a key of a expression
   */
  class ExpressionKeyFieldSelectionDialog : public QDialog, Ui::ExpressionKeyFieldSelectionDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    ExpressionKeyFieldSelectionDialog(const std::shared_ptr<const TableMapping> & tm, QWidget *parent = nullptr);

    /*! \brief Set key of expression
     *
     * Will build the list of unused destination fields
     *  (only regarding fields used in key,
     *   not table mapping wide)
     */
    void setKey(const FieldIndexList & key);

    /*! \brief Set initial key of expression
     *
     * This key will be excluded from warnings
     *  about using allready mapped destination fields
     */
    void setInitialKey(const FieldIndexList & key);

    /*! \brief Make checks and accept if ok
     */
    void accept() override;

    /*! \brief Get selected field index
     */
    int selectedFieldIndex() const;

   private:

    /*! \brief Get list of fields that are mapped and will be removed from table mapping
     */
    QStringList getMappedToRemoveFieldList() const;

    FieldIndexList pvKey;
    FieldIndexList pvInitialKey;
    std::shared_ptr<const TableMapping> pvTableMapping;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{


#endif // #ifndef MDT_SQL_COPIER_EXPRESSION_KEY_FIELD_SELECTION_DIALOG_H
