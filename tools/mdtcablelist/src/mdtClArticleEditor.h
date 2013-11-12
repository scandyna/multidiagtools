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
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include "mdtSqlFormWindow.h"
#include "mdtClEditor.h"

/*! \brief Cable list's article editor
 */
class mdtClArticleEditor : public mdtClEditor
{
 Q_OBJECT

 public:

  /*! \brief Contruct a article editor
   */
  mdtClArticleEditor(QObject *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClArticleEditor();

 private slots:

  /*! \brief Add component
   */
  void addComponent();

  /*! \brief Edit a component
   */
  void editComponent();

  /*! \brief Edit component for given index
   */
  void editComponent(const QModelIndex &index);

  /*! \brief Remove components
   */
  void removeComponents();

  /*! \brief Add link
   */
  void addLink();

  /*! \brief Edit link
   */
  void editLink();

  /*! \brief Remove links
   */
  void removeLinks();

 private:

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Get current Article ID
   *
   * Will return current ID from Article table.
   */
  QVariant currentArticleId();

  /*! \brief Setup Article table
   */
  bool setupArticleTable();

  /*! \brief Setup Article-Component table
   */
  bool setupArticleComponentTable();

  /*! \brief Setup Article-Component usage table
   */
  bool setupArticleUsedByTable();

  /*! \brief Setup Article-Connection table
   */
  bool setupArticleConnectionTable();

  /*! \brief Setup Article-Link table
   */
  bool setupArticleLinkTable();

  Q_DISABLE_COPY(mdtClArticleEditor);
};

#endif  // #ifndef MDT_CL_ARTICLE_EDITOR_H
