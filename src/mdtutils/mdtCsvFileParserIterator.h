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
#ifndef MDT_CSV_FILE_PARSER_ITERATOR_H
#define MDT_CSV_FILE_PARSER_ITERATOR_H

#include "mdtCsvFileParserIteratorSharedData.h"
#include "mdtError.h"
#include <QIODevice>
#include <QByteArray>
#include <memory>

/*! \brief Iterator that acts on a I/O device
 *
 * This iterator is used by mdtCsvFileParser.
 *
 * mdtCsvFileParserIterator is a single-pass input iterator
 *  that reads data from a QIODevice.
 *  When the iterator is incremented, it will take
 *  a char from its internal buffer, witch is handled
 *  by mdtCsvFileParserIteratorSharedData.
 *  When no data is available anymore in the buffer,
 *  a chunck of data is read from QIODevice and encoded
 *  to unicode.
 *
 */
struct mdtCsvFileParserIterator
{
  static_assert(sizeof(wchar_t) >= 2, "wchar_t is < 16 bit");

  typedef wchar_t value_type;
  typedef std::ptrdiff_t difference_type;
  typedef const value_type & reference;
  typedef const value_type* pointer;
  typedef std::input_iterator_tag iterator_category;

  /*! \brief Constructs a end-of-stream iterator.
   */
  mdtCsvFileParserIterator()
   : pvErrorOccured(false)
  {
  }

  /*! \brief Construct a iterator that acts on device
   *
   * Will use mdtCsvFileParserIteratorSharedData::setSource().
   *  On success, a first char is read from device, decoded, then stored.
   *  On failure, this iterator falls back to a end-of-stream iterator,
   *  and error flag is set.
   *  On success, it can also happen that the device is allready
   *  at end. In this case, this iterator also falls back
   *  to a end-of-stream iterator.
   *
   * \sa mdtCsvFileParserIteratorSharedData::setSource()
   * \sa errorOccured()
   */
  mdtCsvFileParserIterator(QIODevice *device, const QByteArray & encoding)
   : pvErrorOccured(false),
     pvShared(new mdtCsvFileParserIteratorSharedData)
  {
    pvErrorOccured = !pvShared->setSource(device, encoding);
    if(pvErrorOccured){
      pvShared.reset();
      return;
    }
    // Check if device is allready at end
    if(pvShared->atEnd()){
      pvShared.reset();
    }
  }

  /*! \brief Copy on base of other iterator
   */
  // Thanks to std::shared_ptr we can let do the compiler
  mdtCsvFileParserIterator(const mdtCsvFileParserIterator & other) = default;

  /*! \brief Move constructor
   */
  // Thanks to std::shared_ptr we can let do the compiler
  mdtCsvFileParserIterator(mdtCsvFileParserIterator && other) = default;

  /*! \brief Assign other iterator
   */
  // Thanks to std::shared_ptr we can let do the compiler
  mdtCsvFileParserIterator & operator=(const mdtCsvFileParserIterator & other) = default;

  /*! \brief Increment iterator (pre-increment)
   *
   * Increment the internal unicode buffer by one.
   *  If buffer is empty,
   *  a chunck of data is also read from device
   *  and decoded into unicode buffer.
   *
   * If a error occures (typically while reading from device),
   *  the error flag is set.
   *
   * \pre This iterator must not be a end-of-stream iterator
   * \sa errorOccured()
   */
  mdtCsvFileParserIterator & operator++()
  {
    Q_ASSERT(!isEof());
    Q_ASSERT(pvShared);

    pvErrorOccured = !pvShared->advance();

    return *this;
  }

  /*! \brief Increment iterator (post-increment)
   *
   * \pre This iterator must not be a end-of-stream iterator
   * \sa operator++()
   */
  mdtCsvFileParserIterator operator++(int)
  {
    Q_ASSERT(!isEof());

    mdtCsvFileParserIterator tmp(*this);
    ++*this;

    return tmp;
  }

  /*! \brief Get last read value
   *
   * Note: if a error occured (typically while reading from device),
   *  the unicode uncertainty sign (0x2BD1) is returned.
   *
   * \pre This iterator must not be a end-of-stream iterator
   * \sa operator++()
   */
  value_type operator*() const
  {
    Q_ASSERT(!isEof());
    if(pvErrorOccured){
      return 0x2BD1;
    }
    return pvShared->get().unicode();
  }

  /*! \brief Returns true if a and b are EOF iterators, or a and b are valid iterators
   */
  friend
  bool operator==(const mdtCsvFileParserIterator & a, const mdtCsvFileParserIterator & b)
  {
    return (a.isEof() == b.isEof());
  }

  /*! \brief See operator==()
   */
  friend
  bool operator!=(const mdtCsvFileParserIterator & a, const mdtCsvFileParserIterator & b)
  {
    return !(a == b);
  }

  /*! \brief Check if this iterator is a end-of-stream iterator
   *
   * Returns true if this is a default constructed
   *  iterator (no device was set),
   *  or device and iternal buffer are both at end.
   */
  bool isEof() const
  {
    if(!pvShared){
      return true;
    }
    return pvShared->atEnd();
  }

  /*! \brief Check if a error occured
   *
   * \sa lastError()
   */
  bool errorOccured() const
  {
    return pvErrorOccured;
  }

  /*! \brief Get last error
   *
   * \pre this must be a iterator attached to a device
   * \sa errorOccured()
   */
  mdtError lastError() const
  {
    Q_ASSERT(pvShared);
    return pvShared->lastError();
  }

 private:

  bool pvErrorOccured;
  std::shared_ptr<mdtCsvFileParserIteratorSharedData> pvShared;
};

/*! \brief \todo Multipass iterator
 *
 * Name: mdtCsvFileParserMultiPassIterator ?
 */


#endif // #ifndef MDT_CSV_FILE_PARSER_ITERATOR_H
