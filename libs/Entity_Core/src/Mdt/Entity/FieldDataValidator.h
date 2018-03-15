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
#ifndef MDT_ENTITY_FIELD_DATA_VALIDATOR_H
#define MDT_ENTITY_FIELD_DATA_VALIDATOR_H

#include "FieldDataValidatorState.h"
#include "FieldDataValidatorCommonImplementation.h"
#include "FieldAttributes.h"
#include "TypeTraits/IsEntityFieldDef.h"
#include "Mdt/Error.h"
#include <QString>

namespace Mdt{ namespace Entity{

  namespace ImplNs{ namespace FieldDataValidator{

    Mdt::Error toGenericError(FieldDataValidatorState state, const QString & fieldName);

  }} // namespace Impl{ namespace FieldDataValidator{

  /*! \brief Validate field data rebargding field attributes
   *
   * Example with %PersonData defined using DataTemplate :
   * \code
   * PersonData data;
   * FieldDataValidator<> validator;  // Use the default implementation
   *
   * if(!validator.validateData( data.firstName(), data.def().firstName() ){
   *   auto error = validator.toGenericError();
   * }
   * \endcode
   *
   * To add support for custom types, it is possible to provide a specific implementation.
   *  A implementation is a class with a %validateData() method:
   * \code
   * class MyCustomFieldDataValidatorImplementation : public Mdt::Entity::FieldDataValidatorCommonImplementation
   * {
   *  public:
   *
   *   using Mdt::Entity::FieldDataValidatorCommonImplementation::validateData;
   *
   *   Mdt::Entity::FieldDataValidatorState %validateData(const MyDataType & data, const Mdt::Entity::FieldAttributes & fieldAttributes) const;
   * };
   * \endcode
   *
   * The above example becomes:
   * \code
   * PersonData data;
   * FieldDataValidator<MyCustomFieldDataValidatorImplementation> validator;
   *
   * if(!validator.validateData( data.firstName(), data.def().firstName() ){
   *   auto error = validator.toGenericError();
   * }
   * \endcode
   */
  template<typename Impl = FieldDataValidatorCommonImplementation>
  class FieldDataValidator
  {
   public:

    /*! \brief Validate \a data resgarding \a fieldDef
     */
    template<typename T, typename FieldDef>
    bool validateData(const T & data, const FieldDef & fieldDef)
    {
      static_assert( TypeTraits::IsEntityFieldDef<FieldDef>::value, "fieldDef must be a entity field definition" );

      mState = mImpl.validateData(data, fieldDef.fieldAttributes());
      if(mState == FieldDataValidatorState::Ok){
        return true;
      }
      mFieldName = fieldDef.fieldName();

      return false;
    }

    /*! \brief Get the state of this validator
     */
    FieldDataValidatorState state() const noexcept
    {
      return mState;
    }

    /*! \brief Get a genric error regarding state of this validator
     */
    Mdt::Error toGenericError() const
    {
      return ImplNs::FieldDataValidator::toGenericError(mState, mFieldName);
    }


   private:

    Impl mImpl;
    FieldDataValidatorState mState = FieldDataValidatorState::Ok;
    FieldAttributes mFieldAttributes;
    QString mFieldName;
  };
  
}} // namespace Mdt{ namespace Entity{

#endif // #ifndef MDT_ENTITY_FIELD_DATA_VALIDATOR_H
