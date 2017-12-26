/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_DATA_STRUCT_DEF_H
#define MDT_ENTITY_DATA_STRUCT_DEF_H

#include <QString>

namespace Mdt{ namespace Entity{

  /*! \brief Create and instanciate a data struct definition class
   *
   * Starting with a data struct:
   * \code
   * struct ClientDataStruct
   * {
   *   qlonlong id = 0;
   *   QString firstName;
   * };
   * \endcode
   *
   * To make a entity of this struct, some meta data must be available,
   *  like the field name, max length, ...
   *  Such meta data can be generated with MDT_ENTITY_DATA_STRUCT_DEF() macro:
   * \code
   * MDT_ENTITY_DATA_STRUCT_DEF(
   *   ??
   * )
   * \endcode
   *
   * Above code will expand to:
   * \code
   * struct ClientDataStructDef
   * {
   *   static const QString entityName()
   *   {
   *     return QStringLiteral("Client");
   *   }
   *
   *   struct idField
   *   {
   *     static const QString fieldName()
   *     {
   *       return QStringLiteral("id");
   *     }
   *
   *     static const FieldAttributes fieldAttributes()
   *     {
   *       return FieldAttributes(FieldFlag::IsRequired | FieldFlag::IsUnique);
   *     }
   *   };
   *
   *   struct firstNameField
   *   {
   *     static const QString fieldName()
   *     {
   *       return QStringLiteral("firstName");
   *     }
   *
   *     static const FieldAttributes fieldAttributes()
   *     {
   *       return FieldAttributes(FieldMaxLength(250));
   *     }
   *   };
   *
   *   idField id;
   *   firstNameField firstName;
   * };
   * \endcode
   */

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_DATA_STRUCT_DEF_H
