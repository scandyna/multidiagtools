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
#ifndef MDT_ITEM_EDITOR_ROW_STATE_H
#define MDT_ITEM_EDITOR_ROW_STATE_H

#include <QMetaType>

namespace Mdt{ namespace ItemEditor{

  /*! \brief
   *
   * In model/view, the model has a row count,
   *  and the view the concept of current row.
   *
   * The following table shows some examples of row states:
   * <table class="srcdoc_td_center">
   *  <tr><th>Row count</th><th>Current row</th><th>Is valid</th><th>Is null</th><th>Remarks</th></tr>
   *  <tr><td>0</td><td>-1</td><td>Yes</td><td>Yes</td><td></td></tr>
   *  <tr><td>1</td><td>-1</td><td>No</td><td>(Yes)</td><td>Happens, f.ex., if a QItemSelectionModel has no current index</td></tr>
   *  <tr><td>1</td><td>0</td><td>Yes</td><td>No</td><td></td></tr>
   *  <tr><td>1</td><td>1</td><td>No</td><td>(Yes)</td><td></td></tr>
   *  <tr><td>2</td><td>-1</td><td>No</td><td>(Yes)</td><td></td></tr>
   *  <tr><td>2</td><td>0</td><td>Yes</td><td>No</td><td></td></tr>
   *  <tr><td>2</td><td>1</td><td>Yes</td><td>No</td><td></td></tr>
   *  <tr><td>3</td><td>-1</td><td>No</td><td>(Yes)</td><td></td></tr>
   *  <tr><td>3</td><td>0</td><td>Yes</td><td>No</td><td></td></tr>
   *  <tr><td>3</td><td>1</td><td>Yes</td><td>No</td><td></td></tr>
   *  <tr><td>3</td><td>2</td><td>Yes</td><td>No</td><td></td></tr>
   * </table>
   */
  class RowState
  {
   public:

    /*! \brief Construct a null row state
     */
    constexpr RowState() noexcept
     : pvRowCount(0), pvCurrentRow(-1) {}

    /*! \brief Set row count
     *
     * \pre n must be >= 0
     */
    void setRowCount(int n) noexcept
    {
      Q_ASSERT(n >= 0);
      pvRowCount = n;
    }

    /*! \brief Get row count
     */
    constexpr int rowCount() const noexcept
    {
      return pvRowCount;
    }

    /*! \brief Set current row
     */
    constexpr void setCurrentRow(int row) noexcept
    {
      pvCurrentRow = row;
    }

    /*! \brief Get current row
     */
    constexpr int currentRow() const noexcept
    {
      return pvCurrentRow;
    }

    /*! \brief Check if this row state is valid
     */
    constexpr bool isValid() const noexcept
    {
      if(pvCurrentRow >= pvRowCount){
        return false;
      }
      if( (pvRowCount > 0) && (pvCurrentRow < 0) ){
        return false;
      }
      return true;
    }

    /*! \brief Check if this row state is null
     */
    constexpr bool isNull() const noexcept
    {
      if(!isValid()){
        return true;
      }
      if( (pvRowCount == 0) && (pvCurrentRow < 0) ){
        return true;
      }
      return false;
    }

    /*! \brief Clear
     */
    constexpr void clear() noexcept
    {
      pvRowCount = 0;
      pvCurrentRow = -1;
    }

   private:

    int pvRowCount;
    int pvCurrentRow;
  };

}} // namespace Mdt{ namespace ItemEditor{

Q_DECLARE_METATYPE(Mdt::ItemEditor::RowState)

#endif // #ifndef MDT_ITEM_EDITOR_ROW_STATE_H
