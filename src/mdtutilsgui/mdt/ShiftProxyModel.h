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
#ifndef MDT_SHIFT_PROXY_MODEL_H
#define MDT_SHIFT_PROXY_MODEL_H

#include <QAbstractProxyModel>

namespace mdt{

  /*! \brief Shifts data
   */
  class ShiftProxyModel : public QAbstractProxyModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit ShiftProxyModel(QObject *parent = nullptr);

    /*! \brief Set row offset
     *
     * \pre offset must be >= 0
     */
    void setRowOffset(int offset);

    /*! \brief Get index
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get parent
     */
    QModelIndex parent(const QModelIndex & child) const override;

    /*! \brief Get row count
     */
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Get column count
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    /*! \brief Map from source
     */
    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const override;

    /*! \brief Map to source
     */
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const override;


  private:

    int pvColumnOffset;
    int pvRowOffset;
  };

} // namespace mdt{

#endif // #ifndef MDT_SHIFT_PROXY_MODEL_H
