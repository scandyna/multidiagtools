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
#ifndef MDT_CL_APPLICATION_WIDGETS_H
#define MDT_CL_APPLICATION_WIDGETS_H

#include "mdtError.h"
#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include <QString>
#include <QVariant>
#include <memory>

class mdtSqlWindow;
class mdtSqlForm;
// Editors
class mdtClUnitEditor;

/*! \brief Container for cable list editors and views
 *
 * Using this class helps to create editors and views.
 *  Most of time, only one instance of each editore or view is needed,
 *  but must be accessible from multiple places in application.
 */
class mdtClApplicationWidgets : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Set database to use
   */
  static void setDatabase(QSqlDatabase db)
  {
    instance().pvDatabase = db;
  }

  /*! \brief Get pointer to instance
   *
   * Useful for signal/slot connections
   */
  static mdtClApplicationWidgets *instancePtr()
  {
    return & instance();
  }

  /*! \brief Get used database
   */
  static QSqlDatabase database()
  {
    return instance().pvDatabase;
  }

  /*! \brief Close all open widgets
   *
   * If one widget have unsaved data,
   *  it will not be closed and this function returns false.
   */
  static bool closeOpenWidgets();

  /*! \brief Clear
   *
   * Will destroy all created widgets
   */
  static void clear();

  /*! \brief Edit a specific unit
   */
  static void editUnit(const QVariant & unitId);

  /*! \brief Edit units
   */
  static void editUnits();

 public slots:

  /*! \brief Edit units
   */
  void slotEditUnits();

 private:

  /*! \brief Create Unit editor
   */
  bool createUnitEditor();

  /*! \brief Setup given editor in mdtSqlWindow
   *
   * Returned pointer can be used, for example, to resize the editor.
   *
   * Freshliy created window will be added to open editors list.
   *
   * \pre editor must be a valid pointer.
   */
  std::shared_ptr<mdtSqlWindow> setupEditorInSqlWindow(const std::shared_ptr<mdtSqlForm> & editor);

  /*! \brief Show open window that contains given sql form
   */
  void showSqlWindow(const std::shared_ptr<mdtSqlForm> & form, bool enableNavigation, bool enableEdition);

  /*! \brief Get open window for given sql form
   */
  std::shared_ptr<mdtSqlWindow> getOpenSqlWindow(const std::shared_ptr<mdtSqlForm> & form);

  /*! \brief Close all open widgets
   */
  bool closeAllOpenWidgets();

  /*! \brief Clear all widgets
   */
  void clearAllWidgets();

  /*! \brief Display a error to the user
   */
  void displayError(const mdtError & error);

  /*! \brief Get the (unique) instance of mdtClApplicationWidgets
   */
  static mdtClApplicationWidgets & instance();

  /*! \brief Prevent multiple object instances
   */
  mdtClApplicationWidgets() {}

  // We define no destructer, it will never be called

  Q_DISABLE_COPY(mdtClApplicationWidgets);

  QSqlDatabase pvDatabase;
  QList<std::shared_ptr<mdtSqlWindow> > pvOpenEditorWidows;
  // Editors
  std::shared_ptr<mdtClUnitEditor> pvUnitEditor;
};

#endif // #ifndef MDT_CL_APPLICATION_WIDGETS_H
