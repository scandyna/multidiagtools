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
#include "mdtErrorDialog.h"

mdtErrorDialog::mdtErrorDialog(QWidget* parent)
 : QMessageBox(parent)
{
}

mdtErrorDialog::mdtErrorDialog(const mdtError & error, QWidget* parent)
 : QMessageBox(parent)
{
  setError(error);
}

void mdtErrorDialog::setError(const mdtError & error)
{
  setIconFromLevel(error.level());
  setText(error.text());
}

void mdtErrorDialog::setIconFromLevel(mdtError::Level level)
{
  switch(level){
    case mdtError::Error:
      setIcon(QMessageBox::Critical);
      break;
    case mdtError::Warning:
      setIcon(QMessageBox::Warning);
      break;
    case mdtError::Info:
      setIcon(QMessageBox::Information);
      break;
    case mdtError::NoError:
      setIcon(QMessageBox::NoIcon);
      break;
  }
}
