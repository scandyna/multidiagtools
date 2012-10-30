/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_ANALOG_IO_H
#define MDT_ANALOG_IO_H

#include "mdtAbstractIo.h"

/*! \brief Representation of a analog I/O
 *
 * On wide range of automation applications,
 *  it is usefull to display a output to the user.
 *  For example, a analog I/O module, with a 8 bit
 *  encoding could be displayed with this class
 *  togther with mdtAnalogInWidget or mdtAnalogOutWidget.
 */
class mdtAnalogIo : public mdtAbstractIo
{
  Q_OBJECT

 public:

  mdtAnalogIo(QObject *parent = 0);

  ~mdtAnalogIo();

  /*! \brief Set the unit (V, A, °C, ...)
   */
  void setUnit(const QString &unit);

  /*! \brief Get unit
   */
  QString unit() const;

  /*! \brief Set the range
   *
   * Store the new range and set the value to the minimum.
   *  The valueChanged() signal is emitted.
   *
   * Note for UI developpers:
   *  - The signal rangeChangedForUi() is emited
   *  - The signal valueChangedForUi() is emited
   *
   * \param min Minimum value to display (f.ex. 0V, or 4mA)
   * \param max Maximum value to display (f.ex. 10V, or 20mA)
   * \param steps Number of steps of the input (f.ex. 256 with 8 bits resolution)
   * \pre steps must be > 1
   * \pre max must be > min
   */
  void setRange(double min, double max, int steps);

  /*! \brief Get the minimum value of the range
   */
  double minimum() const;

  /*! \brief Get the maximum value of the range
   */
  double maximum() const;

  /*! \brief Set the value to update display
   *
   * Store the value and emit valueChanged() if
   *  new value is different from current.
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  void setValue(double value);

  /*! \brief Get value (set with setValue() )
   */
  double value() const;

  /*! \brief Set the integer value
   *
   * Will calculate the real value depending on resolution and update display.
   *  The resolution is set with setRange() (steps parameter)
   *
   * The valueChanged() signal is emitted.
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  void setValueInt(int value);

  /*! \brief Get the integer value
   *
   * The integer value depend on value and resolution.
   *  The resolution is set with setRange() (steps parameter)
   */
  int valueInt() const;

 signals:

  /*! \brief This signal is emitted whenever the value is changed
   */
  void valueChanged(double newValue);

  /*
   * These signals are emited every time
   *  a member is set with a setter method.
   * Usefull to update the UI (should not be used for other purpose)
   */
  void unitChangedForUi(const QString &unit);
  void rangeChangedForUi(double min, double max);
  void valueChangedForUi(double newValue);

 private slots:

  // Used from UI to update internal value.
  //  The valueChangedForUi() signal will not be emitted with this call.
  void setValueFromUi(double value);

 private:

  Q_DISABLE_COPY(mdtAnalogIo);

  QString pvUnit;
  double pvValue;
  double pvMinimum;
  double pvMaximum;
  double pvStep;
  double pvStepInverse;
  bool pvUpdatingUi;
};

#endif  // #ifndef MDT_ANALOG_IO_H
