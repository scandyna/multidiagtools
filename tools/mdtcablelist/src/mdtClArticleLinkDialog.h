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

  /*! \brief Display link type in combobox
   */
  void setLinkTypeCode(const QVariant & code);

  /*! \brief Get selected link type code
   */
  QVariant linkTypeCode() const;

  /*! \brief Display link direction in combobox
   */
  void setLinkDirectionCode(const QVariant & code);

  /*! \brief Get selected link direction code
   */
  QVariant linkDirectionCode() const;

  /*! \brief Display value
   */
  void setValue(const QVariant & value);

  /*! \brief Get value
   */
  QVariant value() const;

  /*! \brief
   */
  void setStartConnectionId(const QVariant & id);

  /*! \brief Get start connection ID
   */
  QVariant startConnectionId() const;

  /*! \brief
   */
  void setEndConnectionId(const QVariant & id);

  /*! \brief Get end connection ID
   */
  QVariant endConnectionId() const;

 private slots:

  /*! \brief Update some things when link type combobox index changed
   */
  void onCbLinkTypeCurrentIndexChanged(int row);

  /*! \brief Update some things when link direction combobox index changed
   */
  void onCbLinkDirectionCurrentIndexChanged(int row);

  /*! \brief Call selection dialog for start connection
   */
  void selectStartConnection();

  /*! \brief Call selection dialog for end connection
   */
  void selectEndConnection();

  /*! \brief Do some check, store values and accept dialog if Ok
   */
  void accept();

  /*! \brief Clear values and reject dialog
   */
  void reject();

 private:

  /*! \brief Store code of current selected link type
   */
  void storeCurrentSelectedLinkType();

  /*! \brief Store code of current selected link direction
   */
  void storeCurrentSelectedLinkDirection();

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
