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
#ifndef MDT_CL_ARTICLE_EDITOR_H
#define MDT_CL_ARTICLE_EDITOR_H

#include <QSqlDatabase>
#include <QObject>
#include <QVariant>
#include "mdtSqlFormWindow.h"

/*! \brief Cable list's article editor
 */
class mdtClArticleEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Contruct a article editor
   */
  mdtClArticleEditor(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClArticleEditor();

  /*! \brief Setup tables
   *
   * \param includeConnections If true, ArticleConnection_tbl will be added as child widget.
   */
  bool setupTables(bool includeConnections);

  /*! \brief Get form object
   */
  mdtSqlFormWindow *form();

 private slots:

  /*! \brief Add link
   */
  void addLink();

 private:

  /*! \brief Get current Article ID
   *
   * Will return current ID from Article table.
   */
  QVariant currentArticleId();

  /*! \brief Setup Article table
   */
  bool setupArticleTable();

  /*! \brief Setup Article-Connection table
   */
  bool setupArticleConnectionTable();

  /*! \brief Setup Article-Link table
   */
  bool setupArticleLinkTable();

  Q_DISABLE_COPY(mdtClArticleEditor);

  QSqlDatabase pvDatabase;
  mdtSqlFormWindow *pvForm;
};

#endif  // #ifndef MDT_CL_ARTICLE_EDITOR_H
