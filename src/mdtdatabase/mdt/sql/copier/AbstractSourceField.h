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
#ifndef MDT_SQL_COPIER_ABSTRACT_SOURCE_FIELD_H
#define MDT_SQL_COPIER_ABSTRACT_SOURCE_FIELD_H

#include <QSharedData>
#include <QVariant>

//#include <QDebug>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Base class to implement SourceField
   */
  class AbstractSourceField : public QSharedData
  {
   public:

    /*! \brief Default constructor
     */
    AbstractSourceField()
     : QSharedData()
    {
    }

    /*! \brief Destructor
     */
    virtual ~AbstractSourceField()
    {
    }

    // Disable copy (must use clone())
    AbstractSourceField(const AbstractSourceField &) = delete;
    AbstractSourceField & operator=(const AbstractSourceField &) = delete;

    /*! \brief Polymorphic copy
     */
    virtual AbstractSourceField* clone() const = 0;

    /*! \brief Check if source field is null
     */
    virtual bool isNull() const = 0;

    /*! \brief Clear
     */
    virtual void clear() = 0;

    /*! \brief Set source field index
     *
     * This default implementation does nothing
     */
    virtual void setFieldIndex(int index)
    {
      Q_UNUSED(index);
    }

    /*! \brief Get source field index
     *
     * This default implementation allways returns -1
     */
    virtual int fieldIndex() const
    {
      return -1;
    }

    /*! \brief Set fixed value
     *
     * This default implementation does nothing
     */
    virtual void setFixedValue(const QVariant & value)
    {
      Q_UNUSED(value);
    }

    /*! \brief Get fixed value
     *
     * This default implementation allways return a null value
     */
    virtual QVariant fixedValue() const
    {
      return QVariant();
    }

  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_ABSTRACT_SOURCE_FIELD_H

