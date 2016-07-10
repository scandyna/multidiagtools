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

namespace Mdt{ namespace ItemEditor{

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

    /*! \brief Set row state
     */
    void setRowState(RowState rs);

   signals:

    /*! \brief Emitted each time row count or current row changed
     */
    void rowStateChanged(RowState rs);

    /*! \brief Emitted each time a model was set
     */
    void modelChanged(QAbstractItemModel *model);

   protected:

   private:

    int pvCurrentRow;
    QPointer<QAbstractItemModel> pvModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
