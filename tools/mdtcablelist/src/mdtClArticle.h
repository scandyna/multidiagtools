/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CL_ARTICLE_H
#define MDT_CL_ARTICLE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>
#include <QModelIndex>

/*! \brief Helper class for Article and related tables edition
 */
class mdtClArticle
{
 public:

  /*! \brief Constructor
   */
  mdtClArticle(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClArticle();

  /*! \brief Get last error
   */
  const QSqlError &lastError() const;

  /*! \brief Add a record in Link table
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode);

  /*! \brief 
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeLink(const QVariant & linkId);

  /*! \brief 
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeLinks(const QVariant & linkIdList);

  /*! \brief 
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeLinks(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Add a link of type resistor
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addResistor(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value);

  /*! \brief Add a link of type diode
   */
  bool addDiode(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double Vf, const QVariant & directionCode);

  /*! \brief Add a link of type bridge
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addBridge(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId);

 private:

  QSqlDatabase pvDatabase;
  QSqlError pvLastError;
};

#endif  // #ifndef MDT_CL_ARTICLE_H
