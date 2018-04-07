/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_ENTITY_ABSTRACT_REPOSITORY_H
#define MDT_ENTITY_ABSTRACT_REPOSITORY_H

#include <QtGlobal>
#include <vector>

namespace Mdt{ namespace Entity{

  /*! \brief Base class to create a cached repository
   *
   * \code
   * class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
   * {
   *  public:
   *
   *   bool fetchRecords(int count) override;
   * };
   * \endcode
   */
  template<typename EntityData>
  class AbstractCachedRepository
  {
   public:

    /*! \brief STL style value type
     */
    using value_type = EntityData;

    /*! \brief Construct a repository
     */
    AbstractCachedRepository() = default;

    /*! \brief Destructor
     */
    virtual ~AbstractCachedRepository() = default;

    AbstractCachedRepository(const AbstractCachedRepository &) = delete;
    AbstractCachedRepository & operator=(const AbstractCachedRepository &) = delete;

    AbstractCachedRepository(AbstractCachedRepository &&) = delete;
    AbstractCachedRepository & operator=(AbstractCachedRepository &&) = delete;

    /*! \brief Set maximum rows in cache
     *
     * \sa cachedRowCountLimit()
     */
    void setCachedRowCountLimit(int limit)
    {
      mCachedRowCountLimit = limit;
    }

    /*! \brief Get maximum rows in cache
     */
    int cachedRowCountLimit() const
    {
      return mCachedRowCountLimit;
    }

    /*! \brief Get cout of rows
     *
     * \note Returns the count of rows that are actually in cache
     */
    int rowCount() const
    {
      return mCache.size();
    }

    /*! \brief Get record a \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    const EntityData & constRecordAt(int row) const
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache[row];
    }

    /*! \brief Get record a \a row
     *
     * \pre \a row must be in valid range ( 0 <= \a row < rowCount() )
     */
    EntityData & recordAt(int row)
    {
      Q_ASSERT(row >= 0);
      Q_ASSERT(row < rowCount());
      return mCache[row];
    }

    /*! \brief Fetch all data from the underlaying storage, until defined row count limit
     */
    bool fetchAll()
    {
      mCache.clear();
      return fetchRecords( cachedRowCountLimit() );
    }

   protected:

    /*! \brief Fetch \a count records from the underlaying storage
     *
     * To add the fetched records to the cache, use appendRecordToCache() .
     *
     * If the storage has less recors available than \a count
     *  (or no records at all), this method should return true.
     *  false should be returned on errors, like DB connection failure,
     *  file read errors, etc..
     *
     * Example of a possible, simplified, implementation using QSqlQuery:
     * \code
     * bool fetchRecords(int count) override
     * {
     *   if(!execSelectAll()){
     *     return false;
     *   }
     *   for(int i = 0; i < count; ++i){
     *     if(!mQuery.next()){
     *       return true;
     *     }
     *     addCurrentRecordToCache();
     *   }
     *   return true;
     * }
     * \endcode
     */
    virtual bool fetchRecords(int count) = 0;

    /*! \brief Append a record that come from storage to the cache
     *
     * \pre rowCount() must be < cachedRowCountLimit()
     */
    void appendRecordToCache(const EntityData & record)
    {
      Q_ASSERT(rowCount() < cachedRowCountLimit());
      mCache.push_back(record);
    }

   private:

    std::vector<EntityData> mCache;
    int mCachedRowCountLimit = 5000;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_ABSTRACT_REPOSITORY_H
