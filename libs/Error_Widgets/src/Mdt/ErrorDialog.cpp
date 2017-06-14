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
#include "ErrorDialog.h"

namespace Mdt{

ErrorDialog::ErrorDialog(QWidget* parent)
 : QMessageBox(parent)
{
}

ErrorDialog::ErrorDialog(const Mdt::Error & error, QWidget* parent)
 : QMessageBox(parent)
{
  setError(error);
}

void ErrorDialog::setError(const Mdt::Error & error)
{
  setIconFromLevel(error.level());
  setText(error.text());
  setInformativeText(error.informativeText());
}

void ErrorDialog::setIconFromLevel(Mdt::Error::Level level)
{
  switch(level){
    case Mdt::Error::Critical:
      setIcon(QMessageBox::Critical);
      break;
    case Mdt::Error::Warning:
      setIcon(QMessageBox::Warning);
      break;
    case Mdt::Error::Info:
      setIcon(QMessageBox::Information);
      break;
    case Mdt::Error::NoError:
      setIcon(QMessageBox::NoIcon);
      break;
  }
}

} // namespace Mdt{
