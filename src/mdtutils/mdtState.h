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
#ifndef MDT_STATE_H
#define MDT_STATE_H

#include <QState>
#include <QString>

/*! \brief Representation of a state in mdtStateMachine
 *
 * This class is based on QState, and add some properties.
 *
 * \sa mdtStateMachine
 */
class mdtState : public QState
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Will set given id, let a empty text and set notifyEnteredToUi flag false.
   */
  mdtState(int id, QState *parent = 0);

  /*! \brief Destructor
   */
  ~mdtState();

  /*! \brief Set state ID
   *
   * \sa id()
   */
  void setId(int id);

  /*! \brief Get ID
   *
   * The id can typically be used with enums (STATE_READY, ...)
   */
  int id() const;

  /*! \brief Set text of the state
   */
  void setText(const QString & text);

  /*! \brief Get text of the state
   */
  QString text() const;

  /*! \brief Set LED color ID
   *
   * \sa ledColorId()
   */
  void setLedColorId(int colorId);

  /*! \brief Get LED color ID
   *
   * The color ID can, for example, be used with mdtLed color enum.
   *  It was choosen store this id in int format,
   *  because mdtLed is in mdtUtilsGui package (we not want to depend on it).
   */
  int ledColorId() const;

  /*! \brief Set LED ON state
   */
  void setLedOn(bool on);

  /*! \brief Get LED ON state
   */
  bool ledIsOn() const;

  /*! \brief Set notifyEnteredToUi flag
   *
   * \sa notifyEnteredToUi()
   */
  void setNotifyEnteredToUi(bool notify);

  /*! \brief Get notifyEnteredToUi flag
   *
   * If notifyEnteredToUi is true,
   *  mdtStateMachine will emit stateChangedForUi() signal each time
   *  this state was entered.
   */
  bool notifyEnteredToUi() const;

 signals:

  /*! \brief Emited each time state was enterd
   *
   * Note: this signal is used by mdtStateMachine and should not be used directly.
   */
  void entered(mdtState *state);

 private slots:

  /*! \brief Used to send entered() signal with this as argument
   */
  void onStateEntered();

 private:

  Q_DISABLE_COPY(mdtState);

  int pvId;
  QString pvText;
  int pvLedColorId;
  bool pvLedIsOn;
  bool pvNotifyEnteredToUi;
};

#endif
