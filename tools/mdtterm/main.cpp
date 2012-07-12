/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtApplication.h"
#include "mdtPortTerm.h"
#include <QObject>
#include <QLocale>

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);

  // Init app, we allow multiple instances
  if(!app.init(true)){
    return 1;
  }

  // Init port term
  mdtPortTerm term;
  QObject::connect(&term, SIGNAL(languageChanged(const QLocale&)), &app, SLOT(changeLanguage(const QLocale&)));
  QObject::connect(&app, SIGNAL(languageChanged()), &term, SLOT(retranslate()));
  term.setAvailableTranslations(app.availableTranslations(), app.currentTranslationKey());
  term.show();

  return app.exec();
}
