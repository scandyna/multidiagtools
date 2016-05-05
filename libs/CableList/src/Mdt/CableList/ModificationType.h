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
#ifndef MDT_CABLE_LIST_MODIFICATION_TYPE_H
#define MDT_CABLE_LIST_MODIFICATION_TYPE_H

namespace Mdt{ namespace CableList{

  /*! \brief Modification type
   */
  enum class ModificationType
  {
    Undefined,  /*!< Undefined modification (is the case for null data) */
    New,        /*!< New modification (add for given version) */
    Rem,        /*!< Remove modification (remove for given version) */
    ModNew,     /*!< Modification: new part (a mdoification for given version, add part) */
    ModRem,     /*!< Modification: remove part (a mdoification for given version, remove part) */
    Exists      /*!< No modification */
  };


}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_MODIFICATION_TYPE_H
