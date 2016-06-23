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
#include "Schema.h"

namespace Mdt{ namespace Sql{ namespace Schema{

void Schema::addTable(const Table & table)
{
  pvTableList.append(table);
}

void Schema::addView(const View & view)
{
  pvViewList.append(view);
}

void Schema::addTablePopulation(const TablePopulation& tp)
{
  pvTablePopulationList.append(tp);
}

void Schema::addTrigger(const Trigger& trigger)
{
  pvTriggerList.append(trigger);
}

void Schema::clear()
{
  pvTableList.clear();
  pvViewList.clear();
  pvTablePopulationList.clear();
  pvTriggerList.clear();
}

}}} // namespace Mdt{ namespace Sql{ namespace Schema{
