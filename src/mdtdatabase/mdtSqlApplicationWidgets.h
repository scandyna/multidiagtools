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
#ifndef MDT_SQL_APPLICATION_WIDGETS_H
#define MDT_SQL_APPLICATION_WIDGETS_H

#include "mdtError.h"
#include "mdtErrorDialog.h"
#include "mdtSqlWindow.h"
#include "mdtSqlForm.h"
#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMessageBox>
#include <QMutableListIterator>
#include <memory>

#include <QDebug>

/*! \brief Container for SQL editors and views
 *
 * Using this class helps to create editors and views.
 *  Most of time, only one instance of each editore or view is needed,
 *  but must be accessible from multiple places in application.
 *
 * \sa mdtSqlApplicationWidgetsGuard
 */
template <typename T>
class mdtSqlApplicationWidgets : public QObject
{
 public:

  /*! \brief Set database to use
   */
  static void setDatabase(QSqlDatabase db)
  {
    instance()->pvDatabase = db;
  }

  /*! \brief Get used database
   */
  static QSqlDatabase database()
  {
    return instance()->pvDatabase;
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

  /*! \brief Get the (unique) instance of (concrete) application widgets
   *
   * Memory allocation is done automatically if needed,
   *  so it is allways valid.
   *
   * Once destrcut() was called, this function will also
   *  return a pointer to a new object.
   *  Take care when storing returned pointer and calling destrcut().
   *  Note also the lifetime of created mdtSqlApplicationWidgetsGuard object,
   *  witch will call destrcut() when it is deleted.
   *
   * \sa mdtSqlApplicationWidgetsGuard
   */
  static T *instance()
  {
    if(pvInstance == nullptr){
      pvInstance = new T;
    }
    return pvInstance;
  }

  /*! \brief Free resource
   *
   * Will delete the instance of application widgets.
   *
   * \sa mdtSqlApplicationWidgetsGuard
   */
  static void destruct()
  {
    delete pvInstance;
  }

 protected:

  /*! \brief Setup given editor in mdtSqlWindow
   *
   * Returned pointer can be used, for example, to resize the editor.
   *
   * Freshliy created window will be added to open editors list.
   *
   * \pre editor must be a valid pointer.
   */
  std::shared_ptr<mdtSqlWindow> setupEditorInSqlWindow(mdtSqlForm * const editor);

  /*! \brief Show open window that contains given sql form
   */
  void showSqlWindow(const mdtSqlForm * const form, bool enableNavigation, bool enableEdition);

  /*! \brief Get open window for given sql form
   */
  std::shared_ptr<mdtSqlWindow> getOpenSqlWindow(const mdtSqlForm * const form);

  /*! \brief Close all open SQL windows
   *
   * All SQL windows (mdtSqlWindow objects) that was created with setupEditorInSqlWindow()
   *  are stored in a container. When calling this function, it is checked if internal
   *  form (a mdtSqlForm object) has all data saved.
   *
   * If one window contains a form that has pending data, this function retunrs false.
   */
  bool closeOpenSqlWindows();

  /*! \brief Close custom widgets
   *
   * For windows created with a mdtSqlForm in a mdtSqlWindow,
   *  closeOpenSqlWindows() can be used.
   *  For real application, many other widgets can be created.
   *
   * Subclass can reimplement this function to close such widgets,
   *  and use their specific way to check if it is OK to really close it now.
   *
   * If one widget cannot be closed now, this function returns false.
   *
   * Default implementation does nothing and allways return true.
   */
  virtual bool closeCustomWidgets()
  {
    qDebug() << "mdtSqlApplicationWidgets::closeCustomWidgets() ...";
    return true;
  }

  /*! \brief Destroy all created widgets
   *
   * Must be impplemented in subclass.
   */
  virtual void clearAllWidgets() = 0;

  /*! \brief Display a error to the user
   */
  void displayError(const mdtError & error);

  // Prevent multiple object instances
  mdtSqlApplicationWidgets() {}

  // Destructor
  ~mdtSqlApplicationWidgets()
  {
    pvInstance = nullptr;
  }

  // Disable also move and copy
  mdtSqlApplicationWidgets(const mdtSqlApplicationWidgets &&) = delete;
  mdtSqlApplicationWidgets(mdtSqlApplicationWidgets &) = delete;
  mdtSqlApplicationWidgets & operator=(const mdtSqlApplicationWidgets &) = delete;

  QSqlDatabase pvDatabase;

 private:

  static T *pvInstance;
  QList<std::shared_ptr<mdtSqlWindow> > pvOpenEditorWidows;
};
template<typename T> T *mdtSqlApplicationWidgets<T>::pvInstance = nullptr;


template <typename T>
bool mdtSqlApplicationWidgets<T>::closeOpenWidgets()
{
  if(!instance()->closeOpenSqlWindows()){
    return false;
  }
  return instance()->closeCustomWidgets();
}

template <typename T>
void mdtSqlApplicationWidgets<T>::clear()
{
  instance()->clearAllWidgets();
  instance()->pvOpenEditorWidows.clear();
}

template <typename T>
std::shared_ptr<mdtSqlWindow> mdtSqlApplicationWidgets<T>::setupEditorInSqlWindow(mdtSqlForm * const editor)
{
  Q_ASSERT(editor != nullptr);

  std::shared_ptr<mdtSqlWindow> window(new mdtSqlWindow);

  window->setSqlForm(editor);
  window->enableInsertion();
  window->enableDeletion();
  window->enableEdition();
  pvOpenEditorWidows.append(window);

  return window;
}

template <typename T>
void mdtSqlApplicationWidgets<T>::showSqlWindow(const mdtSqlForm * const form, bool enableNavigation, bool enableEdition)
{
  Q_ASSERT(form != nullptr);

  auto window = getOpenSqlWindow(form);
  Q_ASSERT(window);
  window->setNavigationEnabled(enableNavigation);
  window->setEditionEnabled(enableEdition);
  window->raise();
  window->show();
}

template <typename T>
std::shared_ptr<mdtSqlWindow> mdtSqlApplicationWidgets<T>::getOpenSqlWindow(const mdtSqlForm * const form)
{
  Q_ASSERT(form != nullptr);

  for(auto & window : pvOpenEditorWidows){
    Q_ASSERT(window);
    if(window->sqlForm() == form){
      return window;
    }
  }

  return std::shared_ptr<mdtSqlWindow>();
}

template <typename T>
bool mdtSqlApplicationWidgets<T>::closeOpenSqlWindows()
{
  mdtSqlForm *form;

  // Close all editors handled by a generic SQL window
  for(auto & window : pvOpenEditorWidows){
    Q_ASSERT(window);
    form = window->sqlForm();
    if(form != 0){
      if(!form->allDataAreSaved()){
        return false;
      }
    }
    window->close();
  }

  return true;
}

template <typename T>
void mdtSqlApplicationWidgets<T>::displayError(const mdtError & error)
{
  mdtErrorDialog dialog(error);
  dialog.exec();
}

/*! \brief Guard for mdtSqlApplicationWidgets
 *
 * mdtSqlApplicationWidgets is based on a singleton pattern.
 *  A problem is that mdtSqlApplicationWidgets::destruct() must
 *  be called explicitly to free resource.
 *
 * To avoid handling this free explicitly,
 *  create one instance of mdtSqlApplicationWidgetsGuard
 *  in the scope of the lifetime of mdtSqlApplicationWidgets (f.ex. in main() ).
 *
 * \note One option was tested with mdtSqlApplicationWidgets::instance() returning a shared pointer,
 *        which also avoid creating this guard class.
 *        The problem with this approach is that calling a function witch calls
 *        mdtSqlApplicationWidgets::instance() creates a shared pointer,
 *        witch is also destructed when the function returns.
 *        The result was allways creating and deleting the application widgets singleton
 *        (witch is not only a performance issue, but a danger if instance pointer is reused somewhere).
 *
 * \code
 * int main(void)
 * {
 *  // myApplicationWidgets is a specialization of mdtSqlApplicationWidgets
 *  mdtSqlApplicationWidgetsGuard<myApplicationWidgets> myAppGuard;
 *
 *  // Classic init stuff (f.ex. QApplication, ...)
 *
 *  // Application code ...
 *  myApplicationWidgets::showSomeWidget();
 *
 *  // When returning from main, myApplicationWidgets::destrcut() is called
 * }
 * \endcode
 */
template <typename T>
class mdtSqlApplicationWidgetsGuard
{
 public:

  mdtSqlApplicationWidgetsGuard() {}
  // Move and copy is not allowed
  mdtSqlApplicationWidgetsGuard(mdtSqlApplicationWidgetsGuard &&) = delete;
  mdtSqlApplicationWidgetsGuard(const mdtSqlApplicationWidgetsGuard &) = delete;
  mdtSqlApplicationWidgetsGuard operator=(const mdtSqlApplicationWidgetsGuard &) = delete;

  /*! \brief Destructor
   *
   * Will call destruct() on specified mdtSqlApplicationWidgets type
   */
  ~mdtSqlApplicationWidgetsGuard()
  {
    mdtSqlApplicationWidgets<T>::destruct();
  }
};

#endif // #ifndef MDT_SQL_APPLICATION_WIDGETS_H
