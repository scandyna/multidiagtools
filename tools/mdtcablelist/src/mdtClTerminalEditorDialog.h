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
#ifndef MDT_CL_TERMINAL_EDITOR_DIALOG_H
#define MDT_CL_TERMINAL_EDITOR_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QVariant>

class QLineEdit;
class QLabel;
class QPushButton;
class QTableView;
class QDialogButtonBox;

/*! \brief Dialog used to edit a terminal
 */
class mdtClTerminalEditorDialog : public QDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClTerminalEditorDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtClTerminalEditorDialog();

  /*! \brief Set the terminal block name
   */
  void setTerminalBlockName(const QString &name);

  /*! \brief Get connector name
   *
   * \return Connector name or invalid QVariant if dialog was rejected.
   */
  QVariant connectorName() const;

  /*! \brief Get contact name
   *
   * \return Contact name or invalid QVariant if dialog was rejected.
   */
  QVariant contactName() const;

 public slots:

  /*! \brief Overloads QDialog
   *
   * Will store results and call QDialog::accept()
   */
  void accept();

  /*! \brief Overloads QDialog
   */
  void reject();

 private:

  Q_DISABLE_COPY(mdtClTerminalEditorDialog);

  // Terminal block
  QLineEdit *leTerminalBlock;
  QLabel *lbTerminalBlock;
  // Connector
  QLabel *lbConnectorName;
  QLineEdit *leConnectorName;
  QVariant pvConnectorName;
  QLabel *lbContactName;
  QLineEdit *leContactName;
  QVariant pvContactName;
  // Terminal ranges
  QTableView *tvRanges;
  QPushButton *pbAddRange;
  QPushButton *pbRemoveRange;
  // Dialog buttons
  QDialogButtonBox *pvDialogButtons;
  
};

#endif  // #ifndef MDT_CL_TERMINAL_EDITOR_DIALOG_H
