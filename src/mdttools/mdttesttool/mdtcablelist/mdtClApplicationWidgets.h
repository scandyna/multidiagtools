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

#include "mdtSqlApplicationWidgets.h"

// Editors
class mdtClUnitEditor;

/*! \brief Container for cable list editors and views
 *
 * Using this class helps to create editors and views.
 *  Most of time, only one instance of each editore or view is needed,
 *  but must be accessible from multiple places in application.
 */
class mdtClApplicationWidgets : public mdtSqlApplicationWidgets<mdtClApplicationWidgets>
{
 Q_OBJECT

 // Needed because mdtSqlApplicationWidgets must access some private function from mdtTtApplicationWidgets
 friend class mdtSqlApplicationWidgets<mdtClApplicationWidgets>;

 public:

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

  /*! \brief Clear all widgets
   */
  void clearAllWidgets();

  /*! \brief Prevent multiple object instances
   */
  mdtClApplicationWidgets() {}

  // We define no destructer, it will never be called

  Q_DISABLE_COPY(mdtClApplicationWidgets);

//   QSqlDatabase pvDatabase;
//   QList<std::shared_ptr<mdtSqlWindow> > pvOpenEditorWidows;
  // Editors
  std::shared_ptr<mdtClUnitEditor> pvUnitEditor;
};

#endif // #ifndef MDT_CL_APPLICATION_WIDGETS_H
