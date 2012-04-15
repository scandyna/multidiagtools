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
#ifndef MDT_UIC_NUMBER_ITEM_H
#define MDT_UIC_NUMBER_ITEM_H

#include <QString>

/*! \brief UIC Number item
 * \sa mdtUicNumber
 */
class mdtUicNumberItem
{
 public:

  /*! \brief Create a unknow item.
   * 
   * A unknow item has unknow names
   */
  mdtUicNumberItem();

  ~mdtUicNumberItem();

  /*! \brief Set numerical value
   */
  void setNumber(int number);

  /*! \brief Set numerical value
   * 
   * Overloaded method.
   * \return True if ok, false on conversion error
   */
  bool setNumber(const QString &number);

  /*! \brief Get numerical value
   */
  int number();

  /*! \brief Set code string
   * 
   * Code string is used for country code (CH, FR, ...)
   */
  void setCode(const QString &code);

  /*! \brief Get code string
   * \sa setCode()
   */
  QString &code();

  /*! \brief Set the name string for french
   */
  void setNameFr(const QString &name);

  /*! \brief Get the name string in french
   */
  QString &nameFr();

  /*! \brief Set the name string for english
   */
  void setNameEn(const QString &name);

  /*! \brief Get the name string in english
   */
  QString &nameEn();

  /*! \brief Set the name string for german
   */
  void setNameDe(const QString &name);

  /*! \brief Get the name string in german
   */
  QString &nameDe();

 private:

  int pvNumber;
  QString pvCode;
  QString pvNameFr;
  QString pvNameEn;
  QString pvNameDe;
};

#endif  // #ifndef MDT_UIC_NUMBER_ITEM_H
