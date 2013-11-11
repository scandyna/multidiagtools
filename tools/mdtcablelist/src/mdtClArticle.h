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

#include "mdtClBase.h"
#include "mdtClLinkData.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QVariant>
#include <QModelIndex>
#include <QList>

/*! \brief Helper class for Article and related tables edition
 */
class mdtClArticle : public mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtClArticle(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClArticle();

  /*! \brief Get a model with articles for compononent selection
   *
   * Will setup a query model witch contains all articles except articleId
   *  and articles that allready are component of articleID .
   */
  QSqlQueryModel *articleModelForComponentSelection(const QVariant &articleId);

  /*! \brief Add a article as component
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addComponent(const QVariant &articleId, const QVariant &componentId, const QVariant &qty, const QVariant &qtyUnit);

  /*! \brief Edit a component
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool editComponent(const QVariant &articleId, const QVariant &currentComponentId, const QVariant &newComponentId, const QVariant &qty, const QVariant &qtyUnit);

  /*! \brief Remove a single component
   */
  bool removeComponent(const QVariant &articleId, const QVariant &componentId);

  /*! \brief Remove a list of components
   */
  bool removeComponents(const QVariant &articleId, const QList<QVariant> &componentIdList);

  /*! \brief Remove each link that is contained in selection
   *
   * This is usefull used together with mdtSqlTableWidget .
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeComponents(const QVariant &articleId, const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Add a record in Link table
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool addLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, double value, const QVariant & directionCode, const QVariant & typeCode);

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

  /*! \brief Edit a record in Link table
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool editLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId, const mdtClLinkData &data);

  /*! \brief Remove a signle link
   *
   * \return True on success, false else.
   *          To get reason of failure, use lastError() .
   */
  bool removeLink(const QVariant & articleConnectionStartId, const QVariant & articleConnectionEndId);

  /*! \brief Remove each unit link that is contained in selection
   */
  bool removeLinks(const QList<QModelIndexList> &indexListOfSelectedRowsByRows);

 private:

  QSqlQueryModel *pvArticleModel;
};

#endif  // #ifndef MDT_CL_ARTICLE_H
