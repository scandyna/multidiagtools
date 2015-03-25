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
#include "mdtTtTestStep.h"
#include "mdtAlgorithms.h"

mdtTtTestStep::mdtTtTestStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget* tsw, QObject* parent )
 : QObject(parent),
   pvTestNodeManager(tnm),
   pvWidget(tsw)
{
  Q_ASSERT(pvTestNodeManager);
}


bool mdtTtTestStep::isInRange(const mdtValueDouble & x, double min, double max, const QString & valueUnit)
{
  if(!mdtAlgorithms::isInRange(x.value(), min, max, true, true)){
    QString msg;
    pvLastError.setError(tr("Range checking failed for given value."), mdtError::Error);
//     msg  = tr("Value") + " " + QString::number(x.value()) + " " + tr("is out of range.") + " ";
//     msg += tr("Allowed range is: ") + "[" + QString::number(min) + ";" + QString::number(max) + "]";
    msg  = tr("Value %1 %2 is out of range.").arg(x.value(), 0, 'g', 2).arg(valueUnit) + " ";
    msg += tr("Allowed range is from %1 to %2 %3 [%1;%2] %3").arg(min, 0, 'g', 2).arg(max, 0, 'g', 2).arg(valueUnit);
    pvLastError.setInformativeText(msg);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtBasicTestNodeCalibrationTool");
    pvLastError.commit();
    return false;
  }

  return true;
}
