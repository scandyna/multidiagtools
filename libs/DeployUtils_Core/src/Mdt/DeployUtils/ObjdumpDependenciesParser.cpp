/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "ObjdumpDependenciesParser.h"
#include "Impl/Objdump/DependenciesParserImplWindows.h"

using namespace Mdt::PlainText;

namespace Mdt{ namespace DeployUtils{

ObjdumpDependenciesParser::ObjdumpDependenciesParser()
 : mParser( std::make_unique< Impl::Objdump::DependenciesParserTemplateWindows<StringConstIterator> >() )
{
}

ObjdumpDependenciesParser::~ObjdumpDependenciesParser()
{

}

bool ObjdumpDependenciesParser::parse(const QString& data)
{
  StringConstIterator first = data.cbegin();
  StringConstIterator last = data.cend();

  return mParser->parseAll(first, last);
}

PlainText::StringRecordList ObjdumpDependenciesParser::rawDependencies() const
{
  return mParser->rawDependencies();
}


}} // namespace Mdt{ namespace DeployUtils{
