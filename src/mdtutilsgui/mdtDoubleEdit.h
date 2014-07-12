/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_DOUBLE_EDIT_H
#define MDT_DOUBLE_EDIT_H

#include <QWidget>
#include <QString>
#include <QVariant>
///#include <QDesignerCustomWidgetInterface>

class QLineEdit;
class QPushButton;
class mdtDoubleValidator;

/*! \brief Input box for double values
 */
class /**QDESIGNER_WIDGET_EXPORT*/ mdtDoubleEdit : public QWidget
{
 Q_OBJECT

 Q_PROPERTY(QString unit READ unit WRITE setUnit)

  /*! \brief Special symbols for units
   */
  enum unitSymbol_t {
                      OmegaCapital    /*!< Capital omega , &Omega */
                    };

 public:

  /*! \brief Constructor
   */
  mdtDoubleEdit(QWidget* parent = 0);

  /*! \brief Set unit
   */
  void setUnit(const QString & unit);

  /*! \brief Get unit
   */
  inline QString unit() const { return pvUnit; }

  /*! \brief Get the factor for a given char
   *
   * Will return factor of given char,
   *  or a 1.0 char is unknown.
   *
   * Supported chars are:
   *
   *  <table border="1" cellpadding="5">
   *   <tr><th>Char</th><th>Designation</th><th>Factor</th></tr>
   *   <tr><td>a</td><td>atto</td><td>1e-18</td></tr>
   *   <tr><td>f</td><td>femto</td><td>1e-15</td></tr>
   *   <tr><td>p</td><td>pico</td><td>1e-12</td></tr>
   *   <tr><td>n</td><td>nano</td><td>1e-9</td></tr>
   *   <tr><td>u</td><td>micro</td><td>1e-6</td></tr>
   *   <tr><td>m</td><td>milli</td><td>1e-3</td></tr>
   *   <tr><td>c</td><td>centi</td><td>1e-2</td></tr>
   *   <tr><td>d</td><td>deci</td><td>1e-1</td></tr>
   *   <tr><td>h</td><td>hecto</td><td>1e2</td></tr>
   *   <tr><td>k</td><td>kilo</td><td>1e3</td></tr>
   *   <tr><td>M</td><td>mega</td><td>1e6</td></tr>
   *   <tr><td>G</td><td>giga</td><td>1e9</td></tr>
   *   <tr><td>T</td><td>tera</td><td>1e12</td></tr>
   *   <tr><td>P</td><td>peta</td><td>1e15</td></tr>
   *   <tr><td>E</td><td>exa</td><td>1e18</td></tr>
   *  </table>
   */
  static double factor(const QChar & c);

  /*! \brief Set value from a string
   *
   * s can also contain power of 10 char
   *  and unit. Unit must be the same than
   *  set with setUnit().
   */
  void setValue(const QString & s, bool emitValueChanged = true);

  /*! \brief Set value
   */
  void setValue(double v, bool emitValueChanged = true);

  /*! \brief Get value
   */
  inline double valueDouble() const { return pvValue; }

  /*! \brief Get value
   */
  QVariant value() const;

  /*! \brief Check if value is valid
   */
  inline bool valueIsValid() const { return pvValueIsValid; }

  /*! \brief Get text in line edit
   */
  QString text() const;

  /*! \brief Get infinity string
   */
  static QString infinityString();

 signals:

  /*! \brief Emitted when a new value was set
   *
   * Happens when user has finished editting,
   *  or when a set functions was called with
   *  emitValueChanged flag set.
   */
  void valueChanged(double value, bool isValid);

 private slots:

  void setMinusInfinity();
  void setInfinity();
  void setValueFromLineEdit();

 private:

  /*! \brief Convert and set value from string
   */
  void convertAndSetValue(QString str);

  /*! \brief Display stored value
   */
  void displayValue();

  /*! \Check if x is in given range from min to max
   *
   * Returns true if x is [min, max[
   */
  bool isInRange(double x, double min, double max);

  /*! \brief Check if str is a infinity (or -infinity)
   */
  bool strIsInfinity(const QString & str) const;

  Q_DISABLE_COPY(mdtDoubleEdit);

  QLineEdit *pvLineEdit;
  QPushButton *pbSetInfinity;
  QPushButton *pbSetMinusInfinity;
  mdtDoubleValidator *pvValidator;
  QString pvUnit;
  double pvValue;
  bool pvValueIsValid;
};

#endif // #ifndef MDT_DOUBLE_EDIT_H
