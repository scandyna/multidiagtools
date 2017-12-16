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
#ifndef MDT_SQL_SCHEMA_CASE_SENSITIVITY_H
#define MDT_SQL_SCHEMA_CASE_SENSITIVITY_H

namespace Mdt{ namespace Sql{ namespace Schema{

  /*! \brief SQL Schema case sensitivity flag
   */
  enum class CaseSensitivity
  {
    NotDefined,        /*!< Case sensitivity is not defined (use defaults). */
    CaseInsensitive,  /*!< Not case sensitive.
                           For some DBMS, like PostgrSQL, this is only possible for Text field type,
                           which is in reallity replaced by a CITEXT field type during SQL generation by Driver. */
    CaseSensitive     /*!< Case sensitive. */
  };

}}} // namespace Mdt{ namespace Sql{ namespace Schema{

#endif // #ifndef MDT_SQL_SCHEMA_CASE_SENSITIVITY_H
