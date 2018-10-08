/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_ERROR_CODE_H
#define MDT_ERROR_CODE_H

namespace Mdt{

  /*! \brief A list of error codes
   */
  enum class ErrorCode
  {
    NoError,              /*!< No error */
    UnknownError,         /*!< Unknown error */
    ConstraintError,      /*!< Some constraint error */
    UniqueConstraintError /*!< A unique constraint failed */
  };

} // namespace Mdt{

#endif // #ifndef MDT_ERROR_CODE_H
