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
#ifndef MDT_CSV_PARSER_FORMAT_SETUP_DELEGATE_H
#define MDT_CSV_PARSER_FORMAT_SETUP_DELEGATE_H

#include "mdtComboBoxItemDelegate.h"

namespace mdt{ namespace csv{

  /*! \brief Item delegate to let the user choose CSV format
   *
   * Usage is similar to mdtComboBoxItemDelegate,
   *  but without population step.
   */
  class ParserFormatSetupDelegate : public mdtComboBoxItemDelegate
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * Will populate combobox with available CSV field types
     */
    ParserFormatSetupDelegate(QObject *parent = nullptr);
  };

}} // namespace mdt{ namespace csv{

#endif // #ifndef MDT_CSV_PARSER_FORMAT_SETUP_DELEGATE_H
