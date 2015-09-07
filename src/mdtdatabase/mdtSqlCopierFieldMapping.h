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
#ifndef MDT_SQL_COPIER_FIELD_MAPPING_H
#define MDT_SQL_COPIER_FIELD_MAPPING_H

/*! \brief Field mapping data
 *
 * \sa mdtSqlFieldMappingDialog
 */
struct mdtSqlCopierFieldMapping
{
  /*! \brief Source field index
   */
  int sourceFieldIndex;

  /*! \brief Destination field index
   */
  int destinationFieldIndex;

  /*! \brief Default constructor
   */
  mdtSqlCopierFieldMapping()
   : sourceFieldIndex(-1),
     destinationFieldIndex(-1)
  {
  }

  /*! \brief Check if mapping data is null
   *
   * Mapping data is null if source or destination field index is < 0
   */
  bool isNull() const
  {
    return ( (sourceFieldIndex < 0) || (destinationFieldIndex < 0) );
  }

  /*! \brief Clear
   */
  void clear()
  {
    sourceFieldIndex = -1;
    destinationFieldIndex = -1;
  }
};

#endif // MDT_SQL_COPIER_FIELD_MAPPING_H
