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
#ifndef MDT_ANALOG_OUT_WIDGET_H
#define MDT_ANALOG_OUT_WIDGET_H

#include "mdtAbstractIoWidget.h"
#include <QString>

class QDoubleSpinBox;
class QwtSlider;

/*! \brief Representation of a analog output
 *
 * On wide range of automation application,
 *  it is usefull to display a output to the user.
 *  For example, a analog output of a I/O module, with a 8 bit
 *  encoding could be displayed with this class.
 */
class mdtAnalogOutWidget : public mdtAbstractIoWidget
{
 Q_OBJECT

 public:

  mdtAnalogOutWidget(QWidget *parent = 0);

  ~mdtAnalogOutWidget();

  /*! \brief Set the range
   *
   * \param min Minimum value to display (f.ex. 0V, or 4mA)
   * \param max Maximum value to display (f.ex. 10V, or 20mA)
   * \param steps Number of steps of the input (f.ex. 256 with 8 bits resolution)
   * \pre steps must be > 1
   * \pre max must be > min
   */
  void setRange(double min, double max, int steps);

  /*! \brief Set the unit (V, A, °C, ...)
   */
  void setUnit(const QString &unit);

  /*! \brief Set the value to update display
   */
  void setValue(double value);

  /*! \brief Get value (set with setValue() )
   */
  double value();

  /*! \brief Set the integer value
   *
   * Will calculate the real value depending on resolution and update display.
   *  The resolution is set with setRange() (steps parameter)
   */
  void setValueInt(int value);

  /*! \brief Get the integer value
   *
   * The integer value depend on value and resolution.
   *  The resolution is set with setRange() (steps parameter)
   */
  int valueInt();

 signals:

  /*! \brief This signal is emitted whenever the value is changed
   */
  void valueChanged(double value);

 private:

  Q_DISABLE_COPY(mdtAnalogOutWidget);

  double pvStep;
  double pvStepInverse;
  double pvCurrentValue;
  QwtSlider *slValue;
  QDoubleSpinBox *sbValue;
  QString pvUnit;
};

#endif  // #ifndef MDT_ANALOG_OUT_WIDGET_H
