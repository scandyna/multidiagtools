/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#ifndef MDT_CABLE_LIST_LINK_TYPE_MODEL_H
#define MDT_CABLE_LIST_LINK_TYPE_MODEL_H

#include "mdtError.h"
#include "LinkTypePk.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QLocale>
#include <QString>

class QComboBox;

namespace Mdt{ namespace CableList{

  /*! \brief Model that acts on LinkType_tbl
   *
   * This model is typically used with QComboBox
   *  for link type selection.
   *
   * This model will select Code_PK and a name field that depends on locale
   *  (see constructors for details).
   *
   * Usage:
   * \code
   *   using Mdt::CableList::LinkTypeModel;
   *   QComboBox *cb = new QComboBox;
   *   cb->setModel(new LinkTypeModel(this, db));
   *   cb->setModelColumn(1);  // Will display name field
   * \endcode
   */
  class LinkTypeModel : public QSqlQueryModel
  {
   public:

    /*! \brief Constructor
     */
    LinkTypeModel(QObject *parent, const QSqlDatabase & db);

    /*! \brief Constructor
     */
    LinkTypeModel(const QSqlDatabase & db);

    /*! \brief Set locale
     *
     * Will choose name field that matches
     *  locale. If not available, english name is choosen.
     */
    void setLocale(const QLocale & locale);

    /*! \brief Get row that contains given link type
     *
     * \return Row of searched link type or -1 if not found.
     */
    int row(LinkType type);

    /*! \brief Get row that contains given PK
     *
     * \return Row of searched key or -1 if not found.
     */
    int row(LinkTypePk pk);

    /*! \brief Get link type primary key for given row
     *
     * If row is < 0, a null PK is returned (lastError() is not updated).
     * If row >= 0, and a error occured, a null PK is returned and lastError() contains error.
     * If all works fine, requested PK is returned.
     */
    LinkTypePk primaryKey(int row);

    /*! \brief Get link primary key of given combobox's current index
     *
     * \sa primaryKey()
     */
    LinkTypePk currentPrimaryKey(const QComboBox * const cb);

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

    /*! \brief Update query
     */
    void updateQuery();

    /*! \brief Check if QSqlQueryModel is in error (and store if true)
     */
    bool isInError();

    QString pvNameFieldName;
    QSqlDatabase pvDatabase;
    mdtError pvLastError;
  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_TYPE_MODEL_H

