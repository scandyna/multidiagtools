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
#include "mdtTtTestStepWidget.h"
#include "mdtAlgorithms.h"

mdtTtTestStep::mdtTtTestStep(const std::shared_ptr<mdtTtTestNodeManager> & tnm, mdtTtTestStepWidget* tsw, QObject* parent )
 : QObject(parent),
   pvTestNodeManager(tnm),
   pvWidget(tsw),
   pvAbortSupported(false)
{
  Q_ASSERT(pvTestNodeManager);
  if(pvWidget != nullptr){
    connect(pvWidget, SIGNAL(runAbortButtonClicked()), this, SLOT(runAbort()));
    connect(pvWidget, SIGNAL(destroyed()), this, SLOT(setWidgetNull()));
  }
  reset();
}

void mdtTtTestStep::setTitle(const QString & text)
{
  if(pvWidget != nullptr){
    pvWidget->setTitle(text);
  }
}

void mdtTtTestStep::setMessage(const QString & msg)
{
  if(pvWidget != nullptr){
    pvWidget->setMessage(msg);
  }
}

void mdtTtTestStep::setMessage(const mdtError & error)
{
  QString msg = error.text();
  if(!error.informativeText().isEmpty()){
    msg += "\n" + error.informativeText();
  }
  setMessage(msg);
}

void mdtTtTestStep::clearMessage()
{
  if(pvWidget != nullptr){
    pvWidget->clearMessage();
  }
}

void mdtTtTestStep::setAbortSupported(bool support)
{
  pvAbortSupported = support;
  if(pvWidget != nullptr){
    pvWidget->setAbortSupported(support);
  }
}

void mdtTtTestStep::setRunAbortEnabled(bool enable)
{
  if(pvWidget != nullptr){
    pvWidget->setRunAbortButtonEnabled(enable);
  }
}

void mdtTtTestStep::setRunning()
{
  pvState = State_t::Running;
  if(pvWidget != nullptr){
    pvWidget->setState(State_t::Running);
  }
  emit started(this);
}

void mdtTtTestStep::setFinishedSuccess()
{
  pvState = State_t::Success;
  if(pvWidget != nullptr){
    pvWidget->setState(State_t::Success);
  }
  emit stopped(this);
}

void mdtTtTestStep::setFinishedWarn()
{
  pvState = State_t::Warn;
  if(pvWidget != nullptr){
    pvWidget->setState(State_t::Warn);
  }
  emit stopped(this);
}

void mdtTtTestStep::setFinishedFail()
{
  pvState = State_t::Fail;
  if(pvWidget != nullptr){
    pvWidget->setState(State_t::Fail);
  }
  emit stopped(this);
}

void mdtTtTestStep::reset()
{
  pvState = State_t::Initial;
  if(pvWidget != nullptr){
    pvWidget->setState(State_t::Initial);
  }
  emit stopped(this);
}

void mdtTtTestStep::runAbort()
{
  if(pvState == State_t::Running){
    if(pvAbortSupported){
      emit abortCalled();
    }
  }else{
    emit runCalled();
  }
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

void mdtTtTestStep::setWidgetNull()
{
  pvWidget = nullptr;
}
