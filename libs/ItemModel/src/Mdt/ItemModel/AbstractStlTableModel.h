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
#ifndef MDT_ITEM_MODEL_ABSTRACT_STL_TABLE_MODEL_H
#define MDT_ITEM_MODEL_ABSTRACT_STL_TABLE_MODEL_H

#include "MdtItemModelExport.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVector>

namespace Mdt{ namespace ItemModel{

  /*! \brief Abstract base for STL compliant container list model
   */
  class MDT_ITEMMODEL_EXPORT AbstractStlTableModel : public QAbstractTableModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    AbstractStlTableModel(QObject *parent = nullptr);

   protected slots:

    /*! \brief Emit dataChanged() signal
     */
    void emitDataChangedSignal(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles = QVector<int> ());

    /*! \brief Emit headerDataChanged(Qt::Vertical, \a firstRow, \a lastRow)
     */
    void emitVerticalHeaderDataChanged(int firstRow, int lastRow);

  };
}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_ABSTRACT_STL_TABLE_MODEL_H
