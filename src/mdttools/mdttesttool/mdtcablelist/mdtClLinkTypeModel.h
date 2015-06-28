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
#ifndef MDT_CL_LINK_TYPE_MODEL_H
#define MDT_CL_LINK_TYPE_MODEL_H

#include "mdtError.h"
#include "mdtClLinkTypeData.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QLocale>
#include <QString>

/*! \brief Model that acts on LinkType_tbl
 *
 * This model is typically used with QComboBox
 *  for link type selection.
 *
 * This model will select Code_PK and a name field that depends on given locale
 *  (see constructors for details).
 *
 * Usage:
 * \code
 *   QComboBox *cb = new QComboBox;
 *   cb->setModel(new mdtClLinkTypeModel(this, db));
 *   cb->setModelColumn(1);  // Will display name field
 * \endcode
 */
class mdtClLinkTypeModel : public QSqlQueryModel
{
 public:

  /*! \brief Constructor
   */
  mdtClLinkTypeModel(QObject *parent, QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Constructor
   */
  mdtClLinkTypeModel(QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Get row that contains given link type
   *
   * \return Row of searched link type or -1 if not found.
   */
  int row(mdtClLinkType_t t);

  /*! \brief Get row that contains given key
   *
   * \return Row of searched key or -1 if not found.
   */
  int row(const mdtClLinkTypeKeyData & key);

  /*! \brief Get link type key data for given row
   *
   * If row is < 0, a null key is returned (lastError() is not updated).
   * If row >= 0, and a error occured, a null key is returned and lastError() contains error.
   * If all works fine, requested key is returned.
   */
  mdtClLinkTypeKeyData keyData(int row);

  /*! \brief Get unit for given row
   *
   * If row is < 0, a empty string is returned (lastError() is not updated).
   * If row >= 0, and a error occured, a string containing <error> is returned and lastError() contains error.
   * If all works fine, requested unit is returned.
   */
  QString unit(int row);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClLinkTypeModel);

  /*! \brief Check if QSqlQueryModel is in error (and store if true)
   */
  bool isInError();

  mdtError pvLastError;
};

#endif // MDT_CL_LINK_TYPE_MODEL_H
