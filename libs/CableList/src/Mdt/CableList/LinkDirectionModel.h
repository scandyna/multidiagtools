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
#ifndef MDT_CABLE_LIST_LINK_DIRECTION_MODEL_H
#define MDT_CABLE_LIST_LINK_DIRECTION_MODEL_H

#include "mdtError.h"
#include "LinkDirectionPk.h"
#include "LinkType.h"
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QLocale>
#include <QString>

class QComboBox;

namespace Mdt{ namespace CableList{

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
   *   using Mdt::CableList::LinkDirectionModel;
   *   QComboBox *cb = new QComboBox;
   *   cb->setModel(new LinkDirectionModel(this, db));
   *   cb->setModelColumn(1);  // Will display name field
   * \endcode
   */
  class LinkDirectionModel : public QSqlQueryModel
  {
   public:

    /*! \brief Constructor
     */
    LinkDirectionModel(QObject *parent, const QSqlDatabase & db);

    /*! \brief Constructor
     */
    LinkDirectionModel(const QSqlDatabase & db);

    // Copy disabled
    LinkDirectionModel(const LinkDirectionModel &) = delete;
    LinkDirectionModel & operator=(const LinkDirectionModel &) = delete;

    /*! \brief Set locale
     *
     * Will choose name field that matches
     *  locale. If not available, english name is choosen.
     */
    void setLocale(const QLocale & locale);

    /*! \brief Get row that contains given direction
     *
     * \return Row of searched direction or -1 if not found.
     */
    int row(LinkDirectionType d);

    /*! \brief Get row that contains given direction
     *
     * \return Row of searched direction or -1 if not found.
     */
    int row(LinkDirectionPk pk);

    /*! \brief Get direction PK for given row
     *
     * If row is < 0, a null PK is returned (lastError() is not updated).
     * If row >= 0, and a error occured, a null PK is returned and lastError() contains error.
     * If all works fine, requested PK is returned.
     */
    LinkDirectionPk directionPk(int row);

    /*! \brief Get direction PK of given combo box current index
     *
     * \sa directionPk()
     */
    LinkDirectionPk currentDirectionPk(QComboBox *cb);

    /*! \brief Get picture in ASCII for given row (PictureAscii field)
     */
    QString pictureAscii(int row);

    /*! \brief Set link type
     *
     * Will filter available directions regarding given link type.
     *  F.ex. a cable link can only be bidirectional, but a diode
     *  is unidirectional (start to end or end to start)
     *
     * If a Undefined link type is given,
     *  the filter will be reset, and all directions avaliable.
     */
    void setLinkType(LinkType lt);

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
    QString pvBaseSql;
    QSqlDatabase pvDatabase;
    mdtError pvLastError;

  };

}} // namespace Mdt{ namespace CableList{

#endif // #ifndef MDT_CABLE_LIST_LINK_DIRECTION_MODEL_H

