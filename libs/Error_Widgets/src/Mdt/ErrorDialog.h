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
#ifndef MDT_ERROR_DIALOG_H
#define MDT_ERROR_DIALOG_H

#include "Mdt/Error.h"
#include <QMessageBox>

namespace Mdt{

  /*! \brief Dialog that displays Mdt::Error
   */
  class ErrorDialog : public QMessageBox
  {
  public:

    /*! \brief Construct a dialog without error
     */
    ErrorDialog(QWidget* parent = nullptr);

    /*! \brief Construct a dialog with error
     */
    ErrorDialog(const Mdt::Error & error, QWidget *parent = nullptr);

    /*! \brief Set error
     */
    void setError(const Mdt::Error & error);

  private:

    /*! \brief Set icon regarding error level
     */
    void setIconFromLevel(Mdt::Error::Level level);
  };

} // namespace Mdt{

#endif // #ifndef MDT_ERROR_DIALOG_H
