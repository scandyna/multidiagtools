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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_ITEM_MODEL_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_ITEM_MODEL_CONTROLLER_H

#include "AbstractController.h"
#include <initializer_list>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Common base for controllers that can act directly on a QAbstractItemModel
   */
  class AbstractItemModelController : public AbstractController
  {
   Q_OBJECT

   public:

    /*! \brief Contructor
     */
    explicit AbstractItemModelController(QObject* parent = nullptr);

    // Copy disabled
    AbstractItemModelController(const AbstractItemModelController &) = delete;
    AbstractItemModelController & operator=(const AbstractItemModelController &) = delete;
    // Move disabled
    AbstractItemModelController(AbstractItemModelController &&) = delete;
    AbstractItemModelController & operator=(AbstractItemModelController &&) = delete;

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
    void setModel(QAbstractItemModel *model);

    /*! \brief Get model
     *
     * Returns the model set with setModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
    QAbstractItemModel *model() const
    {
      return sourceModel();
//       return registeredModel();
    }

    /*! \brief Set primary key
     *
     * \note When source model changes, the primary key will be cleared.
     * \pre sourceModel must be set before setting the primary key
     * \pre Each column in \a pk must be in valid range ( 0 <= column < sourceModel()->columnCount() )
     * \pre Each column in \a pk must be unique
     */
    void setPrimaryKey(const Mdt::ItemModel::PrimaryKey & pk);

    /*! \brief Set primary key
     *
     * \note When source model changes, the primary key will be cleared.
     * \pre sourceModel must be set before setting the primary key
     * \pre Each column in \a list must be in valid range ( 0 <= column < sourceModel()->columnCount() )
     * \pre Each column in \a list must be unique
     */
    void setPrimaryKey(std::initializer_list<int> list);

    /*! \brief Set foreign key
     *
     * \note When source model changes, the foreign key will be cleared.
     * \pre sourceModel must be set before setting the foreign key
     * \pre Each column in \a fk must be in valid range ( 0 <= column < sourceModel()->columnCount() )
     * \pre Each column in \a fk must be unique
     */
    void setForeignKey(const Mdt::ItemModel::ForeignKey & fk);

    /*! \brief Set foreign key
     *
     * \note When source model changes, the foreign key will be cleared.
     * \pre sourceModel must be set before setting the foreign key
     * \pre Each column in \a list must be in valid range ( 0 <= column < sourceModel()->columnCount() )
     * \pre Each column in \a list must be unique
     */
    void setForeignKey(std::initializer_list<int> list);

   private:

    
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_ITEM_MODEL_CONTROLLER_H
