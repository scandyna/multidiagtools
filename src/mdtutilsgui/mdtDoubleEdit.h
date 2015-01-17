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
#ifndef MDT_DOUBLE_EDIT_H
#define MDT_DOUBLE_EDIT_H

#include "mdtAlgorithms.h"
#include <QWidget>
#include <QString>
#include <QVariant>
#include <vector>

class QLineEdit;
class QPushButton;
class mdtDoubleValidator;

/*! \brief Store informations about a range of a unit
 */
struct mdtDoubleEditUnitRange : public mdtAlgorithms::NumericRangeDouble
{
  /*! \brief Construct a invalid range
   */
  mdtDoubleEditUnitRange()
   : NumericRangeDouble(),
     po10prefix('\0'),
     po10exponent(0)
   {
   }

  /*! \brief Construct a unit range that represents all values from min to max
   */
  mdtDoubleEditUnitRange(double min, double max, const QChar & po10c, int po10e, bool includeMin = true, bool includeMax = false)
   : NumericRangeDouble(min, max, includeMin, includeMax),
     po10prefix(po10c),
     po10exponent(po10e)
  {
  }

  /*! \brief Power of 10 prefix (n, u, m, k, ...)
   */
  QChar po10prefix;

  /*! \brief Power of 10 exponent
   */
  int po10exponent;
};

/*! \brief Input box for double values
 *
 * A value can be displayed with the value itself, a power of 10 factor and a unit.
 *  For example, 1 V represents a value 1 and is displayed with [V] unit.
 *  1 mV also represents value 1e-3 and is displayed with [V] unit.
 *
 * Unit exponent, for example m2, is also supported.
 */
class mdtDoubleEdit : public QWidget
{
 Q_OBJECT

 Q_PROPERTY(QString unit READ unit WRITE setUnit)
 Q_PROPERTY(EditionMode_t editionMode READ editionMode WRITE setEditionMode)
 Q_ENUMS(EditionMode_t)
 Q_PROPERTY(double minimum READ minimum WRITE setMinimum RESET setMinimumToMinusInfinity)
 Q_PROPERTY(double maximum READ maximum WRITE setMaximum RESET setMaximumToInfinity)

 public:

  /*! \brief Special symbols for units
   */
  enum class UnitSymbol_t
  {
    omega,          /*!< Omega (small letter) , &omega; */
    OmegaCapital    /*!< Capital omega , &Omega; */
  };

  /*! \brief Edition mode
   */
  enum EditionMode_t
  {
    DefaultEditionMode,   /*!< Default edition mode */
    WireEditionMode       /*!< Wire edition mode */
  };

  /*! \brief Unit prefix ranges
   */
//   enum class UnitPrefixRange_t
//   {
//     Default,        /*!< Contains all supported ranges */
//     CableSection    /*!< Contains range of mm2 [-inf;+inf] */
//   };

  /*! \brief Constructor
   */
  mdtDoubleEdit(QWidget* parent = 0);

  /*! \brief Set read only
   */
  void setReadOnly(bool ro);

  /*! \brief Get unit
   */
  inline QString unit() const { return pvUnit; }

  /*! \brief Set minimum
   */
  void setMinimum(double min);

  /*! \brief Set minimum to -infinity
   */
  void setMinimumToMinusInfinity();

  /*! \brief Get minimum
   */
  double minimum() const;

  /*! \brief Set maximum
   */
  void setMaximum(double max);

  /*! \brief Set maximum to infinity
   */
  void setMaximumToInfinity();

  /*! \brief Get maximum
   */
  double maximum() const;

  /*! \brief Set range
   */
  void setRange(double min, double max);

  /*! \brief Get power of 10 exponent for given char
   *
   * Will return y in 10^y expression, or 0 if char is unknown.
   *
   * Supported chars are:
   *
   *  <table border="1" cellpadding="5">
   *   <tr><th>Char</th><th>Designation</th><th>Factor</th></tr>
   *   <tr><td>a</td><td>atto</td><td>-18</td></tr>
   *   <tr><td>f</td><td>femto</td><td>-15</td></tr>
   *   <tr><td>p</td><td>pico</td><td>-12</td></tr>
   *   <tr><td>n</td><td>nano</td><td>-9</td></tr>
   *   <tr><td>u</td><td>micro</td><td>-6</td></tr>
   *   <tr><td>m</td><td>milli</td><td>-3</td></tr>
   *   <tr><td>c</td><td>centi</td><td>-2</td></tr>
   *   <tr><td>d</td><td>deci</td><td>-1</td></tr>
   *   <tr><td>h</td><td>hecto</td><td>2</td></tr>
   *   <tr><td>k</td><td>kilo</td><td>3</td></tr>
   *   <tr><td>M</td><td>mega</td><td>6</td></tr>
   *   <tr><td>G</td><td>giga</td><td>9</td></tr>
   *   <tr><td>T</td><td>tera</td><td>12</td></tr>
   *   <tr><td>P</td><td>peta</td><td>15</td></tr>
   *   <tr><td>E</td><td>exa</td><td>18</td></tr>
   *  </table>
   */
  static int p10exponent(const QChar & c);

  /*! \brief Set value from a string
   *
   * s can also contain power of 10 char
   *  and unit. Unit must be the same than
   *  set with setUnit().
   */
  void setValueString(const QString & s, bool emitValueChanged = true);

  /*! \brief Set value
   */
  void setValueDouble(double v, bool emitValueChanged = true);

  /*! \brief Set value
   */
  void setValue(const QVariant & v, bool emitValueChanged = true);

  /*! \brief Get value
   */
  inline double valueDouble() const { return pvValue; }

  /*! \brief Get value
   */
  QVariant value() const;

  /*! \brief Check if value is null
   *
   * \sa isValid()
   */
  inline bool isNull() const { return pvIsNull; }

  /*! \brief Check if value is valid
   *
   * Value is valid only if it represents a number or is null.
   *  For example, if setValue() was called with a pure string,
   *  like 'abcd', the function will return false.
   *
   * Note that a value can also be null and valid.
   */
  inline bool valueIsValid() const { return pvValueIsValid; }

  /*! \brief Validate edited data
   *
   * After user has finished editing value,
   *  it is converted, checked and displayed in recognised way.
   *  The valueChanged() signal will also be emitted.
   *  Editing is finished when user hits \<Enter\> key,
   *  or internal line edit losses focus.
   *  To proceed to conversion and validation sooner than that,
   *  call this function.
   *
   * \return If currently edited data represents a valid value,
   *          true is returned, false else.
   */
  bool validate();

  /*! \brief Get text in line edit
   */
  QString text() const;

  /*! \brief Get infinity string
   */
  static QString infinityString();

  /*! \internal Access internal line edit (used for unit tests)
   */
  QLineEdit *lineEdit()
  {
    return pvLineEdit;
  }

  /*! \brief Get edition mode
   */
  EditionMode_t editionMode() const
  {
    return pvEditionMode;
  }

 public slots:

  /*! \brief Set unit
   *
   * If u ends with 2 or 3,
   *  unit will be considered having a exponent.
   *  For example, passing m2 will be unit m (meter) with a unit exponent of 2.
   */
  void setUnit(const QString & u);

  /*! \brief Set edition mode
   */
  void setEditionMode(EditionMode_t mode);

 signals:

  /*! \brief Emitted when a new value was set
   *
   * Happens when user has finished editting,
   *  or when a set function was called with
   *  emitValueChanged flag set.
   */
  void valueChanged(double value, bool isValid);

  /*! \brief Emitted when user has edited value
   *
   * Is a simple relay of internal line edit's textEdited() signal.
   */
  void valueEdited();

 private slots:

  void setNull();
  void setMinusInfinity();
  void setInfinity();
  void setValueFromLineEdit();

 private:

  /*! \brief Set wire section edition mode
   */
  void setWireSectionEditionMode();

  /*! \brief Clear value
   *
   * Will set a invalid value and clear line edit.
   */
  void clear();

  /*! \brief Get special symbol char
   */
  QString specialSymbolChar(UnitSymbol_t symbol) const;

  /*! \brief Get string for unit
   *
   * If a special unit is recognized, it will be converted to
   *  its unicode string.
   *
   * \sa specialSymbolChar()
   */
  QString formatedUnit(const QString & u) const;

  /*! \brief Convert and set value from string
   */
  void convertAndSetValue(QString str);

  /*! \brief Display stored value
   */
  void displayValue();

  /*! \brief Check if str is a infinity (or -infinity)
   */
  bool strIsInfinity(const QString & str) const;

  /*! \brief Returns the range of witch x is from pvUnitRanges
   */
  mdtDoubleEditUnitRange getRange(double x);

  /*! \brief Build default unit ranges
   */
  void buildDefaultUnitRanges();

  /*! \brief Build default unit ranges for square unit (f.ex. [m2])
   */
  void buildDefaultSquareUnitRanges();

  /*! \brief Build ranges for wire section
   */
  void buildWireSectionRanges();

  Q_DISABLE_COPY(mdtDoubleEdit);

  QLineEdit *pvLineEdit;
  QPushButton *pbSetNull;
  QPushButton *pbSetInfinity;
  QPushButton *pbSetMinusInfinity;
  mdtDoubleValidator *pvValidator;
  
  EditionMode_t pvEditionMode;
  
  QString pvUnit;
  double pvValue;
  std::vector<mdtDoubleEditUnitRange> pvUnitRanges;   // Used to form number when displayed
  int pvUnitExponent;  // F.ex. m2 : unit=m, unitExponent=2 , used to do line edit text -> numeric conversion
  bool pvIsNull;
  bool pvValueIsValid;
};

#endif // #ifndef MDT_DOUBLE_EDIT_H
