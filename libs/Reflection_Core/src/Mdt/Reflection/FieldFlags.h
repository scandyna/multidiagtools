/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_REFLECTION_FIELD_FLAGS_H
#define MDT_REFLECTION_FIELD_FLAGS_H

#include <QFlags>

namespace Mdt{ namespace Reflection{

  /*! \brief This enum lists existing flags available for a reflected struct field
   */
  enum class FieldFlag
  {
    IsRequired = 0x01,      /*!< Field is required */
    HasDefaultValue = 0x02  /*!< Field has a default value.
                              For example, if a data struct has a default value:
                              \code
                              struct MyStruct
                              {
                                QString code = "DEFAULT";
                              };
                              \endcode
                              and this flag is set for the code field,
                              the default value will be used by some storages as default value
                              (for example in SQL databases).
                             */
  };
  Q_DECLARE_FLAGS(FieldFlags, FieldFlag)

}} // namespace Mdt{ namespace Reflection{

Q_DECLARE_OPERATORS_FOR_FLAGS(Mdt::Reflection::FieldFlags)

#endif // #ifndef MDT_REFLECTION_FIELD_FLAGS_H
