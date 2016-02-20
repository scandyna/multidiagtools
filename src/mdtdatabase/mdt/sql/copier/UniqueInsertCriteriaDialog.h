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
#ifndef MDT_SQL_COPIER_UNIQUE_INSERT_CRITERIA_DIALOG_H
#define MDT_SQL_COPIER_UNIQUE_INSERT_CRITERIA_DIALOG_H

#include "ui_UniqueInsertCriteriaDialog.h"
#include "UniqueInsertCriteria.h"
#include "TableMapping.h"
#include "TableMappingItem.h"
#include <QDialog>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  class ExpressionMatchItemModel;

  /*! \brief Dialog to edit unique insert expression for SQL copier
   */
  class UniqueInsertCriteriaDialog : public QDialog, Ui::UniqueInsertCriteriaDialog
  {
   Q_OBJECT

   public:

    /*! \brief Construct dialog to edit unique insert expression (contained in table mapping)
     */
    UniqueInsertCriteriaDialog(const std::shared_ptr<const TableMapping> & tm, QWidget *parent = nullptr);

    /*! \brief Set criteria
     */
//     void setCriteria(const UniqueInsertCriteria & c);

    /*! \brief Get criteria
     */
    UniqueInsertCriteria criteria() const
    {
      return pvCriteria;
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

    /*! \brief Resize table view's contents
     */
    void resizeMatchItemViewToContents();

    std::shared_ptr<const TableMapping> pvTableMapping;
    UniqueInsertCriteria pvCriteria;
    ExpressionMatchItemModel *pvModel;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_UNIQUE_INSERT_CRITERIA_DIALOG_H
