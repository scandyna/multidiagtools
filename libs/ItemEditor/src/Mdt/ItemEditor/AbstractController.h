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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H

#include "RowState.h"
#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>

class QItemSelectionModel;

namespace Mdt{ namespace ItemEditor{

  class RowChangeEventMapper;
  class RowChangeEventDispatcher;
  class ItemSelectionModel;

  /*! \brief Common base for controllers
   */
  class AbstractController : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractController(QObject* parent = nullptr);

    // Copy disabled
    AbstractController(const AbstractController &) = delete;
    AbstractController & operator=(const AbstractController &) = delete;
    // Move disabled
    AbstractController(AbstractController &&) = delete;
    AbstractController & operator=(AbstractController &&) = delete;

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \note If subclass overloads this method,
     *        it must call this base setModel().
     * \pre model must be a valid pointer
     */
    virtual void setModel(QAbstractItemModel *model);

    /*! \brief Get model
     *
     * Returns the model set with setModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
    QAbstractItemModel *model() const
    {
      return pvModel;
    }

    /*! \brief Set selection model
     *
     * If a editor uses a view that handle
     *  selection with a QItemSelectionModel
     *  (like QTableView), replace its selection model
     *  with a ItemSelectionModel, and pass it to this controller
     *  with setSelectionModel().
     *  This way, the controller can block row change when needed.
     *
     * \pre model must be of type ItemSelectionModel
     */
    void setSelectionModel(QItemSelectionModel *model);

    /*! \brief Get row count
     *
     * If not model was set, 0 is returned.
     *
     * \note If this controller acts on a model
     *        that handles cache, this method
     *        will report row count in cache.
     *       See QAbstractItemModel documentation,
     *        at least canFetchMore() and fetchMore() ,
     *        for more details about cache.
     *
     * \todo Add remark on fetchAll() when..
     */
    int rowCount() const;

    /*! \brief Get current row
     *
     * If model was not set,
     *  or is empty, or for some other reasons
     *  (for example, a view has currently no current index),
     *  -1 is returned.
     */
    int currentRow() const
    {
      return pvCurrentRow;
    }

   public slots:

    /*! \brief Set current row
     *
     * \pre row must be >= -1
     */
    bool setCurrentRow(int row);

    /*! \brief Go to first row
     */
    void toFirst();

    /*! \brief Go to previous row
     */
    void toPrevious();

    /*! \brief Go to next row
     */
    void toNext();

    /*! \brief Go to last row
     */
    void toLast();

   signals:

    /*! \brief Emitted each time row count or current row changed
     *
     * This is used for components that need row count
     *  and current row to update their state.
     *  A example of such object is NavigationActions .
     */
    void rowStateChanged(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted each time a model was set
     */
    void modelChanged(QAbstractItemModel *model);

   protected:

   private slots:

    /*! \brief Set row state
     *
     * This slot is only called by RowChangeEventMapper,
     *  to tell is a model was change or populated,
     *  if a selection model was changed,
     *  or current row changed by selection model.
     *
     * It will also call setCurrentRow().
     */
//     void setRowState(Mdt::ItemEditor::RowState rs);

   private:

    int pvCurrentRow;
    QPointer<QAbstractItemModel> pvModel;
    RowChangeEventMapper *pvRowChangeEventMapper;
    RowChangeEventDispatcher *pvRowChangeEventDispatcher;
    QPointer<ItemSelectionModel> pvSelectionModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
