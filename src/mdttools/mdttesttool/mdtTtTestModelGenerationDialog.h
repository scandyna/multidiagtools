/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_TT_TEST_MODEL_GENERATION_DIALOG_H
#define MDT_TT_TEST_MODEL_GENERATION_DIALOG_H

#include "ui_mdtTtTestModelGenerationDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QString>
#include <QVariant>
#include <QSqlRecord>

class QWidget;

/*! \brief
 */
class mdtTtTestModelGenerationDialog : public QDialog, Ui::mdtTtTestModelGenerationDialog
{
  Q_OBJECT

 public:

  /*! \brief Test type
   */
  enum testType_t {
                  None,               /*!< No test */
                  ContinuityTest,     /*!< Generate a continuity test (2 wire) */
                  ShortDetectionTest  /*!< Generate a short detetcion test */
                  };

  /*! \brief Constructor
   */
  mdtTtTestModelGenerationDialog(QSqlDatabase db, QWidget *parent = 0);

  /*! \brief Set test model ID
   */
  void setTestModelId(const QVariant & testModelId);

  /*! \brief Get selected test type
   */
  testType_t selectedTestModelType() const;

  /*! \brief Get selected test node ID
   */
  QVariant selectedTestNodeId() const;

  /*! \brief Get selected test cable ID
   */
  QVariant selectedTestCableId() const;

  /*! \brief Get selected measure connection A ID
   */
  QVariant selectedMeasureConnectionA() const;

  /*! \brief Get selected measure connection B ID
   */
  QVariant selectedMeasureConnectionB() const;

 private slots:

  /*! \brief Populate test cable combo box
   */
  void populateTestCableComboBox(int index);

  /*! \brief Populate measure connection A combo box
   */
  void populateMeasureConnectionAComboBox(int index);

  /*! \brief Populate measure connection B combo box
   */
  void populateMeasureConnectionBComboBox(int index);

 private:

  /*! \brief Populate test type combo box
   */
  void populateTestTypeComboBox();

  /*! \brief Populate test node combo box
   */
  void populateTestNodeComboBox(const QVariant & testModelId);

  /*! \brief Get measure connections data
   */
  void getMeasureConnectionData(const QVariant & testNodeId);

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  Q_DISABLE_COPY(mdtTtTestModelGenerationDialog);

  QSqlDatabase pvDatabase;
  QList<QSqlRecord> pvMeasureConnectionDataList;
};

#endif  // #ifndef MDT_TT_TEST_MODEL_GENERATION_DIALOG_H
