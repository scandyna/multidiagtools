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
#include <QString>
#include <QByteArray>
#include <QStringList>

/*! \brief Some usefull little (unefficient) algoritms
 */
namespace mdtAlgorithms
{
  /*! \brief Sort a string list, for witch each string ends with a number
   * Usefull for directory entry list. By default, QDir sorts the entris
   * at alphabetic order. For exemple, if we have ttyS0, ttyS1, ttyS2, ... , ttyS10 ,
   * the entries will be sorted: ttyS0, ttyS1, ttyS10 , ttyS2, ...
   * With this function, the result will be: ttyS0, ttyS1, ttyS2, ..., ttyS10, ...
   * Note that this function is not efficient. For 100 entries it ok, but for bigger
   * data set, it's recommanded to take a look at Qt's Model/View framework,
   * and the QSortFilterProxyModel
   * \param list The list that will be sorted (will be altered)
   * \return The sorted list
   */
  QStringList sortStringListWithNumericEnd(QStringList &list);

  /*! \brief Extract the numeric part at end of string
   * \param str The string in witch the numeric part must be extracted
   * \return The numeric part, or empty string on problem
   */
  QString extractNumericPartAtEnd(const QString &str);

  /*! \brief Compare the numeric end of 2 strings
   * This is a helper function for sortStringListWithNumericEnd()
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
};

#endif  // #ifndef MDT_ALGORITHMS_H
