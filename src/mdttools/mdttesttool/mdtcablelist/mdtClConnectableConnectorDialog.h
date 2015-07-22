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
#ifndef MDT_CL_CONNECTABLE_CONNECTOR_DIALOG_H
#define MDT_CL_CONNECTABLE_CONNECTOR_DIALOG_H

#include "ui_mdtClConnectableConnectorDialog.h"
#include "mdtClConnectableCriteria.h"
#include <QDialog>
#include <QVariant>
#include <QString>
#include <QList>
#include <QSqlDatabase>

class mdtError;
class mdtClConnectableCriteria;
class QWidget;

/*! \brief Dialog that let the user choose a connector that is connectable to start connector
 */
class mdtClConnectableConnectorDialog : public QDialog, Ui::mdtClConnectableConnectorDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClConnectableConnectorDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Set start connector
   */
  void setStartConnector(const QVariant & unitConnectorId);

  /*! \brief Set start connector label
   */
  void setStartConnectorLabel(const QString & text);

  /*! \brief Set end unit ID
   */
  void setEndUnitId(const QVariant & unitId);

  /*! \brief Set end unit ID list
   */
  void setEndUnitIdList(const QList<QVariant> & unitIdList);

  /*! \brief Set end connector label
   */
  void setEndConnectorLabel(const QString & text);

  /*! \brief Get end unit connector ID
   */
  QVariant endUnitConnectorId() const
  {
    return pvEndUnitConnectorId;
  }

  /*! \brief Get end connector name
   */
  QString endUnitConnectorName() const;

  /*! \brief Set connectable criteria
   */
  void setConnectableCriteria(const mdtClConnectableCriteria & c);

  /*! \brief Get connectable criteria
   */
  mdtClConnectableCriteria getConnectableCriteria() const;

 private slots:

  /*! \brief Select end connector
   */
  void selectEndConnector();

 private:

  /*! \brief Clear start connector widgets
   */
  void clearStartConnectorWidgets();

  /*! \brief Display start connector
   */
  void displayStartConnector();

  /*! \brief Clear end connector widgets
   */
  void clearEndConnectorWidgets();

  /*! \brief Display end connector
   */
  void displayEndConnector();

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtClConnectableConnectorDialog);

  QSqlDatabase pvDatabase;
  QVariant pvStartUnitConnectorId;
  QVariant pvEndUnitConnectorId;
  QList<QVariant> pvEndUnitIdList;
};

#endif // #ifndef MDT_CL_CONNECTABLE_CONNECTOR_DIALOG_H
