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
#ifndef MDT_CSV_FIELD_TYPE_H
#define MDT_CSV_FIELD_TYPE_H

#include <QMetaType>
#include <QString>
#include <vector>

namespace mdt{ namespace csv{

  /*! \brief CSV field type
   */
  class FieldType
  {
   public:

    /*! \brief Construct a invalid type
     */
    FieldType();

    /*! \brief Construct a type
     */
    FieldType(QMetaType::Type type);

    /*! \brief Get type
     */
    QMetaType::Type type() const
    {
      return pvType;
    }

    /*! \brief Get type name
     */
    QString name() const
    {
      return pvName;
    }

    /*! \brief Get name of given type
     */
    static QString nameFromType(QMetaType::Type type);

    /*! \brief Get a list of avaliable CSV field types
     */
    static std::vector<FieldType> getAvailableFieldTypeList();

   private:

    QMetaType::Type pvType;
    QString pvName;
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_FIELD_TYPE_H
