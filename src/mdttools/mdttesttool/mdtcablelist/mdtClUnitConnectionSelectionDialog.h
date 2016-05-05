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
#ifndef MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H
#define MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H

#include "mdtError.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtClUnitConnectionKeyData.h"
#include "mdtClUnitConnectionData.h"
#include "Mdt/CableList/UnitConnectionPkList.h"
#include "mdtClUnitConnectorKeyData.h"
#include "mdtClConnectableCriteria.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QLocale>
#include <QList>

using Mdt::CableList::UnitConnectionPkList; /// \todo Remove once migrated

/*! \brief Dialog for unit connection selection (in UnitConnection_tbl)
 */
class mdtClUnitConnectionSelectionDialog : public mdtSqlSelectionDialog
{
 public:

  /*! \brief Unit connection usage in link filter
   */
  enum class LinkUsage_t
  {
    NotUsedAsConnectionStart, /*!< List only unit connections that are not used in link table as unit connection start */
    NotUsedAsConnectionEnd,   /*!< List only unit connections that are not used in link table as unit connection end */
    NotUsed,                  /*!< List only unit connections that are not used in link table */
    All                       /*!< Don't care about link, list all unit connections */
  };

  /*! \brief Constructor
   */
  mdtClUnitConnectionSelectionDialog(QWidget *parent, const QLocale & locale = QLocale());

  /*! \brief Select unit connections that are part of given unit
   */
  bool select(QSqlDatabase db, const QVariant & unitId, LinkUsage_t lu, bool allowMultiSelection);

  /*! \brief Select unit connections that are part of given unit and also part of given unit connectors
   */
  bool select(QSqlDatabase db, const QVariant & unitId, QList<mdtClUnitConnectorPkData> ucnrPkList, LinkUsage_t lu, bool allowMultiSelection);

  /*! \brief Select unit connections that are part of given unit and that are connectable to given connection regarding given criteria
   */
  bool select(QSqlDatabase db, const QVariant & unitId, UnitConnectionPk connectableToPk, const mdtClConnectableCriteria & criteria, bool allowMultiSelection);

  /*! \brief Get selected unit connection PK
   *
   * If user rejected the dialog, a null PK is returned.
   */
  UnitConnectionPk selectedUnitConnectionPk() const;

  /*! \brief Get a list of selected connection PK
   *
   * If user rejected the dialog, a empty list is returned.
   */
  UnitConnectionPkList selectedUnitConnectionPkList() const;

  /*! \brief Get selected unit connection key
   *
   * If user rejected the dialog, a null key is returned.
   */
  mdtClUnitConnectionKeyData selectedUnitConnectionKey() const;

  /*! \brief Get a list of selected connection key
   *
   * If user rejected the dialog, a empty list is returned.
   */
  QList<mdtClUnitConnectionKeyData> selectedUnitConnectionKeyList() const;

 private:

  /*! \brief Set SQL query and other setup
   */
  bool setQuery(const QString & sql, QSqlDatabase & db, bool allowMultiSelection);

  /*! \brief SQL base statement
   */
  QString baseSqlStatement() const;

  /*! \brief Get clause regarding link usage
   */
  QString linkUsageClause(LinkUsage_t lu, const QVariant & unitId) const;

  /*! \brief Build unit connection key data
   */
  mdtClUnitConnectionKeyData buildKeyData(const mdtSqlTableSelection & s, int row) const;

  Q_DISABLE_COPY(mdtClUnitConnectionSelectionDialog);

  QLocale pvLocale;
};

#endif // #ifndef MDT_CL_UNIT_CONNECTION_SELECTION_DIALOG_H

