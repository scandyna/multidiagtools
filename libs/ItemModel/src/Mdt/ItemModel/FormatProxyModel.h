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

#include "FormatMap.h"
#include "MdtItemModelExport.h"
#include <QIdentityProxyModel>
#include <QVariant>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <Qt>

namespace Mdt{ namespace ItemModel{

  /*! \brief Proxy model to provide some formatting
   */
  class MDT_ITEMMODEL_EXPORT FormatProxyModel : public QIdentityProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit FormatProxyModel(QObject *parent = nullptr);

    /*! \brief Set priority when format conflics
     *
     * Define which format should be choosen when
     *  many where set for a specific index.
     *
     * For example:
     * \code
     * setPriority({FormatMapPriority::Row, FormatMapPriority::Column, FormatMapPriority::Index});
     * \endcode
     *
     * Default is index, then row, then column.
     */
    void setPriority(const std::array<FormatMapPriority, 3> & priority);

    /*! \brief Set text alignment for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void setTextAlignmentForIndex(int row, int column, Qt::Alignment alignment);

    /*! \brief Clear text alignment for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void clearTextAlignmentForIndex(int row, int column);

    /*! \brief Set text alignment for row
     *
     * \pre \a row must be >= 0
     */
    void setTextAlignmentForRow(int row, Qt::Alignment alignment);

    /*! \brief Clear text alignment for row
     *
     * \pre \a row must be >= 0
     */
    void clearTextAlignmentForRow(int row);

    /*! \brief Set text alignment for column
     *
     * \pre \a column must be >= 0
     */
    void setTextAlignmentForColumn(int column, Qt::Alignment alignment);

    /*! \brief Clear text alignment for column
     *
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    void clearTextAlignmentForColumn(int column);

    /*! \brief Get text alignment for given row and column
     *
     * Returns a QVariant with value of type Qt::Alignment
     *  if a alignment was set for \a row and \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant textAlignment(int row, int column) const;

    /*! \brief Set text font for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void setTextFontForIndex(int row, int column, const QFont & font);

    /*! \brief Clear text font for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void clearTextFontForIndex(int row, int column);

    /*! \brief Set text font for row
     *
     * \pre \a row must be >= 0
     */
    void setTextFontForRow(int row, const QFont & font);

    /*! \brief Clear text font for row
     *
     * \pre \a row must be >= 0
     */
    void clearTextFontForRow(int row);

    /*! \brief Set text font for column
     *
     * \pre \a column must be >= 0
     */
    void setTextFontForColumn(int column, const QFont & font);

    /*! \brief Clear text font for column
     *
     * \pre \a column must be >= 0
     */
    void clearTextFontForColumn(int column);

    /*! \brief Get text font for given row and column
     *
     * Returns a QVariant with value of type QFont
     *  if a font was set for \a row and \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant textFont(int row, int column) const;

    /*! \brief Set text color for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void setTextColorForIndex(int row, int column, const QColor & color);

    /*! \brief Clear text color for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void clearTextColorForIndex(int row, int column);

    /*! \brief Set text color for row
     *
     * \pre \a row must be >= 0
     */
    void setTextColorForRow(int row, const QColor & color);

    /*! \brief Clear text color for row
     *
     * \pre \a row must be >= 0
     */
    void clearTextColorForRow(int row);

    /*! \brief Set text color for column
     *
     * \pre \a column must be >= 0
     */
    void setTextColorForColumn(int column, const QColor & color);

    /*! \brief Clear text color for column
     *
     * \pre \a column must be >= 0
     */
    void clearTextColorForColumn(int column);

    /*! \brief Get foreground brush for given row and column
     *
     * Returns a QVariant with value of type QBrush
     *  if a text color was set for \a row and \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant foregroundBrush(int row, int column) const;

    /*! \brief Set background brush for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void setBackgroundBrushForIndex(int row, int column, const QBrush & brush);

    /*! \brief Set background color for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void setBackgroundColorForIndex(int row, int column, const QColor & color, Qt::BrushStyle style = Qt::SolidPattern);

    /*! \brief Clear background brush for row and column
     *
     * \pre \a row must be >= 0
     * \pre \a column must be >= 0
     */
    void clearBackgroundBrushForIndex(int row, int column);

    /*! \brief Set background brush for row
     *
     * \pre \a row must be >= 0
     */
    void setBackgroundBrushForRow(int row, const QBrush & brush);

    /*! \brief Set background color for row
     *
     * \pre \a row must be >= 0
     */
    void setBackgroundColorForRow(int row, const QColor & color, Qt::BrushStyle style = Qt::SolidPattern);

    /*! \brief Clear background brush for row
     *
     * \pre \a row must be >= 0
     */
    void clearBackgroundBrushForRow(int row);

    /*! \brief Set background brush for column
     *
     * \pre \a column must be >= 0
     */
    void setBackgroundBrushForColumn(int column, const QBrush & brush);

    /*! \brief Set background color for column
     *
     * \pre \a column must be >= 0
     */
    void setBackgroundColorForColumn(int column, const QColor & color, Qt::BrushStyle style = Qt::SolidPattern);

    /*! \brief Clear background brush for column
     *
     * \pre \a column must be >= 0
     */
    void clearBackgroundBrushForColumn(int column);

    /*! \brief Get background brush for given row and column
     *
     * Returns a QVariant with value of type QBrush
     *  if a background brush was set for \a row and \a column,
     *  otherwise a null QVariant.
     *
     * \pre \a row must be in valid range ( 0 <= row < rowCount() )
     * \pre \a column must be in valid range ( 0 <= column < columnCount() )
     */
    QVariant backgroundBrush(int row, int column) const;

    /*! \brief Get data for given index and role
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

   private:

    void signalFormatChangedForIndex(int row, int column, int role);
    void signalFormatChangedForRow(int row, int role);
    void signalFormatChangedForColumn(int column, int role);

    FormatMap mTextAlignmentMap;
    FormatMap mTextFontMap;
    FormatMap mForegroundBrushMap;
    FormatMap mBackgroundBrushMap;
  };

}} // namespace Mdt{ namespace ItemModel{

#endif // #ifndef MDT_ITEM_MODEL_FORMAT_PROXY_MODEL_H
