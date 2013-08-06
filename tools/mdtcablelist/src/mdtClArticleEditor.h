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
#include "mdtSqlFormWindow.h"

class mdtSqlWindow;
class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;

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

  /*! \brief Setup GUI
   *
   * Will dispose internal widgets into window
   *
   * \pre window must be a valid pointer
   */
  ///void setupUi(mdtSqlWindow *window);

 private:

  /*! \brief Get current Article ID
   *
   * Will return current ID from Unit table.
   *  Returns a value < 0 on error (no row, ...)
   */
  int currentArticleId();

  /*! \brief Setup Article table and widget
   */
  bool setupArticleTable();

  /*! \brief Setup Article-Connection table and widget
   */
  bool setupArticleConnectionTable();

  Q_DISABLE_COPY(mdtClArticleEditor);

  QSqlDatabase pvDatabase;
  mdtSqlFormWindow *pvForm;
  ///mdtSqlFormWidget *pvArticleWidget;
  ///mdtSqlTableWidget *pvArticleConnectionWidget;
  ///QSqlTableModel *pvArticleModel;
  ///QSqlTableModel *pvArticleConnectionModel;
  ///mdtSqlRelation *pvArticleConnectionRelation;
};

#endif  // #ifndef MDT_CL_ARTICLE_EDITOR_H
