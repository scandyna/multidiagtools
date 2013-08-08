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
#ifndef MDT_CL_ARTICLE_LINK_DIALOG_H
#define MDT_CL_ARTICLE_LINK_DIALOG_H

#include "ui_mdtClArticleLinkDialog.h"
#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>

class QSqlQueryModel;
class QComboBox;

/*! \brief Dialog that let the user edit a article link
 */
class mdtClArticleLinkDialog : public QDialog, public Ui::mdtClArticleLinkDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClArticleLinkDialog(QWidget *parent, QSqlDatabase db, QVariant articleId);

  /*! \brief Destructor
   */
  ~mdtClArticleLinkDialog();

  /*! \brief
   */
  void setLinkTypeCode(const QVariant & code);

  /*! \brief
   */
  QVariant linkTypeCode() const;

  /*! \brief
   */
  void setLinkDirectionCode(const QVariant & code);

  /*! \brief
   */
  QVariant linkDirectionCode() const;

  /*! \brief
   */
  void setValue(const QVariant & value);

  /*! \brief
   */
  QVariant value() const;

  /*! \brief
   */
  void setStartConnectionId(const QVariant & id);

  /*! \brief
   */
  QVariant startConnectionId() const;

  /*! \brief
   */
  void setEndConnectionId(const QVariant & id);

  /*! \brief
   */
  QVariant endConnectionId() const;

 private slots:

  /*! \brief
   */
  void onCbLinkTypeCurrentIndexChanged(int row);

  /*! \brief
   */
  void onCbLinkDirectionCurrentIndexChanged(int row);

  /*! \brief
   */
  void selectStartConnection();

  /*! \brief
   */
  void selectEndConnection();

 private:

  Q_DISABLE_COPY(mdtClArticleLinkDialog);

  QVariant pvLinkTypeCode;
  QVariant pvLinkDirectionCode;
  double pvValue;
  QVariant pvStartConnectionId;
  QVariant pvEndConnectionId;
  QSqlDatabase pvDatabase;
  QSqlQueryModel *pvLinkTypeModel;
  QSqlQueryModel *pvLinkDirectionModel;
  QSqlQueryModel *pvArticleConnectionModel;
};

#endif  // #ifndef MDT_CL_ARTICLE_LINK_DIALOG_H
