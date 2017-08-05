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
#ifndef MDT_DEPLOY_UTILS_OBJDUMP_BINARY_FORMAT_PARSER_H
#define MDT_DEPLOY_UTILS_OBJDUMP_BINARY_FORMAT_PARSER_H

#include "OperatingSystem.h"
#include "Processor.h"
#include "Mdt/PlainText/StringConstIterator.h"

// #include "Mdt/PlainText/StringRecordList.h"

#include <QString>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  namespace Impl{ namespace Objdump{

  template<typename SourceIterator>
  class FormatParserTemplate;

  }} // namespace Impl{ namespace Objdump{

  /*! \brief Objdump binary format parser
   */
  class ObjdumpBinaryFormatParser
  {
   public:

    /*! \brief Constructor
     */
    ObjdumpBinaryFormatParser();

    /*! \brief Destructor
     */
    ~ObjdumpBinaryFormatParser();

    /*! \brief Prase the objdump output data
     */
    bool parse(const QString & data);

    /*! \brief Get operating system
     *
     * Return only a valid value after parse() succeded
     */
    OperatingSystem operatindSystem() const
    {
      return mOperatingSystem;
    }

    /*! \brief Get processor
     *
     * Return only a valid value after parse() succeded
     */
    Processor processor() const
    {
      return mProcessor;
    }

   private:

    bool setOperatingSystem(const QString & os);
    static bool isOsLinux(const QString & os);
    static bool isOsWindows(const QString & os);
    bool setProcessor(const QString & processor);
    static bool compareEqual(const QString & str, const char * const cs);

    OperatingSystem mOperatingSystem;
    Processor mProcessor;
    std::unique_ptr< Impl::Objdump::FormatParserTemplate<Mdt::PlainText::StringConstIterator> >mParser;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_OBJDUMP_BINARY_FORMAT_PARSER_H
