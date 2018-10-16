/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ITEM_MODEL_SELECTION_MODEL_CURRENT_ROW_CHANGED_H
#define MDT_ITEM_MODEL_SELECTION_MODEL_CURRENT_ROW_CHANGED_H

#include "MdtItemModelExport.h"
#include <QObject>
#include <QMetaObject>
#include <QPointer>
#include <QModelIndex>

class QItemSelectionModel;
class QAbstractItemModel;

namespace Mdt{ namespace ItemModel{

  /*! \brief Helper class to catch row changed from a QItemSelectionModel
   *
   * \code
   * using Mdt::ItemModel::SelectionModelCurrentRowChanged;
   *
   * MyModel model;
   * QTableView view;
   * view.setModel(&model);
   *
   * SelectionModelCurrentRowChanged currentRowChanged;
   * currentRowChanged.setSelectionModel(view.selectionModel());
   *
   * MyBusinessObject bo;
   * connect( &currentRowChanged, &SelectionModelCurrentRowChanged::currentRowChanged, &bo, &MyBusinessObject::setCurrentRow );
   * \endcode
   *
   * \note When a model is set to a view, its selection model is replaced.
   *    SelectionModelCurrentRowChanged has no way to detect this,
   *    so the selection model has to be set again using setSelectionModel().
   *    See QAbstractItemView::setModel() for more informations about this behaviour.
   */
  class MDT_ITEMMODEL_EXPORT SelectionModelCurrentRowChanged : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit SelectionModelCurrentRowChanged(QObject *parent = nullptr);

    /*! \brief Set selection model
     *
     * Will also emit currentRowChanged()
     *  with the row corresponding to the current index of \a selectionModel ,
     *  or -1 if it has no current index.
     *
     * \pre \a selectionModel must be a valid pointer
     */
    void setSelectionModel(QItemSelectionModel *selectionModel);

   signals:

    /*! \brief Emitted when current row changed
     */
    void currentRowChanged(int newRow);

   private slots:

    void setCurrentRow(const QModelIndex & current, const QModelIndex & previous);

   private:

    QMetaObject::Connection mCurrentRowChangedConnection;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_SELECTION_MODEL_CURRENT_ROW_CHANGED_H
