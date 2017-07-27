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
#include "Platform.h"
#include <QtGlobal>

namespace Mdt{ namespace DeployUtils{

Platform::Platform(OperatingSystem os, Compiler compiler, Processor processor)
 : mOperatingSystem(os),
   mCompiler(compiler),
   mProcessor(processor)
{
}

OperatingSystem Platform::nativeOperatingSystem()
{
#ifdef Q_OS_LINUX
  return OperatingSystem::Linux;
#else
 #error "Current OS is not supported"
#endif // OS
}

Compiler Platform::nativeCompiler()
{
#ifdef Q_CC_GNU
  return Compiler::Gcc;
#else
 #error "Current compiler is not supported"
#endif // Compiler
}

Processor Platform::nativeProcessor()
{
#ifdef Q_PROCESSOR_X86_32
  return Processor::X86_32;
#elif defined Q_PROCESSOR_X86_64
  return Processor::X86_64;
#else
 #error "Current processor is not supported"
#endif // Compiler
}


}} // namespace Mdt{ namespace DeployUtils{
