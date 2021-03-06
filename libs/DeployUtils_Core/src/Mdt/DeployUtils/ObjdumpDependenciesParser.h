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
#ifndef MDT_DEPLOY_UTILS_OBJDUMP_DEPENDENCIES_PARSER_H
#define MDT_DEPLOY_UTILS_OBJDUMP_DEPENDENCIES_PARSER_H

#include "MdtDeployUtils_CoreExport.h"
#include "Mdt/PlainText/StringConstIterator.h"
#include "Mdt/PlainText/StringRecordList.h"
#include <QString>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  namespace Impl{ namespace Objdump{

  template<typename SourceIterator>
  class DependenciesParserTemplateWindows;

  }} // namespace Impl{ namespace Objdump{

  /*! \brief Objdump dependencies parser
   */
  class MDT_DEPLOYUTILS_CORE_EXPORT ObjdumpDependenciesParser
  {
   public:

    /*! \brief Constructor
     */
    ObjdumpDependenciesParser();

    /*! \brief Destructor
     */
    ~ObjdumpDependenciesParser();

    /*! \brief Prase the objdump output data
     */
    bool parse(const QString & data);

    /*! \brief Get parsed dependencies
     */
    Mdt::PlainText::StringRecordList rawDependencies() const;

   private:

    std::unique_ptr< Impl::Objdump::DependenciesParserTemplateWindows<Mdt::PlainText::StringConstIterator> >mParser;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_OBJDUMP_DEPENDENCIES_PARSER_H
