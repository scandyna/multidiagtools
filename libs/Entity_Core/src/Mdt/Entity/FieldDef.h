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
#ifndef MDT_ENTITY_FIELD_DEF_H
#define MDT_ENTITY_FIELD_DEF_H

namespace Mdt{ namespace Entity{

  /*! \brief Create and instanciate a field definition class
   *
   * This is a helper macro for MDT_ENTITY_DEF() .
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
   *
   * \code
   * struct ClientDataStructDef
   * {
   *   MDT_ENTITY_FIELD_DEF(name)
   * };
   * \endcode
   *
   * \code
   * struct ClientDataStructDef
   * {
   *   struct idField
   *   {
   *     static const QString fieldName()
   *     {
   *       return "id";
   *     }
   *
   *     static const FieldAttributes fieldAttributes()
   *     {
   *       return FieldAttributes(FieldFlag::IsRequired | FieldFlag::IsUnique);
   *     }
   *   };
   * };
   * \endcode
   *
   * \sa MDT_ENTITY_DEF()
   */

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_DEF_H
