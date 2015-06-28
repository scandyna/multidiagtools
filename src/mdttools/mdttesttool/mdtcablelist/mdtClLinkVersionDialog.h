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
#ifndef MDT_CL_LINK_VERSION_DIALOG_H
#define MDT_CL_LINK_VERSION_DIALOG_H

#include "ui_mdtClLinkVersionDialog.h"
#include "mdtClLinkVersionData.h"
#include <QDialog>

/*! \brief Dialog for link version edition
 *
 * Refers to LinkVersion_tbl
 */
class mdtClLinkVersionDialog : public QDialog, Ui::mdtClLinkVersionDialog
{
 public:

  /*! \brief Constructor
   */
  mdtClLinkVersionDialog(QWidget *parent = nullptr);

  /*! \brief Set last link version data
   */
  void setLastVersionData(mdtClLinkVersionData data);

  /*! \brief Set link version data
   */
  void setVersionData(const mdtClLinkVersionData & data);

  /*! \brief Get link version data
   *
   * Note: if dialog was rejected, data can be not cocistent
   */
  mdtClLinkVersionData versionData() const
  {
    return pvVersionData;
  }

  /*! \brief Set version data and accept dialog
   */
  void accept();

 private:

  Q_DISABLE_COPY(mdtClLinkVersionDialog);

  mdtClLinkVersionData pvVersionData;
};

#endif // #ifndef MDT_CL_LINK_VERSION_DIALOG_H
