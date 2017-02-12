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
#ifndef MDT_ITEM_MODEL_FORMAT_PROXY_MODEL_H
#define MDT_ITEM_MODEL_FORMAT_PROXY_MODEL_H

#include "ColumnFormatMap.h"
#include <QIdentityProxyModel>
#include <QVariant>
#include <QFont>
#include <QBrush>
#include <Qt>

namespace Mdt{ namespace ItemModel{

  /*! \brief Proxy model to provide some formatting
   */
  class FormatProxyModel : public QIdentityProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit FormatProxyModel(QObject *parent = nullptr);

    /*! \brief Set text alignment for column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void setTextAlignmentForColumn(int column, Qt::Alignment alignment);

    /*! \brief Clear text alignment for column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void clearTextAlignmentForColumn(int column);

    /*! \brief Get text alignment for given column
     *
     * Returns a QVariant with value of type Qt::Alignment
     *  if a alignment was set for \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant textAlignmentForColumn(int column) const;

    /*! \brief Set text font for column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void setTextFontForColumn(int column, const QFont & font);

    /*! \brief Clear text font for column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void clearTextFontForColumn(int column);

    /*! \brief Get text font for given column
     *
     * Returns a QVariant with value of type QFont
     *  if a font was set for \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant textFontForColumn(int column) const;

    /*! \brief Get data for given index and role
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

   private:

    ColumnFormatMap mTextAlignmentMap;
    ColumnFormatMap mTextFontMap;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FORMAT_PROXY_MODEL_H
