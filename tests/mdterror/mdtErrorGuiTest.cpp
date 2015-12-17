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
#include "mdtErrorGuiTest.h"
#include "mdtApplication.h"
#include "mdtErrorDialog.h"

void mdtErrorGuiTest::errorDialogTest()
{
  mdtErrorDialog dialog;
  mdtError error;

  error.setError<int>(1, "Some error", mdtError::Error);
  dialog.setError(error);

  /*
   * Play
   */
  dialog.exec();
}


/*
 * Main
 */
int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtErrorGuiTest errorTest;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&errorTest, argc, argv);
}
