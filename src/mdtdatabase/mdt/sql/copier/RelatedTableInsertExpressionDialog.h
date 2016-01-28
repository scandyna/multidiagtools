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
#ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_DIALOG_H
#define MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_DIALOG_H

#include "ui_RelatedTableInsertExpressionDialog.h"
#include "RelatedTableInsertExpression.h"
#include <QDialog>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  class TableMapping;
  class RelatedTableInsertMatchItemModel;

  /*! \brief Dialog to edit related table insert expression for SQL copier
   */
  class RelatedTableInsertExpressionDialog : public QDialog, Ui::RelatedTableInsertExpressionDialog
  {
   Q_OBJECT

   public:

    /*! \brief Construct dialog to edit expression at given itemIndex in table mapping
     */
    RelatedTableInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, int itemIndex, QWidget *parent = nullptr);

    /*! \brief Construct dialog to edit a new expression
     */
    RelatedTableInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, QWidget *parent = nullptr);

   private:

    /*! \brief Add a destination field
     */
    void addDestinationField();

    /*! \brief Clear destination fields
     */
    void clearDestinationFields();

    /*! \brief Display destination fields
     */
    void displayDestinationFields();

    /*! \brief Populate value source combobox
     */
    void populateValueSourceCombobox();

    std::shared_ptr<const TableMapping> pvTableMapping;
    RelatedTableInsertExpression pvExpression;
    RelatedTableInsertMatchItemModel *pvMatchItemModel;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_RELATED_TABLE_INSERT_EXPRESSION_DIALOG_H
