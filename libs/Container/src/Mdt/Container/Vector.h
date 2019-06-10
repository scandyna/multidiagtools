/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_CONTAINER_VECTOR_H
#define MDT_CONTAINER_VECTOR_H

#include <QtGlobal>
#include <vector>
#include <initializer_list>

namespace Mdt{ namespace Container{

  /*! \brief Vector is a wrapper around std::vector that provides a more Mdt and Qt style API
   */
  template<typename T>
  class Vector
  {
   public:

    /*! \brief STL value type
     */
    using value_type = typename std::vector<T>::value_type;

    /*! \brief STL size type
     */
    using size_type = int;

    /*! \brief STL difference type
     */
    using difference_type = typename std::vector<T>::difference_type;

    /*! \brief STL reference
     */
    using reference = typename std::vector<T>::reference;

    /*! \brief STL const reference
     */
    using const_reference = typename std::vector<T>::const_reference;

    /*! \brief STL pointer
     */
    using pointer = typename std::vector<T>::pointer;

    /*! \brief STL const pointer
     */
    using const_pointer = typename std::vector<T>::const_pointer;

    /*! \brief STL iterator
     */
    using iterator = typename std::vector<T>::iterator;

    /*! \brief STL const iterator
     */
    using const_iterator = typename std::vector<T>::const_iterator;

    /*! \brief STL reverse iterator
     */
    using reverse_iterator = typename std::vector<T>::reverse_iterator;

    /*! \brief STL const reverse iterator
     */
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    /*! \brief Construct a empty vector
     */
    Vector() noexcept = default;

    /*! \brief Construct a vector with \a count copies of \a value
     */
    explicit Vector(int count, const T & value)
     : mVector(count, value)
    {
    }

    /*! \brief Construct a vector with the content of \a init
     */
    Vector(std::initializer_list<T> init)
     : mVector(init)
    {
    }

    /*! \brief Copy construct a vector from \a other
     */
    Vector(const Vector & other) = default;

    /*! \brief Copy assign \a other to this vector
     */
    Vector & operator=(const Vector & other) = default;

    /*! \brief Move construct a vector from \a other
     */
    Vector(Vector && other) noexcept = default;

    /*! \brief Move assign \a other to this vector
     */
    Vector & operator=(Vector && other) noexcept = default;

    /*! \brief Get the number of elements in this vector
     */
    int size() const noexcept
    {
      return mVector.size();
    }

    /*! \brief Check if this vector is empty
     */
    bool isEmpty() const noexcept
    {
      return mVector.empty();
    }

    /*! \brief Get the element at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < size() )
     * \note Bound checking is only done by preconditions (i.e. in Debug build)
     */
    const_reference at(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return mVector[index];
    }

    /*! \brief Access the element at \a index for update
     *
     * \pre \a index must be in valid range ( 0 <= \a index < size() )
     * \note Bound checking is only done by preconditions (i.e. in Debug build)
     */
    reference operator[](int index)
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < size());

      return mVector[index];
    }

    /*! \brief Erases all elements from the container
     */
    void clear() noexcept
    {
      mVector.clear();
    }

    /*! \brief Add a element at the end of this vector
     */
    void append(const T & value)
    {
      mVector.push_back(value);
    }

    /*! \brief Add a element at the end of this vector
     */
    void append(T && value)
    {
      mVector.push_back(value);
    }

    /*! \brief Add a element at the end of this vector
     */
    void push_back(const T & value)
    {
      mVector.push_back(value);
    }

    /*! \brief Add a element at the end of this vector
     */
    void push_back(T && value)
    {
      mVector.push_back(value);
    }

    /*! \brief Add a element at the end of this vector
     */
    template<typename... Args>
    void emplace_back(Args&&... args)
    {
      mVector.emplace_back(args...);
    }

    /*! \brief Insert \a count copies of \a value before \a pos
     *
     * Returns a iterator pointing to the first element inserted, or pos if count==0.
     *
     * \pre \a count must be >= 0
     */
    iterator insert(const_iterator pos, int count, const T & value)
    {
      Q_ASSERT(count >= 0);

      return mVector.insert(pos, count, value);
    }

    /*! \brief Get a iterator to the first element of this container
     */
    iterator begin() noexcept
    {
      return mVector.begin();
    }

    /*! \brief Get a const iterator to the first element of this container
     */
    const_iterator begin() const noexcept
    {
      return mVector.cbegin();
    }

    /*! \brief Get a const iterator to the first element of this container
     */
    const_iterator cbegin() const noexcept
    {
      return mVector.cbegin();
    }

    /*! \brief Get a iterator to the element following the last element in this container
     */
    iterator end() noexcept
    {
      return mVector.end();
    }

    /*! \brief Get a const iterator to the element following the last element in this container
     */
    const_iterator end() const noexcept
    {
      return mVector.cend();
    }

    /*! \brief Get a const iterator to the element following the last element in this container
     */
    const_iterator cend() const noexcept
    {
      return mVector.cend();
    }

   private:

    std::vector<T> mVector;
  };

}} // namespace Mdt{ namespace Container{

#endif // #ifndef MDT_CONTAINER_VECTOR_H
