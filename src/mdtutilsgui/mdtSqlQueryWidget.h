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
#ifndef MDT_SQL_QUERY_WIDGET_H
#define MDT_SQL_QUERY_WIDGET_H

#include "ui_mdtSqlQueryWidget.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>

class QSqlQueryModel;

/*! \brief Widget for data queriyng
 */
class mdtSqlQueryWidget : public QWidget, Ui::mdtSqlQueryWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlQueryWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlQueryWidget();

  /*! \brief Set database
   */
  void setDatabase(QSqlDatabase db);

  /*! \brief Set correspondence between fields name and texts to display in header
   *
   * \param mapping Map containing field name as keys and display text as values
   */
  void setHeaderTextsByFieldNames(const QHash<QString, QString> &mapping);

 private slots:

  /*! \brief Select...
   */
  void select();

  /*! \brief Display the table chooser
   */
  void chooseTables();

  /*! \brief Display the fields chooser
   */
  void chooseFields();

  /*! \brief Display the fields sort chooser
   */
  void chooseFieldsSort();

 private:

  /*! \brief Update header texts
   *
   * Will set display texts corresponding to each field in model
   */
  void updateHeaderTexts();

  /*! \brief Remove sorting fields that are not in selected fields
   *
   * When remove fields in pvFields, call this method to remove them
   *  in pvSortingFields and pvSortingFieldsWithOrder.
   */
  void removeNonExistantFieldsInSortingFields();

  Q_DISABLE_COPY(mdtSqlQueryWidget);

  QSqlQueryModel *pvModel;
  QSqlDatabase pvDb;
  QHash<QString, QString> pvHeaderTextsByFieldNames;  // For model's header
  QHash<QString, QString> pvFieldNamesBySelectorDisplayTexts; // Used with selector dialog
  QStringList pvTables;
  QStringList pvFields;         // Fields (technical) name in correct (display) order
  QStringList pvSortingFields;  // Fields (technical) name in correct sort order
  QList<QPair<QString, int> > pvSortingFieldsWithOrder;  // Fields (technical) name and sort order (ASC, DESC), in correct sort order
};

#endif  // #ifndef MDT_SQL_QUERY_WIDGET_H
