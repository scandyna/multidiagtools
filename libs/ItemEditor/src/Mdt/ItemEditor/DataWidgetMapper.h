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
#ifndef MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_H
#define MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_H

#include "MappedWidgetList.h"
#include <QAbstractItemDelegate>
#include <QObject>
#include <QPointer>

class QWidget;
class QAbstractItemModel;
class QModelIndex;

namespace Mdt{ namespace ItemEditor{

  /*! \brief DataWidgetMapper provides mapping between a column of data model to widgets
   *
   * Qt provides QDataWidgetMapper, which provides such mapping.
   *  Some feature are missing in QDataWidgetMapper to work with AbstractWidgetMapperController:
   *   - Enable/disable mapped widgets regarding current row, controller state, etc..
   *      (is possible to fix without changing QDataWidgetMapper)
   *   - Clear widgets content on invalid current row or empty model
   *      (is also possible to fix without changing QDataWidgetMapper)
   *   - Catch when user beginns to edit in a mapped widget
   *      (found no clean solution for that)
   *
   * This is why DataWidgetMapper was created, which provides less functionality
   *  than QDataWidgetMapper, but can interact with AbstractWidgetMapperController as expected.
   */
  class DataWidgetMapper : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit DataWidgetMapper(QObject* parent = nullptr);

    /*! \brief Set model
     *
     * After model was set, current row will be set to -1.
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the mapper does not take ownership of it (it will not delete it).
     * \pre No widget must be mapped when setting a model. If widget has been mapped, call clearMapping() first.
     *       Which widget must be mapped with which column is only known from application developper,
     *       this is the reason why no automatic remapping is done when changing the model.
     * \pre model must be a valid pointer
     * \sa model()
     */
    void setModel(QAbstractItemModel *model);

    /*! \brief Get model
     *
     * Can return a nullptr if no model was set,
     *  or model was deleted.
     *
     * \sa setModel()
     */
    QAbstractItemModel *model() const;

    /*! \brief Set item delegate
     *
     * \sa itemDelegate()
     */
    void setItemDelegate(QAbstractItemDelegate *delegate);

    /*! \brief Get item delegate
     *
     * \sa setItemDelegate()
     */
    QAbstractItemDelegate *itemDelegate() const;

    /*! \brief Adds a mapping between a widget and a column from the model
     *
     * Once widget is mapped, data will be exchanged between model and widget
     *  using this mapper's delegate.
     *  If this mapper refers to a invalid index, widget will be disabled.
     *
     * If widget has a readOnly property, it will be used if model's Qt::ItemIsEditable
     *  flag is not present for current index, else widget will be disabled in this case.
     *
     * \note widget will not display model data until setCurrentRow() was called
     * \pre model must be set before mapping any widget
     * \pre widget pointer must be valid
     * \sa setModel()
     */
    void addMapping(QWidget *widget, int column);

    /*! \brief Clear mapping
     *
     * After that call, all widgets that were mapped will no longer
     *  been handled by this mapper.
     *
     * \note Mapped widgets are not owned by this mapper,
     *        so they will not be deleted.
     */
    void clearMapping();

    /*! \brief Get current row
     */
    int currentRow() const
    {
      return pvCurrentRow;
    }

   public slots:

    /*! \brief Set current row
     */
    void setCurrentRow(int row);

    /*! \brief Submit data from mapped widgets to the model
     *
     * \pre Model must be set before calling this method
     */
    bool submit();

    /*! \brief Repopulate mapped widgets with data from model
     */
    void revert();

   signals:

    /*! \brief Emitted whenever current row changed
     */
    void currentRowChanged(int row);

    /*! \brief Emitted when user started editing with a mapped widget
     */
    void dataEditionStarted();

    /*! \brief Emitted when editing was done
     *
     * Editing is done afetr a successfull submit()
     *  or a revert()
     */
    void dataEditionDone();

   private slots:

    /*! \brief Called from any mapped widget that start editing
     */
    void onDataEditionStarted();

    /*! \brief Called from model when data changed
     */
    void onModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles);

    /*! \brief Called from model after a reset
     *
     * Will allways place the mapper to currentRow -1
     * (Controller can then set currentRow to a appropriate row)
     */
    void onModelReset();

    /*! \brief Called when model has new columns inserted
     */
//     void onModelColumnsInserted(const QModelIndex & parent, int first, int last);

   private:

    enum class ConnectAction
    {
      Connect,
      Disctonnect
    };

    /*! \brief Connect widget's user property notify signal to this onDataEditionStarted() slot
     */
    void connectUserPropertyNotifySignal(QWidget * const widget, ConnectAction ca);

    /*! \brief Update editor
     */
    void updateMappedWidget(QWidget * const widget, int column, bool hasReadOnlyProperty, bool updateDataFromModel);

    /*! \brief Update all mapped widgets
     */
    void updateAllMappedWidgets(bool updateDataFromModel);

    /*! \brief Commit widget's data to model
     */
    bool commitData(QWidget * const widget, int column);

    /*! \brief Commit data of all mapped widgets to model
     */
    bool commitAllMappedWidgetsData();
    
    int pvCurrentRow;
    bool pvUpdatingMappedWidget;
    bool pvEditionStartNotified;
    QPointer<QAbstractItemModel> pvModel;
    QPointer<QAbstractItemDelegate> pvDelegate;
    MappedWidgetList pvMappedWidgetList;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_DATA_WIDGET_MAPPER_H
