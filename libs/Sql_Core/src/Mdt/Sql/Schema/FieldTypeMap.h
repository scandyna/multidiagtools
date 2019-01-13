/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_SQL_SCHEMA_FIELD_TYPE_MAP_H
#define MDT_SQL_SCHEMA_FIELD_TYPE_MAP_H

#include "FieldType.h"
#include "FieldTypeFlags.h"
#include "FieldLength.h"
#include "MdtSql_CoreExport.h"
#include <QMetaType>
#include <QVariant>
#include <memory>
#include <type_traits>

namespace Mdt{ namespace Sql{ namespace Schema{

  class FieldTypeMapImplementation;

  /*! \brief Map a FieldType from or to a QMetaType::Type
   *
   *
   * Provides a mapping between QMetaType::Type and common SQL data type.
   *
   *  This can be used to propose a starting point to the user
   *  for database import or export tools.
   *  Mdt::Sql::Schema::tableFromReflected() also use it this way.
   *
   * The default implementation uses this table:
   *
   * | QMetaType type |   FieldType  | FieldTypeFlags |   %FieldLength    | Remarks
   * |:--------------:|:------------:|:--------------:|:-----------------:|:------------------------------------------------------------------------------------------------------------------------|
   * |  UnknownType   | UnknownType  | NoFlag         | null              |                                                                                                                         |
   * |  Bool          | Boolean      | NoFlag         | null              |                                                                                                                         |
   * |  Short         | Smallint     | NoFlag         | null              |                                                                                                                         |
   * |  UShort        | Smallint     | IsUnsigned     | null              |                                                                                                                         |
   * |  Int           | Integer      | NoFlag         | null              | Correspond to a C++ int, at least 16 bit, mostly 32 bit                                                                 |
   * |  UInt          | Integer      | IsUnsigned     | null              | Correspond to a C++ unsigned int, at least 16 bit, mostly 32 bit                                                        |
   * |  Long          | Bigint       | NoFlag         | null              | Correspond to a C++ long int, at least 32 bit (Win64), can be 64 bit (64bit Linux, Mac OS-X). See notes below.          |
   * |  ULong         | Bigint       | IsUnsigned     | null              | Correspond to a C++ unsigned long int, at least 32 bit (Win64), can be 64 bit (64bit Linux, Mac OS-X). See notes below. |
   * |  LongLong      | Bigint       | NoFlag         | null              | Correspond to a C++11 long long int, at least 64 bit                                                                    |
   * |  ULongLong     | Bigint       | IsUnsigned     | null              | Correspond to a C++11 unsigned long long int, at least 64 bit                                                           |
   * |  Float         | Float        | NoFlag         | null              |                                                                                                                         |
   * |  Double        | Double       | NoFlag         | null              |                                                                                                                         |
   * |  Char          | ?            | ?              | ?                 |                                                                                                                         |
   * |  SChar         | ?            | ?              | ?                 |                                                                                                                         |
   * |  UChar         | ?            | ?              | ?                 |                                                                                                                         |
   * |  QChar         | Char         | NoFlag         | null              | Correspond to a SQL CHAR(1), which can store 1 character (which can require 1 or more byte, dependeing on encoding)     |
   * |  QString       | Char         | NoFlag         | 2                 |                                                                                                                         |
   * |  QString       | Varchar      | NoFlag         | > 2               | Varchar may require at least 1 byte as length prefix.                                                                   |
   * |  QString       | Text         | NoFlag         | null              |                                                                                                                         |
   * |  QByteArray    | Blob         | IsBinary       | null              |                                                                                                                         |
   * |  QDate         | ?            | ?              | ?                 |                                                                                                                         |
   * |  QTime         | ?            | ?              | ?                 |                                                                                                                         |
   * |  QDateTime     | ?            | ?              | ?                 |                                                                                                                         |
   * |  QImage        | ?            | ?              | ?                 |                                                                                                                         |
   * |  QPixmap       | ?            | ?              | ?                 |                                                                                                                         |
   * |  QBitmap       | ?            | ?              | ?                 |                                                                                                                         |
   *
   * It is also possible to provide a different table by subclassing FieldTypeMapImplementation
   *  and setting it using setImplementation() or make<>().
   *
   * For a overview of C++ fundamental types, see http://en.cppreference.com/w/cpp/language/types
   *
   * To instantiate a FieldTypeMap with its default implementation:
   * \code
   * const auto fieldTypeMap = FieldTypeMap::make();
   * \endcode
   *
   * \note Some types are mapped the same way in both directions:
   * \code
   * FieldType fieldType = fieldTypeMap.fieldTypeFromQMetaType(QMetaType::Int);
   * // fieldType is FieldType::Integer
   * QMetaType::Type metaType = fieldTypeMap.qmetaTypeFromFieldType(fieldType, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag));
   * // metaType is QMetaType::Int
   * \endcode
   * For some types, this is not the case:
   * \code
   * FieldType fieldType = fieldTypeMap.fieldTypeFromQMetaType(QMetaType::Long);
   * // fieldType is FieldType::Bigint
   * QMetaType::Type metaType = fieldTypeMap.qmetaTypeFromFieldType(fieldType, FieldLength(), FieldTypeFlags(FieldTypeFlag::NoFlag));
   * // metaType is QMetaType::LongLong
   * \endcode
   */
  class MDT_SQL_CORE_EXPORT FieldTypeMap
  {
   public:

    /*! \brief Create a field type map without any implementation
     */
    FieldTypeMap() = default;

    /*! \brief Destructor
     */
    ~FieldTypeMap();

    /*! \brief Copy construct a field type map from \a other
     */
    FieldTypeMap(const FieldTypeMap & other) = default;

    /*! \brief Copy assign a field type map from \a other
     */
    FieldTypeMap & operator=(const FieldTypeMap & other) = default;

    /*! \brief Move construct a field type map from \a other
     */
    FieldTypeMap(FieldTypeMap && other) = default;

    /*! \brief Move assign a field type map from \a other
     */
    FieldTypeMap & operator=(FieldTypeMap && other) = default;

    /*! \brief Check if this map is valid
     *
     * returns true if this map contains a implementation.
     */
    bool isValid() const
    {
      if(mImpl){
        return true;
      }
      return false;
    }

    /*! \brief Set the default implementation
     */
    void setDefaultImplementation();

    /*! \brief Set a specific implementation
     *
     * \pre \a Implementation must be a subclass of FieldTypeMapImplementation
     */
    template<typename Implementation>
    void setImplementation()
    {
      static_assert( std::is_base_of<FieldTypeMapImplementation, Implementation>::value, "Implementation must be a subclass of FieldTypeMapImplementation" );

      mImpl = std::make_shared<const Implementation>();
    }

    /*! \brief Get the field type that corresponds to \a qmetaType
     *
     * \pre This map must contain a implementation.
     * \sa isValid()
     */
    FieldType fieldTypeFromQMetaType(QMetaType::Type qmetaType, FieldLength fieldLength) const;

    /*! \brief Get the field type that corresponds to \a qvariantType
     *
     * \pre This map must contain a implementation.
     * \sa isValid()
     */
    FieldType fieldTypeFromQVariantType(QVariant::Type qvariantType, FieldLength fieldLength) const
    {
      return fieldTypeFromQMetaType( static_cast<QMetaType::Type>(qvariantType), fieldLength );
    }

    /*! \brief Get the field type flags that corresponds to \a qmetaType
     *
     * \pre This map must contain a implementation.
     * \sa isValid()
     */
    FieldTypeFlags fieldTypeFlagsFromQMetaType(QMetaType::Type qmetaType) const;

    /*! \brief Get the field type flags that corresponds to \a qvariantType
     *
     * Note that QVariant::Type is marked as deprecated in Qt's documentation.
     *
     * \pre This map must contain a implementation.
     * \sa isValid()
     */
    FieldTypeFlags fieldTypeFlagsFromQVariantType(QVariant::Type qvariantType) const
    {
      return fieldTypeFlagsFromQMetaType( static_cast<QMetaType::Type>(qvariantType) );
    }

    /*! \brief Get QMetaType from \a fieldType, \a fieldLength and \a fieldTypeFlags
     *
     * \pre This map must contain a implementation.
     * \sa isValid()
     */
    QMetaType::Type qmetaTypeFromFieldType(FieldType fieldType, FieldLength fieldLength, FieldTypeFlags fieldTypeFlags) const;

    /*! \brief Create a field type map with a specific implementation
     *
     * \pre \a Implementation must be a subclass of FieldTypeMapImplementation
     */
    template<typename Implementation>
    static FieldTypeMap make()
    {
      static_assert( std::is_base_of<FieldTypeMapImplementation, Implementation>::value, "Implementation must be a subclass of FieldTypeMapImplementation" );

      return FieldTypeMap( std::make_shared<const Implementation>() );
    }

    /*! \brief Create a field type map with the default implementation
     */
    static FieldTypeMap make();

   private:

    FieldTypeMap(const std::shared_ptr<const FieldTypeMapImplementation> & impl)
     : mImpl(impl)
    {
    }

    std::shared_ptr<const FieldTypeMapImplementation> mImpl;
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_FIELD_TYPE_MAP_H
