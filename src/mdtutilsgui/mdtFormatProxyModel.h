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
#ifndef MDT_FORMAT_PROXY_MODEL_H
#define MDT_FORMAT_PROXY_MODEL_H

#include "mdtFormatProxyModelSettings.h"
#include <QIdentityProxyModel>
#include <QVariant>
#include <QModelIndex>

/*! \brief Apply formats to Qt's model/view
 */
class mdtFormatProxyModel : public QIdentityProxyModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtFormatProxyModel(QObject *parent = nullptr);

  /*! \brief Set settings
   */
  void setSettings(const mdtFormatProxyModelSettings & settings);

  /*! \brief Get current settings
   */
  mdtFormatProxyModelSettings settings() const
  {
    return pvSettings;
  }

  /*! \brief Clear settings
   */
  void clearSettings();

  /*! \brief Get data
   */
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

 private:

  /*! \brief Get background role data
   */
  QVariant backgroundRoleData(int row) const;

  Q_DISABLE_COPY(mdtFormatProxyModel)

  mdtFormatProxyModelSettings pvSettings;
};

#endif // #ifndef MDT_FORMAT_PROXY_MODEL_H
