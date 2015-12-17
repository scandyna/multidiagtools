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
#ifndef MDT_ERROR_DIALOG_H
#define MDT_ERROR_DIALOG_H

#include "mdtError.h"
#include <QMessageBox>

/*! \brief Dialog that displays mdtError
 */
class mdtErrorDialog : public QMessageBox
{
 public:

  /*! \brief Construct a dialog without error
   */
  mdtErrorDialog(QWidget* parent = nullptr);

  /*! \brief Construct a dialog with error
   */
  mdtErrorDialog(const mdtError & error, QWidget *parent = nullptr);

  /*! \brief Set error
   */
  void setError(const mdtError & error);

 private:

  /*! \brief Set icon regarding error level
   */
  void setIconFromLevel(mdtError::Level level);
};

#endif // #ifndef MDT_ERROR_DIALOG_H
