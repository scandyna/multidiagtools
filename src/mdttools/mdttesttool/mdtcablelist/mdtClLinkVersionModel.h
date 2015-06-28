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
#ifndef MDT_CL_LINK_VERSION_MODEL_H
#define MDT_CL_LINK_VERSION_MODEL_H

#include "mdtError.h"
#include "mdtClLinkVersionData.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>

class QComboBox;

/*! \brief Model that acts on LinkVersion_tbl
 *
 * This model is typically used with QComboBox
 *  for link version selection.
 *
 * This model will select Version_PK and Version
 *  (see constructors for details).
 *
 * Usage:
 * \code
 *   QComboBox *cb = new QComboBox;
 *   cb->setModel(new mdtClLinkVersionModel(this));
 *   cb->setModelColumn(1);  // Will display Version field
 * \endcode
 */
class mdtClLinkVersionModel : public QSqlQueryModel
{
 public:

  /*! \brief Constructor
   */
  mdtClLinkVersionModel(QObject *parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClLinkVersionModel(QSqlDatabase db);

  /*! \brief Get row that contains given version PK
   *
   * \return Row of searched key or -1 if not found.
   */
  int row(const mdtClLinkVersionPkData & key);

  /*! \brief Get version PK for given row
   *
   * If row is < 0, a null PK is returned (lastError() is not updated).
   * If row >= 0, and a error occured, a null PK is returned and lastError() contains error.
   * If all works fine, requested PK is returned.
   */
  mdtClLinkVersionPkData versionPk(int row);

  /*! \brief Get version PK of given combo box current index
   *
   * \sa versionPk()
   */
  mdtClLinkVersionPkData currentVersionPk(QComboBox *cb);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClLinkVersionModel);

  /*! \brief Check if QSqlQueryModel is in error (and store if true)
   */
  bool isInError();

  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_LINK_VERSION_MODEL_H
