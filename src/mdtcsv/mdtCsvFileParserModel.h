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
#ifndef MDT_CSV_FILE_PARSER_MODEL_H
#define MDT_CSV_FILE_PARSER_MODEL_H

#include "mdtCsvParserModel.h"
#include "mdtCsvFileParser.h"
#include "mdtCsvSettings.h"
#include "mdtExpected.h"
#include "mdtError.h"
#include <QFileInfo>
#include <QByteArray>

#include <memory>

/*! \brief Item model to display data while parsing a CSV file
 */
class mdtCsvFileParserModel : public mdtCsvParserModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtCsvFileParserModel(QObject *parent = nullptr);

  // Destructor (required because mdtCsvFileParser is forward decalred)
  ~mdtCsvFileParserModel();

  // Copy disabled
  mdtCsvFileParserModel(const mdtCsvFileParserModel &) = delete;
  mdtCsvFileParserModel & operator=(const mdtCsvFileParserModel &) = delete;

  // Move disabled
  mdtCsvFileParserModel(mdtCsvFileParserModel &&) = delete;
  mdtCsvFileParserModel & operator=(mdtCsvFileParserModel &&) = delete;

  /*! \brief Set CSV settings
   *
   * If a file is allready open,
   *  it will be closed first.
   *  If a file was allready set with setFile(),
   *  it will be open with given CSV settings.
   *
   * \pre csvSettings must be valid
   */
  bool setCsvSettings(const mdtCsvParserSettings & csvSettings);

  /*! \brief Set CSV file
   *
   * If CSV settings was allready set,
   *  file will be open (or re-open if one was allready open)
   */
  bool setFile(const QFileInfo & fileInfo, const QByteArray & fileEncoding);

  /*! \brief Return true if more data is available in CSV file
   */
  bool canFetchMore(const QModelIndex & parent) const override;

  /*! \brief Fetch data
   */
  void fetchMore(const QModelIndex & parent) override;

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

 private:

  /*! \brief Open or re-open SCV file
   */
  bool openFile();

  mdtCsvFileParser pvParser;
  QFileInfo pvFileInfo;
  QByteArray pvFileEncoding;
  mdtCsvParserSettings pvCsvSettings;
  mdtError pvLastError;
};

#endif // #ifndef MDT_CSV_FILE_PARSER_MODEL_H
