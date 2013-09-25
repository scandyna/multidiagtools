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
#ifndef MDT_PORT_STATUS_WIDGET_H
#define MDT_PORT_STATUS_WIDGET_H

#include <QWidget>
#include <QString>
#include <QGridLayout>
#include "mdtBlinkLed.h"
#include "mdtPortThread.h"

class QLabel;
class QPushButton;
class QTimer;
class QMessageBox;

/*! \brief Helper class to display port/device state and messages
 *
 * This widget contains 4 parts:
 *  - A push button that opens a message box with details
 *  - A status region with a LED and a text area
 *  - A custom widget place holder
 *  - A fixed text area (typically usefull to display wich port is open, f.ex.)
 */
class mdtPortStatusWidget : public QWidget
{
  Q_OBJECT

 public:

  /*! \brief Build a status widget with default texts
   */
  mdtPortStatusWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtPortStatusWidget();

  /*! \brief Enable the TX/RX LEDs
   *
   * \pre txThread and rxThread must be valid.
   */
  void enableTxRxLeds(mdtPortThread *txThread, mdtPortThread *rxThread);

  /*! \brief Disable the TX/RX LED's
   */
  void disableTxRxLeds();

  /*! \brief Add a custom widget
   *
   * One custom widget can be added.
   */
  void addCustomWidget(QWidget *widget);

  /*! \brief Remove the custom widget
   *
   * \see addCustomWidget().
   */
  void removeCustomWidget();

  /*! \brief Set the permanent text
   *
   * Note: permanent text is allways visible
   */
  void setPermanentText(const QString &text);

  /*! \brief Clear current message
   *
   * Clear message showed with showMessage()
   *
   * After calling this method, netx call of setState() will update text.
   */
  void clearMessage();

 public slots:

  /*! \brief Set state
   *
   * States are defined in mdtPortManager
   */
  void setState(int id, const QString & text, int ledColorId, bool ledIsOn);

  /*! \brief Used to show a message and details
   *
   * Note: calling this slot with a empty message has the same effect than clearMessage() .
   *
   * \param message Message to show.
   * \param details Details to show
   * \param timeout If > 0, message will be cleared after timeout [ms]
   */
  void showMessage(const QString &message, const QString &details, int timeout = 0);

 private slots:

  /*! \brief Trig the TX led for 100 ms
   *
   * \pre TX led must be set (see enableTxRxLeds)
   */
  void trigTxLed();

  /*! \brief Trig the RX led for 100 ms
   *
   * \pre RX led must be set (see enableTxRxLeds)
   */
  void trigRxLed();

  /*! \brief Re-display state text after timeout
   *
   * Used by showMessage()
   */
  void backToStateText();

 private:

  Q_DISABLE_COPY(mdtPortStatusWidget);

  QGridLayout *pvLayout;
  QWidget *pvCustomWidget;
  mdtBlinkLed *ldState;
  QPushButton *pbDetails;
  QMessageBox *mbDetails;
  QLabel *lbMessage;
  QLabel *lbPermanentTextLabel;
  QLabel *lbTx;
  QLabel *lbRx;
  mdtBlinkLed *ldTx;
  mdtBlinkLed *ldRx;
  // Status text
  QString pvCurrentStateText;
  QTimer *pvBackToStateTextTimer;
  bool pvShowingMessage;  // Set by showMessage()
  // Used by disableTxRxLeds()
  mdtPortThread *pvTxThread;
  mdtPortThread *pvRxThread;
};

#endif  // #ifndef MDT_PORT_STATUS_WIDGET_H
