/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_FIELD_MAP_FIELD_H
#define MDT_FIELD_MAP_FIELD_H

#include <QSqlField>
#include <QString>
#include <QStringList>

/*! \brief Container for information about field in field map
 *
 * Internally, in QSqlField object contains most of field attributes.
 *  Those not aviable in QSqlField are also stored separatly.
 */
class mdtFieldMapField
{
 public:

  /*! \brief Constructor
   */
  mdtFieldMapField();

  /*! \brief Set column index
   */
  void setIndex(int value);

  /*! \brief Get column index
   */
  int index() const;

  /*! \brief Set text that will be displayed in header
   */
  void setDisplayText(const QString & text);

  /*! \brief Get text that will be displayed in header
   */
  QString displayText() const;

  /*! \brief Set field name
   *
   * Is stored in sqlField
   */
  void setName(const QString & name);

  /*! \brief Get field name
   *
   * Comes from sqlField
   */
  QString name() const;

  /*! \brief Set SQL field
   */
  void setSqlField(const QSqlField & field);

  /*! \brief Get SQL field
   */
  QSqlField sqlField() const;

  /*! \brief Get SQL field object for update
   */
  QSqlField &sqlField();

  /*! \brief Check if str contains a illegal string
   */
  static bool strContainsIllegalString(const QString &str);

  /*! \brief Get a field name with no illegal strings
   */
  static QString getFieldName(const QString &name, const QString &replaceBy = "_");

 private:

  QSqlField pvSqlField;
  int pvIndex;
  QString pvDisplayText;
  static QStringList spvIllegalStrings;
};
Q_DECLARE_METATYPE(mdtFieldMapField);

#endif // #ifndef MDT_FIELD_MAP_FIELD_H
