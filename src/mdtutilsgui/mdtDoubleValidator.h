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
#ifndef MDT_DOUBLE_VALIDATOR_H
#define MDT_DOUBLE_VALIDATOR_H

#include <QValidator>
#include <QDoubleValidator>
#include <QString>
///#include <QStringRef>

/*! \brief Double validator
 *
 * mdtDoubleValidator uses QDoubleValidator internally,
 *  but also accepts a power of 10 suffix.
 */
class mdtDoubleValidator : public QValidator
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtDoubleValidator(QObject* parent = 0);

  /*! \brief Set a custom suffix
   *
   * Can be typically a unit (A, V, ...)
   */
  void setSuffix(const QString & s);

  /*! \brief Validate input
   */
  QValidator::State validate(QString & input, int & pos) const;

 private:

  /*! \brief Check if str beginns with a digit or a acceptable char (typically -)
   */
  bool strBeginIsNumber(const QString & str) const;

  /*! \brief Check if str has only accpeted elements
   *
   * Accepted elements are given by charIsAccepted().
   */
  bool strHasOnlyAccpedElements(const QString & str) const;

  /*! \brief Check if a char is accpeted
   *
   * Accepted elements are:
   *  - Digits
   *  - Chars used to format a number (- + , . e E)
   *  - Power of 10 suffixes
   *  - Chars contained in pvSuffix
   */
  bool charIsAccepted(const QChar & c) const;

  /*! \brief Check if char if a power of 10 suffix
   */
  bool charIsPowerOf10Suffix(const QChar & c) const;

  /*! \brief Check str ends state
   *
   * str ends correctly if it is ready if:
   *  - It ends with a digit
   *  - It ends with a power of 10 char
   *  - It ends with pvSuffix
   */
  bool strEndsCorrectly(const QString & str) const;

  /*! \brief Check if str ends with a power of 10 suffix
   */
  bool strEndsWithPowerOf10Suffix(const QString & str) const;

  /*! \brief Check if str ends with pvSuffix
   *
   *  \return 0 : str does not end with pvSuffix , 1 : str ends with a part of pvSuffix , 2 : str ends with pvSuffix
   */
  ///int strEndsWithSuffix(const QString & str) const;

  Q_DISABLE_COPY(mdtDoubleValidator);

  QDoubleValidator pvValidator;
  QString pvSuffix;
};

#endif  // #ifndef MDT_DOUBLE_VALIDATOR_H
