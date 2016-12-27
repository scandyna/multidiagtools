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
#ifndef CLIENT_EDITOR_SCHEMA_H
#define CLIENT_EDITOR_SCHEMA_H

#include "Mdt/Sql/Schema/SchemaTemplate.h"

/*! \brief Database schema for client editor
 */
class ClientEditorSchema : public Mdt::Sql::Schema::SchemaTemplate<ClientEditorSchema>
{
 public:

  ClientEditorSchema();
};

#endif // #ifndef CLIENT_EDITOR_SCHEMA_H
