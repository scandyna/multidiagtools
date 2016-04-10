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
#include "ParserFormatSetupDelegate.h"
#include "FieldType.h"
#include <algorithm>

namespace mdt{ namespace csv{

ParserFormatSetupDelegate::ParserFormatSetupDelegate(QObject *parent)
 : mdtComboBoxItemDelegate(parent)
{
  // Populate type combobox
  using mdt::csv::FieldType;
  auto ftList = FieldType::getAvailableFieldTypeList();
  std::sort(ftList.begin(), ftList.end(), [](const FieldType & a, const FieldType & b){ return a.name() < b.name(); } );
  for(const auto & ft : ftList){
    addItem(ft.name(), static_cast<int>(ft.type()) );
  }
}

}} // namespace mdt{ namespace csv{
