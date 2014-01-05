/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtTestNodeUnitSetupData.h"

mdtTtTestNodeUnitSetupData::mdtTtTestNodeUnitSetupData() 
{
}

void mdtTtTestNodeUnitSetupData::setId(QVariant value) 
{
  pvId = value;
}

QVariant mdtTtTestNodeUnitSetupData::id() const 
{
  return pvId;
}

void mdtTtTestNodeUnitSetupData::setState(QVariant value) 
{
  pvState = value;
}

QVariant mdtTtTestNodeUnitSetupData::state() const 
{
  return pvState;
}

void mdtTtTestNodeUnitSetupData::setValue(QVariant value) 
{
  pvValue = value;
}

QVariant mdtTtTestNodeUnitSetupData::value() const 
{
  return pvValue;
}

void mdtTtTestNodeUnitSetupData::setSchemaPosition(QVariant value)
{
  pvSchemaPosition = value;
}

QVariant mdtTtTestNodeUnitSetupData::schemaPosition() const
{
  return pvSchemaPosition;
}

void mdtTtTestNodeUnitSetupData::setNodeId(QVariant value)
{
  pvNodeId = value;
}

QVariant mdtTtTestNodeUnitSetupData::nodeId() const
{
  return pvNodeId;
}

void mdtTtTestNodeUnitSetupData::setTestModelItemId(QVariant value) 
{
  pvTestModelItemId = value;
}

QVariant mdtTtTestNodeUnitSetupData::testModelItemId() const 
{
  return pvTestModelItemId;
}

void mdtTtTestNodeUnitSetupData::setTestItemId(QVariant value) 
{
  pvTestItemId = value;
}

QVariant mdtTtTestNodeUnitSetupData::testItemId() const 
{
  return pvTestItemId;
}
