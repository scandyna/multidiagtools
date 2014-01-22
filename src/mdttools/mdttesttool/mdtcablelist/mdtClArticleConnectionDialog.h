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
#ifndef MDT_CL_ARTICLE_CONNECTION_DIALOG_H
#define MDT_CL_ARTICLE_CONNECTION_DIALOG_H

///#include "mdtClArticleConnectionData.h"
#include "ui_mdtClArticleConnectionDialog.h"
#include "mdtSqlRecord.h"
#include <QDialog>
#include <QVariant>
#include <QSqlRecord>

class QWidget;
class QSqlQueryModel;

/*! \brief Dialog for article connection edition
 */
class mdtClArticleConnectionDialog : public QDialog, Ui::mdtClArticleConnectionDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClArticleConnectionDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtClArticleConnectionDialog();

  /*! \brief Set model for article connector model
   *
   * \todo replace model by a SQL query (handle model internally..)
   */
  void setArticleConnectorModel(QSqlQueryModel *model);

  /*! \brief Set connection data
   */
  ///void setData(const mdtClArticleConnectionData & data);

  /*! \brief Set connection data
   *
   * \pre Following fields must exit in data:
   *       - ArticleContactName
   *       - IoType
   *       - FunctionEN
   *       - FunctionDE
   *       - FunctionFR
   *       - FunctionIT
   */
  void setData(const QSqlRecord & data);

  /*! \brief Get connection data
   */
  ///mdtClArticleConnectionData data() const;
  mdtSqlRecord data() const;

 private slots:

  /*! \brief Select article connector
   */
  void selectArticleConnector();

  /*! \brief Make some checks and accept dialog if ok
   */
  void accept();

 private:

  Q_DISABLE_COPY(mdtClArticleConnectionDialog);

  ///mdtClArticleConnectionData pvData;
  mdtSqlRecord pvData;
  QSqlQueryModel *pvArticleConnectorModel;
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_DIALOG_H
