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
#ifndef MDT_ITEM_EDITOR_WIDGET_STYLE_SHEET_H
#define MDT_ITEM_EDITOR_WIDGET_STYLE_SHEET_H

#include <QString>
#include <QVariant>

class QFont;
class QBrush;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Helper class to generate CSS string for basic widget style sheet
   */
  class WidgetStyleSheet
  {
   public:

    /*! \brief Set text font
     */
    void setTextFont(const QFont & font);

    /*! \brief Get text font
     *
     * Returns a QVariant holding a QFont if a text font was set,
     *  otherwise a null QVariant.
     */
    QVariant textFont() const
    {
      return mTextFont;
    }

    /*! \brief Set foreground brush
     */
    void setForegroundBrush(const QBrush & brush);

    /*! \brief Get foreground brush
     *
     * Returns a QVariant holding a QBrush if a foreground was set,
     *  otherwise a null QVariant.
     */
    QVariant foregroundBrush() const
    {
      return mForegroundBrush;
    }

    /*! \brief Set background brush
     */
    void setBackgroundBrush(const QBrush & brush);

    /*! \brief Get background brush
     *
     * Returns a QVariant holding a QBrush if a background was set,
     *  otherwise a null QVariant.
     */
    QVariant backgroundBrush() const
    {
      return mBackgroundBrush;
    }

    /*! \brief Check if this style sheet is null
     */
    bool isNull() const
    {
      return ( mTextFont.isNull() && mForegroundBrush.isNull() && mBackgroundBrush.isNull() );
    }

    /*! \brief Clear this style sheet
     */
    void clear();

    /*! \brief Check if this style sheet is equal to other
     */
    bool operator==(const WidgetStyleSheet & other);

    /*! \brief Get CSS string for this style sheet
     */
    QString toCssString() const;

   private:

    QString getTextFontCssString() const;
    QString fontSizeString(const QFont & font) const;
    QString fontStyleString(const QFont & font) const;
    QString fontWeightString(const QFont & font) const;

    QString getForegroundCssString() const;

    QString getBackgroundCssString() const;

    QVariant mTextFont;
    QVariant mForegroundBrush;
    QVariant mBackgroundBrush;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_WIDGET_STYLE_SHEET_H
