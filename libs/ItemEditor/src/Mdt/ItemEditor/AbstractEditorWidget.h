/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_EDITOR_WIDGET_H
#define MDT_ITEM_EDITOR_ABSTRACT_EDITOR_WIDGET_H

#include "AbstractController.h"
#include <QWidget>

class QAbstractProxyModel;

namespace Mdt{ namespace ItemEditor{

  /*! \brief Base class to create a editor widget
   *
   * AbstractEditorWidget does not own any controller,
   *  but references the one a subclass handles.
   */
  class AbstractEditorWidget : public QWidget
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit AbstractEditorWidget(QWidget* parent = nullptr);

//     /*! \brief Get controller
//      */
//     AbstractController *controller() const
//     {
//       Q_ASSERT(pvController != nullptr);
//       return pvController;
//     }

    /*! \brief Prepend a proxy model
     *
     * \pre \a proxyModel must be a valid pointer
     */
    void prependProxyModel(QAbstractProxyModel *proxyModel);

    /*! \brief Append a proxy model
     *
     * \pre \a proxyModel must be a valid pointer
     */
    void appendProxyModel(QAbstractProxyModel *proxyModel);

   protected:

    /*! \brief Get controller
     *
     * Subclass using a contrete controller must implement this method.
     */
    virtual AbstractController *abstractController() const = 0;

//     /*! \brief Set controller
//      *
//      * Subclass that owns its controller
//      *  must tell it by calling this method.
//      */
//     virtual void setController(AbstractController *controller);

//    private:
// 
//     AbstractController *pvController;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_EDITOR_WIDGET_H
