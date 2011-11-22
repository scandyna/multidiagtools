
#include "mdtError.h"
#include "mdtErrorOut.h"

mdtError::mdtError()
{
}

mdtError::mdtError(int number, const QString &text, mdtError::level_t level)
{
  pvNumber = number;
  pvText = text;
  pvLevel = level;
  pvSystemNumber = 0;
  pvSystemText = "";
  pvFunctionName = "";
  pvFileName = "";
  pvFileLine = 0;
}

void mdtError::setSystemError(int number, const QString &text)
{
  pvSystemNumber = number;
  pvSystemText = text;
}

void mdtError::setSource(const QString &fileName, int fileLine, const QString &className, const QString &functionName)
{
  pvFileName = fileName;
  pvFileLine = fileLine;
  pvFunctionName = className;
  pvFunctionName += "::";
  pvFunctionName += functionName;
  pvFunctionName += "()";
}

void mdtError::commit()
{
  mdtErrorOut::addError(*this);
}

int mdtError::number()
{
  return pvNumber;
}

QString mdtError::text()
{
  return pvText;
}

mdtError::level_t mdtError::level()
{
  return pvLevel;
}

int mdtError::systemNumber()
{
  return pvSystemNumber;
}

QString mdtError::systemText()
{
  return pvSystemText;
}

QString mdtError::functionName()
{
  return pvFunctionName;
}

QString mdtError::fileName()
{
  return pvFileName;
}

int mdtError::fileLine()
{
  return pvFileLine;
}

