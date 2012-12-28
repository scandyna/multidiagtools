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
#ifndef MDT_ALGORITHMS_H
#define MDT_ALGORITHMS_H

#include <QtAlgorithms>
#include <QChar>
#include <QString>
#include <QByteArray>
#include <QStringList>

/*! \brief Some usefull little (unefficient) algoritms
 */
namespace mdtAlgorithms
{
  /*! \brief Sort a string list, for witch each string ends with a number
   *
   * Usefull for directory entry list. By default, QDir sorts the entris
   * at alphabetic order. For exemple, if we have ttyS0, ttyS1, ttyS2, ... , ttyS10 ,
   * the entries will be sorted: ttyS0, ttyS1, ttyS10 , ttyS2, ...
   * With this function, the result will be: ttyS0, ttyS1, ttyS2, ..., ttyS10, ...
   * Note that this function is not efficient. For 100 entries it ok, but for bigger
   * data set, it's recommanded to take a look at Qt's Model/View framework,
   * and the QSortFilterProxyModel
   *
   * \param list The list that will be sorted (will be altered)
   * \return The sorted list
   */
  QStringList sortStringListWithNumericEnd(QStringList &list);

  /*! \brief Extract the numeric part at end of string
   *
   * \param str The string in witch the numeric part must be extracted
   * \return The numeric part, or empty string on problem
   */
  QString extractNumericPartAtEnd(const QString &str);

  /*! \brief Compare the numeric end of 2 strings
   *
   * This is a helper function for sortStringListWithNumericEnd().
   *
   * \return True if str1 < str2
   */
  bool stringWithNumericEndLessThan(QString str1, QString str2);

  /*! \brief Convert a hexadecimal formated string to numerical bytes array
   * 
   * Hexadecimal string is, for example, 2C 12 5F ,
   *  witch will be converted to 44,18,95 numeric byte array.
   * 
   * \pre hexStr must contain multiple of 2 valid chars. F.ex: 2C 12 5 is not valid.
   * \pre hexStr must contain valid chars (0-9 , a-f, A-F). F.ex: 2Z is not valid.
   */
  QByteArray hexStringToByteArray(const QString &hexStr);

  /*! \brief Convert a byte array to a hexadecimal string.
   * 
   * For example, byte array with 44,18,95 numerical values
   *  will generate a string with "2C 12 5F".
   */
  QString byteArrayToHexString(const QByteArray &byteArray);

  /*! \brief Get unprotected string
   *
   * It can happen that a protected string, f.ex. "Hello", must
   *  be extracted. Additionnally, escape chars can be inside.
   * This is a helper function used by splitString().
   * 
   * For example, if data protection is " and escape char is \ ,
   *  "A\"BC\"D" will return A"BC"D.
   * If escape char is alone, nothing will happen:
   *  "A\BCD" will return A\BCD.
   * If data proection not exists, nothing will happen:
   *  ABC will return ABC.
   *
   * \param str The input string to parse
   * \param dataProtection Data protection (typical: " )
   * \param escapeChar Escape char (typical: \ )
   * \param strEndOffset If not Null, the offset (of first char) of closing dataProtection,
   *                      before removing escape chars, is stored to this parameter.
   *                      This is used by splitString(). Some examples with " as dataProtection and \ as escapeChar:
   *                       - For "A\"BC\"D" : value will be 9
   *                       - For "A\BCD" : value will be 6
   *                       - For "A\BCD : value will be -1 (no closing " found)
   *                       - For A : value will be -1 (no " found)
   */
  QString unprotectedString(const QString &str, const QString &dataProtection, const QChar &escapeChar, int *strEndOffset = 0);

  /*! \brief Split a string reagrding a separator and data protection
   *
   * QString has a totally functionnal split method, but it does not
   *  support data proection.
   * For example, if we have to split a string: "123";"AB;C" witch uses
   *  ; char as separator and " as protection, we must have a list
   *  with 2 items: 123 and AB;C.
   *
   * Some details:
   *  - If str is empty, a empty list is returned
   *  - Empty records (data around separator) are added to list
   *
   * \param str The input string to parse
   * \param separator Separator to use (typical: ; )
   * \param protection Data protection (typical: " )
   * \return The list of found fields
   */
  QStringList splitString(const QString &str, const QString &separator, const QString &dataProtection, const QChar &escapeChar = QChar());
};

#endif  // #ifndef MDT_ALGORITHMS_H
