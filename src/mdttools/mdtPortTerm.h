/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_PORT_TERM_H
#define MDT_PORT_TERM_H

#include "ui_mdtPortTerm.h"
#include "mdtPortManager.h"
#include "mdtSerialPortManager.h"
#include "mdtSerialPortSetupDialog.h"
#include "mdtSerialPortCtlWidget.h"
#include <QWidget>
#include <QMainWindow>
#include <QLocale>
#include <QEvent>
#include <QMap>
#include <QString>
#include <QActionGroup>

/*! \brief Mini port treminal
 */
class mdtPortTerm : public QMainWindow, public Ui::mdtPortTerm
{
 Q_OBJECT

 public:

  mdtPortTerm(QWidget *parent = 0);
  ~mdtPortTerm();

  /*! \brief Build the translations menu
   */
  void setAvailableTranslations(QMap<QString, QString> &avaliableTranslations, const QString &currentTranslationKey);

 public slots:

  /*! \brief Append incomming data to terminal
   * 
   * Whenn called, data will be token from active port manager
   */
  void appendReadenData();

  /*! \brief Send command to port
   */
  void sendCmd();

  /*! \brief Abort command transmission
   */
  void on_pbSendCmdAbort_clicked();

  /*! \brief Clear terminal
   */
  void on_pbClearTerm_clicked();

  void retranslate();

 private slots:

  // Set/unset port type to serial port
  void attachToSerialPort();
  void detachFromSerialPort();

  // Call serial port setup dialog
  void serialPortSetup();
  
  // Change lagugage
  void changeLanguage();
  
 signals:
  
  void languageChanged(const QLocale &locale);
  
 private:

  // Diseable copy
  mdtPortTerm(mdtPortTerm &other);
  
  // Serial port
  mdtSerialPortManager *pvSerialPortManager;
  mdtSerialPortCtlWidget *pvSerialPortCtlWidget;
  // Current port manager
  mdtPortManager *pvCurrentPortManager;
  // Translations menu
  QActionGroup *pvLanguageActionGroup;
};

#endif  // #ifndef MDT_PORT_TERM_H
