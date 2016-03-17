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
#ifndef MDT_PROGRESS_VALUE_H
#define MDT_PROGRESS_VALUE_H

#include <QElapsedTimer>
#include <type_traits>

struct mdtProgressValueNoTimer
{
  void start(){}
};

struct mdtProgressValueTimer
{
  mdtProgressValueTimer()
   : notifyInterval(50)
  {
    timer.start();
  }

  void start()
  {
    timer.start();
  }

  bool isTimeToNotify() const
  {
    return timer.hasExpired(notifyInterval);
  }

  int notifyInterval;
  QElapsedTimer timer;
};

/*! \brief Helper class for progress bar
 *
 * To show progress of a process to the user,
 *  Qt offers QProgressBar and QProgressDialog.
 *  Those classes are complete (calculate scaling, ...).
 *
 * mdtProgressValue was created to help with some problems.
 *
 * When copying files, we could use total bytes count as progress,
 *  witch conducts of using very large numbers, stored for example,
 *  in 64 bit integers, witch also needs rescaling value to be able
 *  to transmit them to progress widget (witch talks in int).
 *  A other danger is to update the progress widget each time
 *  a couple of bytes where copied, witch causes heavy
 *  GUI updates, and can also conduct to a stack overflow (GUI event stack).
 *
 * To display progress bars in a model/view system,
 *  a solution is to create a item delegate (such as mdtProgressBarItemDelegate).
 *  In this situation, code becomes more complex if ranges must be stored for each row of the model.
 *  The simple solution is to setup the delegate with minium and maximum,
 *  and scale all values in the process.
 *
 * \tparam T Type of value to count
 * \tparam TimerType mdtProgressValueNoTimer if no timer is requierd, else mdtProgressValueTimer
 */
template<typename T, typename TimerType = mdtProgressValueNoTimer>
class mdtProgressValue
{
 public:

  /*! \brief Default constructor
   *
   * Will initialize value to 0,
   *  scale factor to 1.0
   *  and start notify timer.
   */
  mdtProgressValue()
   : pvValue(0),
     pvScaleFactor(1.0)/*,
     pvNotifyInterval(50)*/
  {
//     pvTimer.start();
  }

  /*! \brief Set scale factor
   *
   * \param min Minimum value
   * \param max Maximum value
   * \param scaledMin Minimum value of the progress widget
   * \param scaledMax Maximum value of the progress widget
   * \pre max - min must be > 0
   */
  void setScaleFactor(T min, T max, int scaledMin, int scaledMax)
  {
    Q_ASSERT((max - min) > static_cast<T>(0));
    pvScaleFactor = static_cast<double>(scaledMax - scaledMin) / static_cast<double>(max - min);
  }

  /*! \brief Set range
   *
   * This is the same as calling setScaleFactor(min, max, 0, 100)
   *
   * \param min Minimum value
   * \param max Maximum value
   * \pre max - min must be > 0
   */
  inline void setRange(T min, T max)
  {
    Q_ASSERT((max - min) > static_cast<T>(0));
    setScaleFactor(min, max, 0, 100);
  }

  /*! \brief Set value
   */
  void setValue(T value)
  {
    pvValue = value;
  }

  /*! \brief Add steps to value
   */
  void increment(T steps)
  {
    pvValue += steps;
  }

  /*! \brief Add steps to value
   */
  mdtProgressValue<T, TimerType> & operator+=(T steps)
  {
    pvValue += steps;
    return *this;
  }

  /*! \brief Add 1 step to value
   */
  mdtProgressValue<T, TimerType> & operator++()
  {
    ++pvValue;
    return *this;
  }

  /*! \brief Get value
   */
  T value() const
  {
    return pvValue;
  }

  /*! \brief Get scaled value
   *
   * Scaled value is the one to be transmitted to the progress widget.
   *
   * Calling this function will also reset the timer.
   *
   * \note The scaled value is calculated at each call of this function.
   */
  int scaledValue()
  {
    pvTimer.start();
    return pvValue * pvScaleFactor;
  }

  /*! \brief Set notify interval
   *
   * \param interval Interval [ms]
   */
  void setNotifyInterval(int interval)
  {
    static_assert(std::is_same<TimerType, mdtProgressValueTimer>::value , "only works with a timer");
    pvTimer.notifyInterval = interval;
//     pvNotifyInterval = interval;
  }

  /*! \brief Check if time to notify a new value was reached
   */
  bool isTimeToNotify() const
  {
    static_assert(std::is_same<TimerType, mdtProgressValueTimer>::value , "only works with a timer");
    return pvTimer.isTimeToNotify();
//     return pvTimer.hasExpired(pvNotifyInterval);
  }

 private:

  T pvValue;
  double pvScaleFactor;
  TimerType pvTimer;
//   int pvNotifyInterval;
//   QElapsedTimer pvTimer;
};

#endif // #ifndef MDT_PROGRESS_VALUE_H
