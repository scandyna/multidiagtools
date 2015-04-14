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
#ifndef MDT_TT_VALUE_LIMITS_H
#define MDT_TT_VALUE_LIMITS_H

#include "mdtAlgorithms.h"

/*! \brief Class to define limits of values in test applications
 *
 *  <table>
 *   <tr><td>-&infin;</td><td style="background-color:red; color:white">Left fail zone</td><td>#leftBottomLimit</td><td bgcolor="orange">Left limit range</td><td>#leftTopLimit</td>
 *   <td bgcolor="00FF00">Ok zone</td>
 *   <td>#rightBottomLimit</td><td bgcolor="orange">Right limit range</td><td>#rightTopLimit</td><td style="background-color:red; color:white">Right fail zone</td><td>+&infin;</td></tr>
 *  </table>
 */
class mdtTtValueLimits
{
 public:

  /*! \brief Result regarding a given value and defined limits
   */
  enum class Result_t
  {
    Undefined,  /*!< Result is undefined. */
    Fail,       /*!< Result is in fail zone */
    Limit,      /*!< Result is in limit zone */
    Ok          /*!< Result is OK */
  };

  /*! \brief Left bottom limit
   *
   * Access functions:<br>
   *  - setLeftBottomLimit()
   *  - leftBottomLimit()
   *  - resetLeftBottomLimit()
   */
  Q_PROPERTY(double leftBottomLimit READ leftBottomLimit WRITE setLeftBottomLimit RESET resetLeftBottomLimit)

  /*! \brief Left top limit
   *
   * Access functions:<br>
   *  - setLeftTopLimit()
   *  - leftTopLimit()
   *  - resetLeftTopLimit()
   */
  Q_PROPERTY(double leftTopLimit READ leftTopLimit WRITE setLeftTopLimit RESET resetLeftTopLimit)

  /*! \brief Right bottom limit
   *
   * Access functions:<br>
   *  - setRightBottomLimit()
   *  - rightBottomLimit()
   *  - resetRightBottomLimit()
   */
  Q_PROPERTY(double rightBottomLimit READ rightBottomLimit WRITE setRightBottomLimit RESET resetRightBottomLimit)

  /*! \brief Right top limit
   *
   * Access functions:<br>
   *  - setRightTopLimit()
   *  - rightTopLimit()
   *  - resetRightTopLimit()
   */
  Q_PROPERTY(double rightTopLimit READ rightTopLimit WRITE setRightTopLimit RESET resetRightTopLimit)

  /*! \brief Default constructor
   */
  mdtTtValueLimits();

  /*! \brief Get result
   */
  Result_t result() const
  {
    return pvResult;
  }

  /*! \brief Get left bottom limit
   */
  double leftBottomLimit() const
  {
    return pvLeftLimitRange.bottom;
  }

  /*! \brief Set left bottom limit
   *
   * \pre x must be <= #leftTopLimit
   */
  void setLeftBottomLimit(double x);

  /*! \brief Reset left bottom limit
   */
  void resetLeftBottomLimit();

  /*! \brief Get left top limit
   */
  double leftTopLimit() const
  {
    return pvLeftLimitRange.top;
  }

  /*! \brief Set left top limit
   *
   * \pre x must be >= #leftBottomLimit and <= #rightBottomLimit
   */
  void setLeftTopLimit(double x);

  /*! \brief Reset left top limit
   */
  void resetLeftTopLimit();

  /*! \brief Get right bottom limit
   */
  double rightBottomLimit() const
  {
    return pvRightLimitRange.bottom;
  }

  /*! \brief Set right bottom limit
   *
   * \pre x must be >= #leftTopLimit and <= #rightTopLimit
   */
  void setRightBottomLimit(double x);

  /*! \brief Reset right bottom limit
   */
  void resetRightBottomLimit();

  /*! \brief Get top bottom limit
   */
  double rightTopLimit() const
  {
    return pvRightLimitRange.top;
  }

  /*! \brief Set right top limit
   *
   * \pre x must be >= #rightBottomLimit
   */
  void setRightTopLimit(double x);

  /*! \brief Reset top bottom limit
   */
  void resetRightTopLimit();

 private:

  mdtAlgorithms::NumericRangeDouble pvLeftLimitRange;
  mdtAlgorithms::NumericRangeDouble pvRightLimitRange;
  Result_t pvResult;
};

#endif // #ifndef MDT_TT_VALUE_LIMITS_H
