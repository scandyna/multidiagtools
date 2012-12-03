/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_FRAME_CODEC_ASCII_H
#define MDT_FRAME_CODEC_ASCII_H

#include "mdtFrameCodec.h"
#include <QString>
#include <QVariant>
#include <QStringList>

class mdtFrameCodecAscii : public mdtFrameCodec
{
 public:

  mdtFrameCodecAscii();
  ~mdtFrameCodecAscii();

  /*! \brief Set the Eond Of Frame Sequence
   * 
   * When using mdtLibrary's communication classes,
   * the endOfFrame sequence is allready removed.
   * So, this setup is optional.
   * \sa mdtFrame
   * \sa mdtPort
   * \sa mdtPortManager
   */
  void setEofSeq(QByteArray &eofSeq);

  /*! \brief Remove whitespaces at begin and end of data frame
   */
  void trim();

  /*! \brief Set frame's data to upper case
   */
  void setCaseUpper();

  /*! \brief Convert a string to first matching type
   */
  QVariant convertData(const QString &data) const;

 protected:

  /*! \brief Clean frame
   *
   * Helper method that removes white spaces at beginning and end of internal
   *  data (see QString::trimmed() ).
   * Next, the EOF seuqence is removed.
   *
   * If a step fails, a message is added to mdtError system.
   *
   * \return true if EOF could be removed, and data size is > 0 after clean.
   */
  bool clean();

  /*! \brief Remove the End Of Frame Sequence
   *
   * Note: this method does nothing if EofSeq was not set
   */
  bool removeEofSeq();

  QString pvAsciiData;
  QByteArray pvEofSeq;  // End Of Frame Squeunce
  QStringList pvNodes;
};

#endif  // #ifndef MDT_FRAME_CODEC_ASCII_H
