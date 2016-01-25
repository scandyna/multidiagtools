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
#ifndef MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_DIALOG_H
#define MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_DIALOG_H

#include "ui_UniqueInsertExpressionDialog.h"
#include "UniqueInsertExpression.h"
#include "TableMapping.h"
#include "TableMappingItem.h"
#include <QDialog>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

//   class UniqueInsertExpressionModel;
  class ExpressionMatchItemModel;

  /*! \brief Dialog to edit unique insert expression for SQL copier
   */
  class UniqueInsertExpressionDialog : public QDialog, Ui::UniqueInsertExpressionDialog
  {
   Q_OBJECT

   public:

    /*! \brief Construct dialog to edit expression at given itemIndex in table mapping
     */
    UniqueInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, int itemIndex, QWidget *parent = nullptr);

    /*! \brief Construct dialog to edit a new expression
     */
    UniqueInsertExpressionDialog(const std::shared_ptr<const TableMapping> & tm, QWidget *parent = nullptr);

    /*! \brief Get expression
     */
    UniqueInsertExpression expression() const
    {
      return pvExpression;
    }

    /*! \brief Do checks and accept if ok
     */
    void accept() override;

   private:

    /*! \brief Add a match item
     */
    void addMatchItem();

    /*! \brief Remove selected match items
     */
    void removeSelectedMatchItems();

    /*! \brief Clear destination key
     */
    void clearDestinationKey();

    /*! \brief Add a field to destination key
     */
    void addFieldToDestinationKey();

    /*! \brief Display destination key
     */
    void displayDestinationKey();

    /*! \brief Resize table view's contents
     */
    void resizeMatchItemViewToContents();

    ///const TableMapping & pvTableMapping;
    std::shared_ptr<const TableMapping> pvTableMapping;
    UniqueInsertExpression pvExpression;
    FieldIndexList pvInitialKey;
//     UniqueInsertExpressionModel *pvModel;
    ExpressionMatchItemModel *pvModel;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_UNIQUE_INSERT_EXPRESSION_DIALOG_H
