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
#ifndef MDT_ENTITY_REPOSITORY_HANDLE_H
#define MDT_ENTITY_REPOSITORY_HANDLE_H

#include <memory>

namespace Mdt{ namespace Entity{

  /*! \brief Provide the facility to share a repository as a value object
   *
   * As example, a repository interface could be defined:
   * \code
   * class AbstractPersonRepository : public Mdt::Entity::AbstractCachedRepository<PersonData>
   * {
   * };
   * \endcode
   *
   * A cached repository has a cache, that should not be copied between several objects.
   *  This repository could be created on the heap and shared between the objects that uses it,
   *  but that needs life time management.
   *
   * RepositoryHandle permit to pass a repository as value object.
   *  Based on a std::shared_ptr , each instance of a handle refers to the same repository.
   *
   * Define a repository interface handle could simply be:
   * \code
   * using PersonRepository = Mdt::Entity::RepositoryHandle<AbstractPersonRepository>;
   * \endcode
   * Note that the handle, %PersonRepository, refers to the interface %AbstractPersonRepository ,
   *  not the any concrete implementation.
   *  This avoids dependencies of any specific implementation of the repository.
   *
   * Lets take a example of a SQL repository implementation:
   * \code
   * class SqlPersonRepository : public AbstractPersonRepository
   * {
   * };
   * \endcode
   *
   * To create a instance of a repository:
   * \code
   * auto personRepository = PersonRepository::make<SqlPersonRepository>();
   * \endcode
   *
   * If the repository inherits AbstractCachedRepository, the handle, PersonRepository,
   *  can be used in the STL compatible table models by using CachedRepositoryStlTableProxy .
   */
  template<typename RepositoryInterface>
  class RepositoryHandle
  {
   public:

    /*! \brief Construct a null repsiotory handle
     */
    RepositoryHandle() = default;

    /*! \brief STL style value type
     */
    using value_type = typename RepositoryInterface::value_type;

    /*! \brief Check if this repository handle is null
     */
    bool isNull() const
    {
      return mRepository.get() == nullptr;
    }

    /*! \brief Access the hold repository
     *
     * \pre This handle must not be null
     */
    const RepositoryInterface & constRepository() const
    {
      Q_ASSERT(!isNull());
      return *( mRepository.get() );
    }

    /*! \brief Access the hold repository
     *
     * \pre This handle must not be null
     */
    RepositoryInterface & repository()
    {
      Q_ASSERT(!isNull());
      return *( mRepository.get() );
    }

    /*! \brief Create a repository handle with a repository implementation
     */
    template<typename Repository>
    static RepositoryHandle make()
    {
      return RepositoryHandle( std::make_shared<Repository>() );
    }

  private:

    RepositoryHandle(std::shared_ptr<RepositoryInterface> && repository)
     : mRepository(repository)
    {
    }

    std::shared_ptr<RepositoryInterface> mRepository;
  };

}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_REPOSITORY_HANDLE_H
