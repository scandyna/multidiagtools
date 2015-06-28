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
#ifndef MDT_CL_MODIFICATION_DATA_H
#define MDT_CL_MODIFICATION_DATA_H

#include <QString>

/*! \brief Link direction type
 */
enum class mdtClModification_t
{
  Undefined,  /*!< Undefined modification (is the case for null data) */
  New,        /*!< New modification (add for given version) */
  Rem,        /*!< Remove modification (remove for given version) */
  ModNew,     /*!< Modification: new part (a mdoification for given version, add part) */
  ModRem,     /*!< Modification: remove part (a mdoification for given version, remove part) */
  Exists      /*!< No modification */
};

/*! \brief Modification primary key data
 *
 * Refers to Modification_tbl
 */
struct mdtClModificationPkData
{
  /*! \brief Code_PK
   */
  QString code;

  /*! \brief Set modification
   */
  void setModification(mdtClModification_t m)
  {
    switch(m){
      case mdtClModification_t::New:
        code = "NEW";
        break;
      case mdtClModification_t::Rem:
        code = "REM";
        break;
      case mdtClModification_t::ModNew:
        code = "MODNEW";
        break;
      case mdtClModification_t::ModRem:
        code = "MODREM";
        break;
      case mdtClModification_t::Exists:
        code = "EXISTS";
        break;
      case mdtClModification_t::Undefined:
        code = "";
        break;
    }
  }

  mdtClModification_t modification() const
  {
    QString c = code;

    if(c == "NEW"){
      return mdtClModification_t::New;
    }else if(c == "REM"){
      return mdtClModification_t::Rem;
    }else if(c == "MODNEW"){
      return mdtClModification_t::ModNew;
    }else if(c == "MODREM"){
      return mdtClModification_t::ModRem;
    }else if(c == "EXISTS"){
      return mdtClModification_t::Exists;
    }else{
      return mdtClModification_t::Undefined;
    }
  }

  /*! \brief Check if PK is null
   *
   * PK is null if code is empty
   */
  bool isNull() const
  {
    return code.trimmed().isEmpty();
  }

  /*! \brief Clear
   */
  void clear()
  {
    code.clear();
  }
};

#endif // #ifndef MDT_CL_MODIFICATION_DATA_H
