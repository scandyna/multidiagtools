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
#ifndef MDT_NUMERIC_LENGTH_H
#define MDT_NUMERIC_LENGTH_H

#include "PhysicsType.h"
#include <QVariant>

namespace Mdt{ namespace Numeric{

  /*! \brief Value type that represents a length
   */
  class Length : public PhysicsType<Length>
  {
   public:

    /*! \brief Construct a null length
     */
    constexpr Length() noexcept
     : PhysicsType() {}

    /*! \brief Construct a length
     */
    explicit constexpr Length(Double l) noexcept
     : PhysicsType(l) {}

    /*! \brief Construct a length
     */
    explicit constexpr Length(double l) noexcept
     : PhysicsType(Double(l)) {}

    /*! \brief Get length from a QVariant
     *
     * If l is null, a null length is returned
     *
     * \pre If l has a value (is not null), it must be convertible to double
     */
    static Length fromQVariant(const QVariant & l)
    {
      if(l.isNull()){
        return Length();
      }
      Q_ASSERT(l.canConvert<double>());
      return Length(l.toDouble());
    }

  };

}} // namespace Mdt{ namespace Numeric{

class QDebug;

/*! \brief Stream operator for QDebug
 */
QDebug operator<<(QDebug dbg, const Mdt::Numeric::Length &);

#endif // #ifndef MDT_NUMERIC_LENGTH_H
