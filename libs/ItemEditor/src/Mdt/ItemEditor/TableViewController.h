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
#ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_H
#define MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_H

#include "AbstractController.h"
#include <memory>

class QAbstractItemModel;
class QTableView;

namespace Mdt{ namespace ItemEditor{

  class TableViewControllerImplementation;

  /*! \brief TableViewController acts on a QAbstractTableModel and a QTableView
   *
   * #### Editing
   *
   * When a editor is created at some index, the controller will turn to editing item state.
   *  Whenn edition is done, the item delegate set data to the model and closes the editor,
   *  also turning this controller to editing state.
   */
  class TableViewController : public AbstractController
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit TableViewController(QObject* parent = nullptr);

    // Destructor - unique_ptr needs define of ItemViewPrivateContainer to delet it
    ~TableViewController();

    // Copy disabled
    TableViewController(const TableViewController &) = delete;
    TableViewController & operator=(const TableViewController &) = delete;
    // Move disabled
    TableViewController(TableViewController &&) = delete;
    TableViewController & operator=(TableViewController &&) = delete;

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

  public slots:

    /*! \brief Set primary key hidden
     *
     * If \a hide is true, columns that are part of primary key will be hidden.
     */
    void setPrimaryKeyHidden(bool hide);

    /*! \brief Set foreign key hidden
     *
     * If \a hide is true,
     *  columns that are part of the foreign key referencing to the entity named \a foreignEntityName,
     *  will be hidden.
     *
     * \pre \a foreignEntityName must not be empty
     * \pre A foreign key referencing \a foreignEntityName must exist
     */
    void setForeignKeyHidden(const QString & foreignEntityName, bool hide);

    /*! \brief Set all foreign keys hidden
     *
     * If \a hide is true,
     *  all columns that are part of any foreign key
     *  will be hidden.
     */
    void setAllForeignKeysHidden(bool hide);

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

    void primaryKeyChangedEvent(const ItemModel::PrimaryKey& oldPrimaryKey, const ItemModel::PrimaryKey& newPrimaryKey) override;
    void foreignKeysChangedEvent(const ItemModel::ForeignKeyList& newForeignKeys) override;

    /*! \brief Get a list of currently selected rows
     */
    Mdt::ItemModel::RowList getSelectedRows() const override;

    std::unique_ptr<TableViewControllerImplementation> mImpl;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_H
