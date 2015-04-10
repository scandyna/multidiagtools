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
#ifndef MDT_SQL_DIALOG_H
#define MDT_SQL_DIALOG_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QVariant>
#include <QList>
#include <QPair>

class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class mdtSqlForm;

/*! \brief Dialog for a database table view/editor
 *
 * This class offers some functionnality for the GUI part.
 *
 * It was designed to work with a subclass of mdtSqlForm .
 */
class mdtSqlDialog : public QDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlDialog();

  /*! \brief Set the SQL form
   *
   * Note: because form will be reparented,
   *       it will be deleted when this dialog is destroyed.
   *
   * \pre form must be a valid pointer .
   */
  void setSqlForm(mdtSqlForm *form);

  /*! \brief Enable edition
   *
   * Will add save/revert buttons
   *
   * \pre Form must be set with setSqlForm() before call this method.
   */
  void enableEdition();

  /*! \brief Disable edition
   *
   * \sa enableEdition()
   */
  void disableEdition();

 public slots:

  /*! \brief Overloads QDialog::accept()
   */
  void accept();

  /*! \brief Overloads QDialog::reject()
   */
  void reject();

 private:

  /*! \brief Create edition elements
   */
  void createEditionElements(QHBoxLayout *l);

  /*! \brief Connect edition elements
   *
   * \pre pvForm must be valid
   */
  void connectEditionElements();

  /*! \brief Disconnect edition elements
   *
   * \pre pvForm must be valid
   */
  void disconnectEditionElements();

  Q_DISABLE_COPY(mdtSqlDialog);

  QVBoxLayout *pvMainLayout;
  QPushButton *pbSubmit;
  QPushButton *pbRevert;
  mdtSqlForm *pvForm;
};

#endif  // #ifndef MDT_SQL_DIALOG_H
