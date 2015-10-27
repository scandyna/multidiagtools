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
#include "mdtClConnectorKeyData.h"
#include <QPointer>

// Editors
class mdtClVehicleTypeEditor;
class mdtClConnectorEditor;
class mdtClArticleEditor;
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

 // Needed because mdtSqlApplicationWidgets must access some private function from mdtClApplicationWidgets
 friend class mdtSqlApplicationWidgets<mdtClApplicationWidgets>;

 public:

  /*! \brief Edit vehicle types
   */
  static void editVehicleTypes();

 private:

  /*! \brief Setup and show vehicle type editor
   */
  void setupAndShowVehicleTypeEditor();

  /*! \brief Create vehicle type editor
   */
  bool createVehicleTypeEditor();

 public:

  /*! \brief Edit connectors
   */
  static void editConnectors();

  /*! \brief Edit specified connector
   */
  static void editConnector(const mdtClConnectorPkData & pk);

 private:

  /*! \brief Setup and show connector editor
   */
  void setupAndShowConnectorEditor(const mdtClConnectorPkData & pk = mdtClConnectorPkData());

  /*! \brief Create connector editor
   */
  bool createConnectorEditor();

 public:

  /*! \brief Edit articles
   */
  static void editArticles();

  /*! \brief Edit specified article
   */
  static void editArticle(const QVariant & articleId);

 private:

  /*! \brief Setup and show article editor
   */
  void setupAndShowArticleEditor(const QVariant & articleId = QVariant());

  /*! \brief Create article editor
   */
  bool createArticleEditor();

 public:

  /*! \brief Edit a specific unit
   */
  static void editUnit(const QVariant & unitId);

  /*! \brief Edit units
   */
  static void editUnits();

 private:

  /*! \brief Setup and show unit editor
   */
  void setupAndShowUnitEditor(const QVariant & unitId = QVariant());

  /*! \brief Create Unit editor
   */
  bool createUnitEditor();

 public:

  /*! \brief Create a new link version
   */
  void createLinkVersion();

 private:

  /*! \brief Clear all widgets
   */
  void clearAllWidgets();

  /*! \brief Prevent multiple object instances
   */
  mdtClApplicationWidgets();

  // We define no destructer, it will never be called

  Q_DISABLE_COPY(mdtClApplicationWidgets);

  // Editors
  QPointer<mdtClVehicleTypeEditor> pvVehicleTypeEditor;
  QPointer<mdtClConnectorEditor> pvConnectorEditor;
  QPointer<mdtClArticleEditor> pvArticleEditor;
  QPointer<mdtClUnitEditor> pvUnitEditor;
};

#endif // #ifndef MDT_CL_APPLICATION_WIDGETS_H
