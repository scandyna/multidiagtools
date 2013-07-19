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
#ifndef MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H
#define MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QObject>
#include <QVariant>

class QWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;
class QDialogButtonBox;

/*! \brief Display units and their connections and let the user choose
 */
class mdtClUnitConnectionSelectionDialog : public QDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClUnitConnectionSelectionDialog(QWidget *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClUnitConnectionSelectionDialog();

  /*! \brief Get the selected connection ID
   */
  QVariant selectedConnectionId() const;

  /*! \brief Get the selected connector name
   */
  QVariant selectedConnectorName() const;

  /*! \brief Get the selected contact name
   */
  QVariant selectedContactName() const;

  /*! \brief Get the selected schema position
   */
  QVariant selectedSchemaPosition() const;

 public slots:

  /*! \brief Overloads QDialog to store result
   */
  void accept();

  /*! \brief Overloads QDialog to tell selectionResult() that it must return a empty result
   */
  void reject();

 private:

  Q_DISABLE_COPY(mdtClUnitConnectionSelectionDialog);

  QSqlDatabase pvDatabase;
  mdtSqlTableWidget *pvUnitWidget;
  mdtSqlTableWidget *pvUnitConnectionWidget;
  QSqlTableModel *pvUnitModel;
  QSqlTableModel *pvUnitConnectionModel;
  mdtSqlRelation *pvUnitConnectionRelation;
  QDialogButtonBox *pvButtons;
  QVariant pvSelectedConnectionId;
  QVariant pvSelectedConnectorName;
  QVariant pvSelectedContactName;
  QVariant pvSelectedShemaPosition;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H
