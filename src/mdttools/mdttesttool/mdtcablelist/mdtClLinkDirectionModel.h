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
#ifndef MDT_CL_LINK_DIRECTION_MODEL_H
#define MDT_CL_LINK_DIRECTION_MODEL_H

#include "mdtError.h"
#include "mdtClLinkDirectionData.h"
#include "mdtClLinkTypeData.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QLocale>
#include <QString>

/*! \brief Model that acts on LinkDirection_tbl
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
 *   cb->setModel(new mdtClLinkDirectionModel);
 *   cb->setModelColumn(1);  // Will display name field
 * \endcode
 */

class mdtClLinkDirectionModel : public QSqlQueryModel
{
 public:

  /*! \brief Constructor
   */
  mdtClLinkDirectionModel(QObject *parent, QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Constructor
   */
  mdtClLinkDirectionModel(QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Get row that contains given key
   *
   * \return Row of searched key or -1 if not found.
   */
  int row(const mdtClLinkDirectionKeyData & key);

  /*! \brief Get connection type key data for given row
   *
   * If row is < 0, a null key is returned (lastError() is not updated).
   * If row >= 0, and a error occured, a null key is returned and lastError() contains error.
   * If all works fine, requested key is returned.
   */
  mdtClLinkDirectionKeyData keyData(int row);

  /*! \brief Set link type
   *
   * Will filter available directions regarding given link type.
   *  F.ex. a cable link can only be bidirectional, but a diode
   *  is unidirectional (start to end or end to start)
   */
  void setLinkType(const mdtClLinkTypeKeyData & key);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClLinkDirectionModel);

  /*! \brief Check if QSqlQueryModel is in error (and store if true)
   */
  bool isInError();

  QString pvBaseSql;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_LINK_DIRECTION_MODEL_H
