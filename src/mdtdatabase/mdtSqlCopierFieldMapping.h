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

#include <QVariant>

/*! \brief Field mapping data
 *
 * Contains only data for field mapping.
 *  The functionnal part is implemented
 *  in specific copier.
 */
struct mdtSqlCopierFieldMapping
{
  /*! \brief Field mapping state
   */
  enum MappingState
  {
    MappingNotSet,      /*!< Source or destination field index was not set.
                             This is the default. */
    MappingComplete,    /*!< Field mapping was done automatically without any mismatch,
                             or confirmed by the user as complete. */
    MappingPartial,     /*!< Field mapping was done automatically, but some mismatch
                             was detected, and a action is required from the user to fix it */
    MappingError        /*!< A error was detected in field mapping. */
  };

  /*! \brief Source type
   */
  enum SourceType
  {
    Field,        /*!< Data will be copied regarding source and destination field mapping */
    FixedValue    /*!< fixedValue will be copied to destination mapped field */
  };

  /*! \brief Source field index
   */
  int sourceFieldIndex;

  /*! \brief Destination field index
   */
  int destinationFieldIndex;

  /*! \brief Field mapping state
   */
  MappingState mappingState;

  /*! \brief Source type
   */
  SourceType sourceType;

  /*! \brief Source fixed value
   *
   * \note has only sense if sourceType is FixedValue.
   */
  QVariant sourceFixedValue;

  /*! \brief Default constructor
   *
   * Will set source and destination field indexes to -1
   */
  mdtSqlCopierFieldMapping()
   : sourceFieldIndex(-1),
     destinationFieldIndex(-1),
     mappingState(mdtSqlCopierFieldMapping::MappingNotSet),
     sourceType(Field)
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
   *
   * Will set source and destination field indexes to -1
   */
  void clear()
  {
    sourceFieldIndex = -1;
    destinationFieldIndex = -1;
    mappingState = MappingNotSet;
    sourceType = Field;
    sourceFixedValue.clear();
  }
};

#endif // MDT_SQL_COPIER_FIELD_MAPPING_H
