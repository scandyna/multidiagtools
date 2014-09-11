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
#ifndef MDT_UI_MESSAGE_HANDLER_H
#define MDT_UI_MESSAGE_HANDLER_H

#include "mdtError.h"
#include <QString>

class QWidget;

/*! \brief Helper class for interaction with the user
 */
class mdtUiMessageHandler
{
 public:

  /*! \brief Message type
   */
  enum Type_t {
                Information = 0,  /*!< Message is a information */
                Question,         /*!< Message is a question */
                Warning,          /*!< Message is a warning */
                Error             /*!< Message is a error */
               };

  /*! \brief Constructor
   *
   * \param parent Parent widget for dialog
   */
  mdtUiMessageHandler(QWidget *parent = 0) : pvParentWidget(parent) {}

  /*! \brief Set type
   */
  void setType(Type_t t) { pvType = t; }

  /*! \brief Get type
   */
  inline Type_t type() const { return pvType; }

  /*! \brief Set text
   */
  void setText(const QString & txt) { pvText = txt; }

  /*! \brief Get text
   */
  inline QString text() const { return pvText; }

  /*! \brief Set informative text
   */
  void setInformativeText(const QString & txt) { pvInformativeText = txt; }

  /*! \brief Get informative text
   */
  inline QString informativeText() const { return pvInformativeText; }

  /*! \brief Set detailed text
   */
  void setDetailedText(const QString & txt) { pvDetailedText = txt; }

  /*! \brief Get detailed text
   */
  inline QString detailedText() const { return pvDetailedText; }

  /*! \brief Set error
   *
   * Will also set text, informativeText, detailedText and type.
   */
  void setError(const mdtError & err);

  /*! \brief Display message to the user
   */
  virtual void displayToUser();

  /*! \brief Prompt the user to do a action
   *
   * \return True if user accepted the message box (typically if he clicked Ok, Yes, ..),
   *          false if he rejected it (typically if Cancel, No, ... was clicked).
   */
  virtual bool promptUser();

  /*! \brief Ask a question to the user
   *
   * \return True if user accepted the message box (typically if he clicked Ok, Yes, ..),
   *          false if he rejected it (typically if Cancel, No, ... was clicked).
   */
  virtual bool askUser();

 private:

  Type_t pvType;
  QString pvText;
  QString pvInformativeText;
  QString pvDetailedText;
  QWidget *pvParentWidget;
};

#endif // MDT_UI_MESSAGE_HANDLER_H
