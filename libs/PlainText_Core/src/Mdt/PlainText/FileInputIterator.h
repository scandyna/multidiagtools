/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_PLAIN_TEXT_FILE_INPUT_ITERATOR_H
#define MDT_PLAIN_TEXT_FILE_INPUT_ITERATOR_H

#include "Mdt/Error.h"
#include <QIODevice>
#include <QByteArray>
#include <memory>

namespace Mdt{ namespace PlainText{

  class FileInputIteratorSharedData;

  /*! \brief Iterator that acts on a I/O device
   *
   * This iterator can be used by with Boost Spirit multipass_iterator.
   *
   * FileInputIterator is a single-pass input iterator
   *  that reads data from a QIODevice.
   *  When the iterator is incremented, it will take
   *  a char from its internal buffer, witch is handled
   *  by FileInputIteratorSharedData.
   *  When no data is available anymore in the buffer,
   *  a chunck of data is read from QIODevice and decoded
   *  to unicode.
   *
   */
  struct FileInputIterator
  {
    static_assert(sizeof(wchar_t) >= 2, "wchar_t is < 16 bit");

    typedef wchar_t value_type;
    typedef std::ptrdiff_t difference_type;
    typedef const value_type & reference;
    typedef const value_type* pointer;
    typedef std::input_iterator_tag iterator_category;

    /*! \brief Constructs a end-of-stream iterator.
     */
    FileInputIterator()
    {
    }

    /*! \brief Construct a iterator that acts on device
     *
     * Will use FileInputIteratorSharedData::setSource().
     *  On success, a first char is read from device, decoded, then stored.
     *  On failure, this iterator falls back to a end-of-stream iterator,
     *  and error flag is set.
     *  On success, it can also happen that the device is allready
     *  at end. In this case, this iterator also falls back
     *  to a end-of-stream iterator.
     *
     * \sa FileInputIteratorSharedData::setSource()
     * \sa errorOccured()
     */
    FileInputIterator(QIODevice *device, const QByteArray & encoding);

    /*! \brief Copy on base of other iterator
     */
    FileInputIterator(const FileInputIterator & other) = default;

    /*! \brief Move constructor
     */
    FileInputIterator(FileInputIterator && other) = default;

    /*! \brief Assign other iterator
     */
    FileInputIterator & operator=(const FileInputIterator & other) = default;

    /*! \brief Set source
     *
     * Will use FileInputIteratorSharedData::setSource().
     *  On success, a first char is read from device, decoded, then stored.
     *  On failure, this iterator falls back to a end-of-stream iterator,
     *  and error flag is set.
     *  On success, it can also happen that the device is allready
     *  at end. In this case, this iterator also falls back
     *  to a end-of-stream iterator.
     *
     * \sa FileInputIteratorSharedData::setSource()
     * \sa errorOccured()
     */
    bool setSource(QIODevice *device, const QByteArray & encoding);

    /*! \brief Clear
     *
     * After calling this function,
     *  this iterator becomes a end-of-stream iterator
     */
    void clear();

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
    FileInputIterator & operator++();

    /*! \brief Increment iterator (post-increment)
     *
     * \pre This iterator must not be a end-of-stream iterator
     * \sa operator++()
     */
    FileInputIterator operator++(int)
    {
      Q_ASSERT(!isEof());

      FileInputIterator tmp(*this);
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
    reference operator*();

    /*! \brief Returns true if a and b are EOF iterators, or a and b are valid iterators
     */
    friend
    bool operator==(const FileInputIterator & a, const FileInputIterator & b)
    {
      return (a.isEof() == b.isEof());
    }

    /*! \brief See operator==()
     */
    friend
    bool operator!=(const FileInputIterator & a, const FileInputIterator & b)
    {
      return !(a == b);
    }

    /*! \brief Check if this iterator is a end-of-stream iterator
     *
     * Returns true if this is a default constructed
     *  iterator (no device was set),
     *  or device and iternal buffer are both at end.
     */
    bool isEof() const;

    /*! \brief Check if a error occured
     *
     * \sa lastError()
     */
    bool errorOccured() const
    {
      return mErrorOccured;
    }

    /*! \brief Get last error
     *
     * \pre this must be a iterator attached to a device
     * \sa errorOccured()
     */
    Mdt::Error lastError() const;

   private:

    /*! \brief Init
     *
     * \sa setSource()
     */
    bool init(QIODevice *device, const QByteArray & encoding);

    bool mErrorOccured = false;
    value_type mCurrentChar = '\0';
    std::shared_ptr<FileInputIteratorSharedData> mShared;
  };

}} // namespace Mdt{ namespace PlainText{

#endif // #ifndef MDT_PLAIN_TEXT_FILE_INPUT_ITERATOR_H
