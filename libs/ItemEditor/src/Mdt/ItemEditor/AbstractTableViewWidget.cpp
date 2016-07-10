/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "AbstractTableViewWidget.h"
#include "EventCatchItemDelegate.h"
#include "ItemSelectionModel.h"
#include "RowChangeEventMapper.h"
#include <QTableView>
#include <QVBoxLayout>

namespace Mdt{ namespace ItemEditor{

AbstractTableViewWidget::AbstractTableViewWidget(QWidget* parent)
 : AbstractEditorWidget(parent),
   pvView(new QTableView),
   pvRowChangeEventMapper(new RowChangeEventMapper(this))
{
  // Layout widgets
  auto *l = new QVBoxLayout;
  l->addWidget(pvView);
  setLayout(l);
  // Replace delegate withour our proxy delegate
  auto *delegate = pvView->itemDelegate();
  auto *proxyDelegate = new EventCatchItemDelegate(pvView);
  if(delegate != nullptr){
    proxyDelegate->setItemDelegate(delegate);
  }
  pvView->setItemDelegate(proxyDelegate);
  // Some signals/slots

  /// \todo other signal/slot connections

}

void AbstractTableViewWidget::setController(AbstractController* controller)
{
  Q_ASSERT(controller != nullptr);

  AbstractEditorWidget::setController(controller);
  /// \todo Must go to setCurrentRow of controller ! (and controller must handle + avoid invinit calls..)
  connect(pvRowChangeEventMapper, &RowChangeEventMapper::rowStateChanged, controller, &AbstractController::rowStateChanged);
}

void AbstractTableViewWidget::updateModel(QAbstractItemModel *model)
{
  if(model != nullptr){
    /*
     * As described in documentation of QAbstractItemView,
     * the selection model will be replaced.
     * QAbstractItemView does not delete the old selection model.
     * While we are owning our view, we can safely delete it here.
     */
    auto *oldSelectionModel = pvView->selectionModel();
    pvView->setModel(model);
    if(oldSelectionModel != nullptr){
      delete oldSelectionModel;
    }
    // Replace selection model
    /// \todo Should we delete this selection model in destructor ??
    pvView->setSelectionModel(new ItemSelectionModel(model));
    /// \todo signal/slots connections
    // Update row change event mapper
    pvRowChangeEventMapper->setModel(model);
    pvRowChangeEventMapper->setSelectionModel(pvView->selectionModel());
  }
}

}} // namespace Mdt{ namespace ItemEditor{
