/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#ifndef MDT_REFLECTION_REFLECT_FIELD_H
#define MDT_REFLECTION_REFLECT_FIELD_H

#include "FieldAttributes.h"
#include <boost/preprocessor/config/config.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/to_array.hpp>
#include <boost/preprocessor/array/enum.hpp>
#include <boost/preprocessor/array/pop_front.hpp>

#if !BOOST_PP_VARIADICS
 #error "The macro MDT_REFLECT_FIELD() requires a compiler that supports preprocessor variadics"
#endif

namespace Mdt{ namespace Reflection{

}} // namespace Mdt{ namespace Reflection{

#endif // #ifndef MDT_REFLECTION_REFLECT_FIELD_H
