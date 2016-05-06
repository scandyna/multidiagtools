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
#ifndef MDT_CL_ARTICLE_LINK_DIALOG_H
#define MDT_CL_ARTICLE_LINK_DIALOG_H

#include "ui_mdtClArticleLinkDialog.h"
#include "mdtClArticleLinkData.h"

#include "mdtClLinkDirectionData.h"

#include <QDialog>
#include <QVariant>
#include <QSqlDatabase>

/// \todo Update once migrated
namespace Mdt{ namespace CableList{
  class LinkTypeModel;
}}

class mdtClLinkTypeModel;
class mdtClLinkDirectionModel;
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

  /*! \brief Lock or unlock connections edition
   */
  void setConnectionEditionLocked(bool lock);

  /*! \brief Set link type
   */
  void setLinkType(LinkType type);

  /*! \brief Set link direction
   */
  void setLinkDirection(mdtClLinkDirection_t d);

  /*! \brief Display value
   */
  void setValue(const QVariant & value);

  /*! \brief Set article link data
   *
   * Will store display given link data and related connections
   */
  void setLinkData(const mdtClArticleLinkData & data);

//   /*! \brief Store key as current selected start connection and display it
//    */
//   void setStartConnection(const mdtClArticleConnectionKeyData & fk);
// 
//   /*! \brief Store key as current selected end connection and display it
//    */
//   void setEndConnection(const mdtClArticleConnectionKeyData & fk);

  /*! \brief Get link data
   */
  mdtClArticleLinkData linkData() const
  {
    return pvLinkData;
  }

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

  /*! \brief Display current selected start connection
   */
  void displayCurrentSelectedStartConnection();

  /*! \brief Display current selected end connection
   */
  void displayCurrentSelectedEndConnection();

  Q_DISABLE_COPY(mdtClArticleLinkDialog);

  mdtClArticleLinkData pvLinkData;
  QSqlDatabase pvDatabase;
  QVariant pvArticleId;
  Mdt::CableList::LinkTypeModel *pvLinkTypeModel; /// \todo update once migrated
  mdtClLinkDirectionModel *pvLinkDirectionModel;
  QSqlQueryModel *pvArticleConnectionModel;
};

#endif  // #ifndef MDT_CL_ARTICLE_LINK_DIALOG_H
