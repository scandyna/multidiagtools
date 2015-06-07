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
#ifndef MDT_CL_ARTICLE_CONNECTION_DIALOG_H
#define MDT_CL_ARTICLE_CONNECTION_DIALOG_H

#include "mdtError.h"
#include "ui_mdtClArticleConnectionDialog.h"
#include "mdtClArticleConnectionData.h"
#include "mdtClConnectionTypeData.h"
#include <QDialog>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlDatabase>

class QWidget;

/*! \brief Dialog for article connection edition
 */
class mdtClArticleConnectionDialog : public QDialog, Ui::mdtClArticleConnectionDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClArticleConnectionDialog(QWidget *parent, const QSqlDatabase & db);

  /*! \brief Destructor
   */
  ~mdtClArticleConnectionDialog();

  /*! \brief Set connection data
   *
   * \pre data's key must have a article ID set
   */
  void setData(const mdtClArticleConnectionData & data);

  /*! \brief Get connection data
   */
  mdtClArticleConnectionData data() const;

 private slots:

  /*! \brief Select article connector
   */
  void selectArticleConnector();

  /*! \brief Make some checks and accept dialog if ok
   */
  void accept();

 private:

  /*! \brief Set connection type combobox current value
   */
  void setCurrentConnectionType(const QString & type);

  /*! \brief Set connection type combobox current value
   */
  void setCurrentConnectionType(const mdtClConnectionTypeKeyData & type);

  /*! \brief Display data conetent to dialog's widget
   */
  void updateDialog();

  /*! \brief Update data from dialog's widgets
   */
  void updateData();

  /*! \brief Check data
   */
  bool checkData();

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtClArticleConnectionDialog);

  mdtClArticleConnectionData pvData;
  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDT_CL_ARTICLE_CONNECTION_DIALOG_H
