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
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlFieldHandler.h"
#include "mdtError.h"
#include "mdtSqlFormWidgetDataValidator.h"
#include "mdtSqlSchemaTable.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QLayout>
#include <QLayoutItem>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>

#include <QDebug>

mdtSqlDataWidgetController::mdtSqlDataWidgetController(QObject* parent)
 : mdtAbstractSqlTableController(parent)
{
}

mdtSqlDataWidgetController::~mdtSqlDataWidgetController()
{
}

bool mdtSqlDataWidgetController::mapFormWidgets(QWidget* widget, const QString& firstWidgetInTabOrder)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(widget->layout() != 0);
  Q_ASSERT(model());

  int i, fieldIndex;
  QString fieldName;
  QWidget *w;
  mdtSqlFieldHandler *fieldHandler;
  QSqlRecord record;
  mdtSqlSchemaTable st;

  // Clear previous mapping
  qDeleteAll(pvFieldHandlers);
  pvFieldHandlers.clear();
  pvWidgetMapper.clearMapping();
  pvFirstDataWidget = 0;
  // Search widgets with fld_ as prefix in they objectName
  buildWidgetsList(widget, "fld_");
  // If we want informations about fields, we must get record from database instance
  record = model()->database().record(model()->tableName());
  // Fetch table information - record returned by QSqlDatabase does not return Date and DateTime field infomration
  if(!st.setupFromTable(model()->tableName(), model()->database())){
    pvLastError = st.lastError();
    return false;
  }
  // Map found widgets
  for(i = 0; i < pvFoundWidgets.size(); ++i){
    w = pvFoundWidgets.at(i);
    Q_ASSERT(w != 0);
    fieldName = w->objectName();
    // Search matching field in model
    fieldName = fieldName.right(fieldName.length()-4);
    fieldIndex = model()->record().indexOf(fieldName);
    // If field was found, map it
    if(fieldIndex >= 0){
      fieldHandler = new mdtSqlFieldHandler;
      fieldHandler->setField(st.field(fieldName));
      fieldHandler->setDataWidget(w);
      ///connect(fieldHandler, SIGNAL(dataEdited()), this, SIGNAL(dataEdited()));
      pvFieldHandlers.append(fieldHandler);
      // If this widget is the first in focus chain, ref it
      if(w->objectName() == firstWidgetInTabOrder){
        pvFirstDataWidget = w;
      }
      // Add to widget mapper
      pvWidgetMapper.addMapping(fieldHandler, fieldIndex, "data");
    }else{
      w->setEnabled(false);
      mdtError e(tr("Cannot find field for widget '") + w->objectName() + tr("'"), mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtSqlFormWidget");
      e.commit();
    }
  }
  /*
   * When calling select() , setQuery() , setFilter() or something similar on model,
   * Widget mapper will not update. This problem is not in QTableView
   *  (witch connects model's rowsInserted() , rowsRemoved(), etc signals to internal slots to handle this, I think).
   * As workaround, we will connect a signal signal to Widget mapper.
   */
  ///connect(this, SIGNAL(modelSelected()), pvWidgetMapper, SLOT(toFirst()));
  /**
  connect(this, SIGNAL(modelSelected()), this, SLOT(onModelSelected()));
  if(model()->rowCount() < 1){
    onCurrentIndexChanged(-1);
  }else{
    pvWidgetMapper->toFirst();
  }
  */
  // Add data validator
  ///addDataValidator(new mdtSqlFormWidgetDataValidator(model(), 0, pvFieldHandlers));

  return true;
}


void mdtSqlDataWidgetController::buildWidgetsList(QWidget *w, const QString &prefix)
{
  Q_ASSERT(w != 0);
  Q_ASSERT(w->layout() != 0);

  int i;

  pvFoundWidgets.clear();
  for(i = 0; i < w->layout()->count(); ++i){
    searchWidgets(w->layout()->itemAt(i), prefix);
  }
}

void mdtSqlDataWidgetController::searchWidgets(QLayoutItem *item, const QString &prefix)
{
  Q_ASSERT(item != 0);

  int i;
  QWidget *widget;
  QLayout *layout;
  QString name;

  // If current item is a layout, and search in it if true
  layout = item->layout();
  if(layout != 0){
    for(i = 0; i < layout->count(); ++i){
      searchWidgets(layout->itemAt(i), prefix);
    }
  }
  widget = item->widget();
  if(widget != 0){
    name = widget->objectName();
    if(name.left(prefix.size()) == prefix){
      pvFoundWidgets.append(widget);
    }
    // If widget has a layout, serach in it
    layout = widget->layout();
    if(layout != 0){
      for(i = 0; i < layout->count(); ++i){
        searchWidgets(layout->itemAt(i), prefix);
      }
    }
  }
}
