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
#include <config.h>

/// NOTE: Mutex for static members..

/*! \brief Main application
 * \sa QApplication
 * \sa QtSingleApplication
 */
class mdtApplication : public QtSingleApplication
{
 public:

  mdtApplication(int &argc, char **argv, bool GUIenabled = true);
  ~mdtApplication();
  
  /*! \brief Some initializations
   * 
   * Several things are done here:
   *  - Search the system data directory path
   *  - Init the error system (\sa mdtErrorOut)
   *  - Check if another instance is running (if allowMultipleInstances Instance is false)
   * \param allowMultipleInstances If true, multiple instance of application is allowed (but NOT multiple instance of mdtApplication in program !)
   */
  bool init(bool allowMultipleInstances = true);

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

 private:

  // Search the data system directory
  bool searchSystemDataDir();

  // Create some directories in home
  bool initHomeDir();

  QString pvSystemDataDirPath;
  QString pvLogDirPath;
  static mdtApplication *pvInstance;
};

#endif  // #ifndef MDT_APPLICATION_H
