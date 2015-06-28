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
#ifndef MDT_CL_MODIFICATION_MODEL_H
#define MDT_CL_MODIFICATION_MODEL_H

#include "mdtError.h"
#include "mdtClModificationKeyData.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QLocale>
#include <QString>

class QComboBox;

/*! \brief Model that acts on Modification_tbl
 *
 * This model is typically used with QComboBox
 *  for modification selection.
 *
 * This model will select Code_PK and a name field that depends on given locale
 *  (see constructors for details).
 *
 * Usage:
 * \code
 *   QComboBox *cb = new QComboBox;
 *   cb->setModel(new mdtClModificationModel(this, db));
 *   cb->setModelColumn(1);  // Will display name field
 * \endcode
 */
class mdtClModificationModel : public QSqlQueryModel
{
 public:

  /*! \brief Constructor
   */
  mdtClModificationModel(QObject *parent, QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Constructor
   */
  mdtClModificationModel(QSqlDatabase db, const QLocale & locale = QLocale());

  /*! \brief Get row that contains given modification
   *
   * \return Row of searched modification or -1 if not found.
   */
  int row(mdtClModification_t m);

  /*! \brief Get row that contains given modification
   *
   * \return Row of searched modification or -1 if not found.
   */
  int row(const mdtClModificationPkData & key);

  /*! \brief Get modification PK for given row
   *
   * If row is < 0, a null PK is returned (lastError() is not updated).
   * If row >= 0, and a error occured, a null PK is returned and lastError() contains error.
   * If all works fine, requested PK is returned.
   */
  mdtClModificationPkData modificationPk(int row);

  /*! \brief Get modification PK of given combo box current index
   *
   * \sa modificationPk()
   */
  mdtClModificationPkData currentModificationPk(QComboBox *cb);

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  Q_DISABLE_COPY(mdtClModificationModel);

  /*! \brief Check if QSqlQueryModel is in error (and store if true)
   */
  bool isInError();

  mdtError pvLastError;
};

#endif // #ifndef MDT_CL_MODIFICATION_MODEL_H

