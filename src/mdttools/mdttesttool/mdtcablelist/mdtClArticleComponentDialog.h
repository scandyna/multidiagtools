/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_CL_ARTICLE_COMPONENT_DIALOG_H
#define MDT_CL_ARTICLE_COMPONENT_DIALOG_H

#include "ui_mdtClArticleComponentDialog.h"
#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>

class QWidget;
class QSqlQueryModel;

/*! \brief
 */
class mdtClArticleComponentDialog : public QDialog, Ui::mdtClArticleComponentDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClArticleComponentDialog(QWidget *parent, QSqlDatabase db, QVariant articleId, QVariant componentId = QVariant());

  /*! \brief Destructor
   */
  ~mdtClArticleComponentDialog();

  /*! \brief Get composite ID
   */
  const QVariant &articleId() const;

  /*! \brief Get component ID
   */
  const QVariant &componentId() const;

  /*! \brief Get qty of component
   */
  const QVariant qty() const;

  /*! \brief Get qty unit
   */
  const QVariant qtyUnit() const;

 private slots:

  /*! \brief Display a dialog to select component and update if dialog is accepted
   */
  void selectComponent();

 private:

  /*! \brief Display current component
   */
  void displayCurrentComponent();

  QSqlDatabase pvDatabase;
  QSqlQueryModel *pvArticleComponentModel;
  QVariant pvArticleId;   // Compsite article ID
  QVariant pvComponentId; // Comonent article ID
};

#endif  // #ifndef MDT_CL_ARTICLE_COMPONENT_DIALOG_H
