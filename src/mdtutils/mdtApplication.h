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
#ifndef MDT_APPLICATION_H
#define MDT_APPLICATION_H

#include <QtSingleApplication>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QTranslator>
#include <QDir>
#include <QLocale>
#include <QAction>
#include <config.h>

#ifndef mdtApp
 #define mdtApp mdtApplication::instance()
#endif

/// NOTE: manage directories better (with, f.ex, a member containing search paths ..)
/// NOTE: Mutex for static members..

/*! \brief Main application
 *
 * \sa QApplication
 * \sa QtSingleApplication
 */
class mdtApplication : public QtSingleApplication
{
 Q_OBJECT

 public:

  mdtApplication(int &argc, char **argv, bool GUIenabled = true);
  ~mdtApplication();

  /*! \brief Some initializations
   * 
   * Several things are done here:
   *  - Search the system data directory
   *  - Init the error system
   *  - Check if another instance is running (if allowMultipleInstances Instance is false)
   *  - Load translation files and set language according system settings.
   * \param allowMultipleInstances If true, multiple instance of application is allowed (but NOT multiple instance of mdtApplication in program !)
   * \param dialogErrorLevelsMask Set the mask of errors that are displayed as dialogs in GUI. By default, no error (geven by mdtErrorOut)
   *          are displayed in GUI.
   * \sa mdtErrorOut::setDialogLevelsMask()
   * \sa mdtErrorOut
   */
  bool init(bool allowMultipleInstances = false, int dialogErrorLevelsMask = 0);

  /*! \brief Build the application version with numeric values.
   * 
   * Create a string with versionMajor.versionMinor.versionMicro , and calls QApplication::setApplicationVersion().
   */
  void setApplicationVersion(int versionMajor, int versionMinor, int versionMicro);

  /*! \brief Get the instance of mdtApplication
   *
   * This function, and returned pointer, should be used with care in multi-thread applicatopn !
   */
  static mdtApplication *instance();

  /*! \brief Get data dir path
   */
  static QString systemDataDirPath();

  /*! \brief Get the multi diag tools library version
   */
  static QString mdtLibVersion();

  /*! \brief Add a directory that contains translations files in search paths
   * 
   * If given directory exists (and is accessible), it will be added to search paths.
   * \pre The mdtErrorOut system must be initialized before using this method, this is done by init().
   */
  void addTranslationsDirectory(const QString &directory);

  /*! \brief Get the list of available translations
   * 
   * The key contains the language suffix, like en , that can be reused
   * with changeLanguage().
   * The list is built during init().
   */
  QMap<QString, QString> &availableTranslations();

  /*! \brief Get the current translation key.
   * 
   * The translation key could be en, de, fr, ...
   */
  QString &currentTranslationKey();

  /*! \brief Adds the translator to the list of translators to be used for translations.
   * 
   * Note: this method overloads QCoreApplication::installTranslator() .
   *   The difference is that it adds new translator to a list, and delete it all in destructor.
   *   Internally, it calls QApplication::installTranslator().
   *   To keep trace of installed translators, you should allways use this method, and never 
   *   mix calls with, for example, qApp->installTranslator() or QCoreApplication::installTranslator()
   * \pre translator must point to a valid object.
   */
  void installTranslator(QTranslator *translator);

  /*! \brief Remove installed translators
   *
   * This will romove all translators installed with installTranslator().
   * \sa installTranslator()
   */
  void removeCurrentTranslators();

 public slots:

  /*! \brief Change the application language
   * 
   * This will remove installed translators, and install language speficifed
   *  translators. Whenn translation files could not be loaded, the application
   *  should fall back to source code's language.
   * If no parameters are given, system's language will be used.
   * By default, the .qm translation files are searched in system's data directory.
   * It's possible to add other search paths using otherQmDirectories parameter.
   * 
   * \pre The error system must be initalized before using this method (this is done by init() ).
   */
  void changeLanguage(const QLocale &locale = QLocale());

  /*! \brief Change the application language (overload)
   * 
   * To make this slot work, the QAction must contain the
   *  language key (like en, fr, de, ...) in the data section.
   * \pre action must be a valid pointer.
   */
  void changeLanguage(QAction *action);

 signals:

  /*! \brief Emitted when language was changed using changeLanguage()
   */
  void languageChanged();

 private:

  // Search the data system directory
  bool searchSystemDataDir();

  // Make some checks on given directory, and returns true if it matches to the data directory
  bool isSystemDataDir(QDir &dir);

  // Create some directories in home
  bool initHomeDir();

  // Find available .qm files and build the available translations list
  void buildAvailableTranslationsList();

  // Load found translation files according given language
  bool loadTranslationFiles(const QString &languageSuffix);

  QString pvSystemDataDirPath;
  QString pvLogDirPath;
  QStringList pvTranslationsDirectories;  // Directories that contains .qm translations files
  static mdtApplication *pvInstance;
  QList<QTranslator*> pvTranslators;
  QMap<QString, QString> pvAvailableTranslations; // Key: suffix, like en, fr, . Value: English, Français, ..
  QString pvCurrentTranslationKey;                // Current loaded translation keay, like en, de, ...
};

#endif  // #ifndef MDT_APPLICATION_H
