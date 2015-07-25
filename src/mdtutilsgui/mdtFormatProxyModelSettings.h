/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#ifndef MDT_FORMAT_PROXY_MODEL_SETTINGS_H
#define MDT_FORMAT_PROXY_MODEL_SETTINGS_H

#include <QBrush>
#include <QVariant>
#include <QString>

#include <QFlags>

#include <vector>

class QSqlQueryModel;

/*! \internal Text alignment container for mdtFormatProxyModelSettings
 */
struct mdtFormatProxyModelTextAlignment
{
  int column;
  Qt::Alignment alignment;

  // Constructor
  mdtFormatProxyModelTextAlignment(int col, Qt::Alignment a)
   : column(col), alignment(a) {}
};

/*! \internal Background container item for mdtFormatProxyModelSettings
 */
struct mdtFormatProxyModelBackgroundItem
{
  QVariant keyData;
  QBrush background;

  // Constructor
  mdtFormatProxyModelBackgroundItem(const QVariant & kd, const QBrush & bg)
   : keyData(kd), background(bg) {}
};

/*! \internal Background container for mdtFormatProxyModelSettings
 */
struct mdtFormatProxyModelBackground
{
  int keyDataColumn;

 private:
  std::vector<mdtFormatProxyModelBackgroundItem> pvItems;

 public:

  mdtFormatProxyModelBackground()
   : keyDataColumn(0) {}

  void addBackground(const QVariant & keyData, const QBrush & bg);
  void clear();

  // Get background for given key - returns a null QVariant if no item with given key exists
  QVariant background(const QVariant & keyData) const;
};

/*! \brief Format settings for mdtFormatProxyModel
 */
class mdtFormatProxyModelSettings
{
 public:

  /*! \brief Add a text alignment for given column
   */
  void addTextAlignment(int column, Qt::Alignment alignment);

  /*! \brief Add a text alignment for given column
   *
   * Note: if given fieldName does not exist in model,
   *  this function silently adds nothing.
   */
  void addTextAlignment(QSqlQueryModel *model, const QString & fieldName, Qt::Alignment alignment);

  /*! \brief Get text alignment for given coloumn
   *
   * \return A QVariant containing the alignment (a Qt::AlignmentFlag),
   *          or a null QVariant if not alignment was set for requested column
   */
  QVariant textAlignment(int column) const;

  /*! \brief Set column that contains key data to determine backgroud
   */
  void setBackgroundKeyDataColumn(int column)
  {
    pvBackground.keyDataColumn = column;
  }

  /*! \brief Set column that contains key data to determine backgroud
   *
   * Note: if given fieldName does not exist in model,
   *  this function silently updates nothing.
   */
  void setBackgroundKeyDataColumn(QSqlQueryModel *model, const QString & fieldName);

  /*! \brief Get column that contains key data to determine backgroud
   */
  int backgroundKeyDataColumn() const
  {
    return pvBackground.keyDataColumn;
  }

  /*! \brief Add a background for given key data
   */
  void addBackground(const QVariant & keyData, const QBrush & bg)
  {
    pvBackground.addBackground(keyData, bg);
  }

  /*! \brief Get background for given key data
   *
   * \return A QVariant that contains background (a QBrush) for given keyData,
   *          or a null QVariant if not background was defined for given keyData
   */
  QVariant background(const QVariant & keyData) const
  {
    return pvBackground.background(keyData);
  }

  /*! \brief Clear settings
   */
  void clear();

 private:

  std::vector<mdtFormatProxyModelTextAlignment> pvTextAlignments;
  mdtFormatProxyModelBackground pvBackground;
};

#endif // #ifndef MDT_FORMAT_PROXY_MODEL_SETTINGS_H
