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
#ifndef MDT_ITEM_EDITOR_SQL_TABLE_VIEW_CONTROLLER_H
#define MDT_ITEM_EDITOR_SQL_TABLE_VIEW_CONTROLLER_H

#include "AbstractSqlTableModelController.h"
#include <memory>

// class QSqlTableModel;
class QTableView;

namespace Mdt{ namespace ItemEditor{

  class TableViewControllerImplementation;
  
  class ItemViewPrivateContainer;

  /*! \brief SqlTableViewController acts on a QSqlTableModel
   */
  class SqlTableViewController : public AbstractSqlTableModelController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SqlTableViewController(QObject* parent = nullptr);

    /*! \brief Construct a controller that acts on \a db
     */
//     explicit SqlTableViewController(QObject *parent, const QSqlDatabase & db);

    /*! \brief Construct a controller that acts on \a db
     */
//     explicit SqlTableViewController(const QSqlDatabase & db);

    // Destructor - unique_ptr needs define of ItemViewPrivateContainer to delet it
    ~SqlTableViewController();

    // Copy disabled
    SqlTableViewController(const SqlTableViewController &) = delete;
    SqlTableViewController & operator=(const SqlTableViewController &) = delete;
    // Move disabled
    SqlTableViewController(SqlTableViewController &&) = delete;
    SqlTableViewController & operator=(SqlTableViewController &&) = delete;

    /*! \brief Attach view to this controller
     *
     * Once view is attached, this controller
     *  take control of it.
     *
     * \note A controller will only control 1 view.
     *       Also, controller does not take ownership
     *       of the view (it will not delete it).
     * \pre view must be a valid pointer.
     */
    void setView(QTableView *view);

    /*! \brief Get view attached to this controller
     *
     * Will also return a nullptr if no view was set
     */
    QTableView *view() const;

    /*! \brief Allow the selection of multiple rows
     *
     * If multiple rows selection is allowed, the user can select other rows than current one,
     *  otherwise only items in current row can be selected.
     *
     * By default, selection of multiple rows is not allowed.
     */
    void setMultiRowSelectionAllowed(bool allow);

   private:

    /*! \brief Set model to the view
     */
    void setModelToView(QAbstractItemModel *model) override;

    /*! \brief Submit data to model
     */
    bool setDataToModel() override;

    /*! \brief Revert data from model
     */
    void revertDataFromModel() override;

    /*! \brief Get a list of currently selected rows
     */
    Mdt::ItemModel::RowList getSelectedRows() const override;

//     void registerModelAndSelectionModel();

    std::unique_ptr<TableViewControllerImplementation> mImpl;
//     std::unique_ptr<ItemViewPrivateContainer> mContainer;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_SQL_TABLE_VIEW_CONTROLLER_H
