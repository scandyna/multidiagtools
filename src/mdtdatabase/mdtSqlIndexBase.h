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
#ifndef MDT_SQL_INDEX_BASE_H
#define MDT_SQL_INDEX_BASE_H

#include "mdtSqlField.h"
#include "mdtSqlDriverType.h"
#include <QString>
#include <QSqlIndex>
#include <QList>

/*! \brief Common stuff for SQL schema indexes
 */
class mdtSqlIndexBase
{
 public:

  /*! \brief Set index name
   */
  void setName(const QString & name)
  {
    pvName = name;
  }

  /*! \brief Get index name
   */
  QString name() const
  {
    return pvName;
  }

  /*! \brief Add a field to index
   */
  void addField(const mdtSqlField & field)
  {
    pvFields.append(field);
  }

  /*! \brief Get field count
   */
  int fieldCount() const
  {
    return pvFields.size();
  }

  /*! \brief Get field at given index
   *
   * \pre index must be in valid range
   */
  mdtSqlField field(int index) const
  {
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < pvFields.size());
    return pvFields.at(index);
  }

  /*! \brief Get field for given field name
   */
  mdtSqlField field(const QString & name) const
  {
    for(const auto & field : pvFields){
      if(field.name() == name){
        return field;
      }
    }
    return mdtSqlField();
  }

  /*! \brief Setup index from QSqlIndex
   */
  void setupFromQSqlIndex(const QSqlIndex & index, mdtSqlDriverType::Type driverType)
  {
    clear();
    pvName = index.name();
    for(int i = 0; i < index.count(); ++i){
      mdtSqlField field;
      field.setupFromQSqlField(index.field(i), driverType);
      pvFields.append(field);
    }
  }

  /*! \brief Check if index is null
   *
   * Index is null is if name is empty,
   *  or if no field was added.
   */
  bool isNull() const
  {
    return (pvName.isEmpty() || pvFields.isEmpty());
  }

  /*! \brief Clear
   */
  void clear()
  {
    pvName.clear();
    pvFields.clear();
  }

 private:

  QString pvName;

 protected:

  QList<mdtSqlField> pvFields;
};

#endif // #ifndef MDT_SQL_INDEX_BASE_H
