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
#ifndef MDT_SQL_COPIER_SOURCE_FIELD_H
#define MDT_SQL_COPIER_SOURCE_FIELD_H

#include <QSharedDataPointer>
#include <QVariant>

namespace mdt{ namespace sql{ namespace copier{

  class AbstractSourceField;

  /*! \brief Source field informations for mdtSqlCopierFieldMapping
   */
  class SourceField
  {
   public:

    /*! \brief Source field type
     */
    enum Type
    {
      SourceFieldIndexType,       /*!< Data will be fetched in field defined by field index in source */
      SourceFixedValueType,       /*!< A fixed value defined will be used */
      SourceFieldExpressionType   /*!< Data will be fetched regarding source field expression */
    };

    /*! \brief Construct a source field of given type
     */
    SourceField(Type type);

    /*! \brief Copy constructor
     */
    SourceField(const SourceField & other);

    /*! \brief Destructor
     */
    ~SourceField();

    /*! \brief Copy assignment
     */
    SourceField & operator=(const SourceField & other);

    /*! \brief Get source field type
     */
    Type type() const
    {
      return pvType;
    }

    /*! \brief Check if source field is null
     */
    bool isNull() const;

    /*! \brief Clear
     */
    void clear();

    /*! \brief Set source field index
     *
     * Does nothing if type is not SourceFieldIndex \todo wrong
     */
    void setFieldIndex(int index);

    /*! \brief Get source field index
     *
     * Returns allways -1 if type is not SourceFieldIndexType
     */
    int fieldIndex() const;

    /*! \brief Set source as given fixed value
     */
    void setFixedValue(const QVariant & value);

    /*! \brief Get source fixed value
     *
     * Returns allways a null value if type is not SourceFixedValueType
     */
    QVariant fixedValue() const;

   private:

    /*! \brief Reset to new type
     */
    void reset(Type type);

    /*! \brief Construct shared data
     */
    void constructShared();

    Type pvType;
    QSharedDataPointer<AbstractSourceField> pvShared;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_SOURCE_FIELD_H
