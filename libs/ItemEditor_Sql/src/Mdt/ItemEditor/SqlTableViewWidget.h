/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_SQL_TABLE_VIEW_WIDGET_H
#define MDT_ITEM_EDITOR_SQL_TABLE_VIEW_WIDGET_H

#include "Mdt/ItemEditor/AbstractTableViewWidget.h"
#include "SqlTableViewController.h"
#include <QSqlDatabase>

class QSqlTableModel;
//class QSqlDatabase;

namespace Mdt{ namespace ItemEditor{

  /*! \brief QTableView based SQL editor
   */
  class SqlTableViewWidget : public AbstractTableViewWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SqlTableViewWidget(QWidget* parent, const QSqlDatabase & db);

    /*! \brief Constructor
     */
    explicit SqlTableViewWidget(const QSqlDatabase & db);

    /*! \brief Get controller
     */
    SqlTableViewController *controller() const
    {
      return mController;
    }

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the editor does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
    void setModel(QSqlTableModel *model);

   private:

    /*! \brief Get controller
     */
    AbstractController *abstractController() const override
    {
      return mController;
    }

    SqlTableViewController *mController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_SQL_TABLE_VIEW_WIDGET_H
