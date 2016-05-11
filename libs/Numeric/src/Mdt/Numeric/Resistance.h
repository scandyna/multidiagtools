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
#ifndef MDT_NUMERIC_RESISTANCE_H
#define MDT_NUMERIC_RESISTANCE_H

#include "PhysicsType.h"
#include <QVariant>

namespace Mdt{ namespace Numeric{

  /*! \brief Value type that represents a resistance
   */
  class Resistance : public PhysicsType<Resistance>
  {
   public:

    /*! \brief Construct a null resistance
     */
    constexpr Resistance() noexcept
     : PhysicsType() {}

    /*! \brief Construct a resistance
     */
    explicit constexpr Resistance(Double r) noexcept
     : PhysicsType(r) {}

    /*! \brief Construct a resistance
     */
    explicit constexpr Resistance(double r) noexcept
     : PhysicsType(Double(r)) {}

    /*! \brief Get resistance from a QVariant
     *
     * If r is null, a null resistance is returned
     *
     * \pre If r has a value (is not null), it must be convertible to double
     */
    static Resistance fromQVariant(const QVariant & r)
    {
      Resistance R;

      if(r.isNull()){
        return R;
      }
      Q_ASSERT(r.canConvert<double>());
      R = Resistance(r.toDouble());

      return R;
    }

  };

}} // namespace Mdt{ namespace Numeric{

class QDebug;

/*! \brief Stream operator for QDebug
 */
QDebug operator<<(QDebug dbg, const Mdt::Numeric::Resistance &);

#endif // #ifndef MDT_NUMERIC_RESISTANCE_H
