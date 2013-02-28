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
#ifndef MDT_ABSTRACT_PORT_SETUP_DIALOG_H
#define MDT_ABSTRACT_PORT_SETUP_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QAbstractButton>
#include <QString>
#include "ui_mdtAbstractPortSetupDialog.h"
#include "mdtPortManager.h"

class mdtPortStatusWidget;

/*! \brief Base class to build a port setup dialog
 *
 * The first step before display a setup dialog is to set the port manager
 * with setPortManager()
 */
class mdtAbstractPortSetupDialog : public QDialog, public Ui::mdtAbstractPortSetupDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtAbstractPortSetupDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtAbstractPortSetupDialog();

  /*! \brief Set the port manager
   *
   * Port manager's stateChanged() signal will be connected
   *  to setStateFromPortManager() slot.
   *
   * \pre portManager must be a valid pointer.
   */
  void setPortManager(mdtPortManager *portManager);

  /*! \brief Get port manager
   *
   * \pre A valid port manager must be set with setPortManager() befor calling this method.
   */
  mdtPortManager *portManager();

 protected:

  /*! \brief Called by setPortManager()
   *
   * Defaul implementation does nothing
   */
  virtual void portManagerSet();

  /*! \brief Set the Disconnected state
   *
   * This method must be implemented in subclass.
   *  Goal is to keep GUI coherent to current state
   *  (f.ex. disable/enable some buttons).
   */
  virtual void setStateDisconnected() = 0;

  /*! \brief Set the Connecting state
   *
   * This method must be implemented in subclass.
   *  Goal is to keep GUI coherent to current state
   *  (f.ex. disable/enable some buttons).
   */
  virtual void setStateConnecting() = 0;

  /*! \brief Set the Ready state
   *
   * This method must be implemented in subclass.
   *  Goal is to keep GUI coherent to current state
   *  (f.ex. disable/enable some buttons).
   */
  virtual void setStateReady() = 0;

  /*! \brief Set the Busy state
   *
   * This method must be implemented in subclass.
   *  Goal is to keep GUI coherent to current state
   *  (f.ex. disable/enable some buttons).
   */
  virtual void setStateBusy() = 0;

  /*! \brief Set the Warning state
   *
   * This method must be implemented in subclass.
   *  Goal is to keep GUI coherent to current state
   *  (f.ex. disable/enable some buttons).
   */
  virtual void setStateWarning() = 0;

  /*! \brief Set the Error state
   *
   * This method must be implemented in subclass.
   *  Goal is to keep GUI coherent to current state
   *  (f.ex. disable/enable some buttons).
   */
  virtual void setStateError() = 0;

  /*! \brief Apply setup
   *
   * This method must be implemented in subclass.
   *
   * When Ok or Apply button was clicked, this method
   *  will be called. Case of Ok button, if this method
   *  returns false, the setup dialog will not be closed.
   */
  virtual bool applySetup() = 0;

  /*! \brief Set the header widget
   *
   * The header widget is on top of setup dialog.
   *
   * \pre w must be a valid pointer.
   */
  void setHeaderWidget(QWidget *w);

  /*! \brief Add a widget to the options tab
   *
   * Options tab is a QTabWidget
   */
  void addOptionsTabWidget(QWidget *w, const QString &label);

  /*! \brief Used to show a message in status widget
   *
   * \param message Message to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showStatusMessage(const QString &message, int timeout = 0);

  /*! \brief Used to show a message and details in status widget
   *
   * \param message Message to show
   * \param details Details to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showStatusMessage(const QString &message, const QString &details, int timeout = 0);

  /*! \brief Enable/disable Ok button
   */
  void setOkButtonEnabled(bool enabled);

  /*! \brief Enable/disable Apply button
   */
  void setApplyButtonEnabled(bool enabled);

  /*! \brief Enable/disable Cancel button
   */
  void setCancelButtonEnabled(bool enabled);

 private slots:

  /*! \brief Update internal state regarding port manager's state
   */
  void setStateFromPortManager(int state);

  /*! \brief Called when Ok, Apply or cancel button is clicked
   */
  void on_buttonBox_clicked(QAbstractButton *button);

 private:

  mdtPortManager *pvPortManager;
  mdtPortStatusWidget *pvStatusWidget;
  QWidget *wHeader;
  Q_DISABLE_COPY(mdtAbstractPortSetupDialog);
};

#endif  // #ifndef MDT_ABSTRACT_PORT_SETUP_DIALOG_H
