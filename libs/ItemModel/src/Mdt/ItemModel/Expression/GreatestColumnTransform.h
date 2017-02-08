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
#ifndef MDT_ITEM_MODEL_EXPRESSION_GREATEST_COLUMN_TRANSFORM_H
#define MDT_ITEM_MODEL_EXPRESSION_GREATEST_COLUMN_TRANSFORM_H

#include "../FilterColumn.h"
#include "../ParentModelColumn.h"
#include "../RelationKey.h"
#include "ColumnTerminal.h"
#include <QVariant>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>
#include <type_traits>

#include <boost/proto/fusion.hpp>
#include <boost/proto/proto.hpp>

#include <QDebug>
#include <iostream>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  struct TraverseTreeExpression
  {
    template<typename Expr>
    int operator()(const Expr & expr) const
    {
      return impl(expr, boost::mpl::long_<boost::proto::arity_of<Expr>::value>());
    }

   private:

    template<typename Expr>
    int impl(const Expr & expr, boost::mpl::long_<0>) const
    {
      qDebug() << "Arity: 0, -> Terminal ?";
      return displayTerminal(boost::proto::value(expr));
    }

    template<typename Expr, typename Arity>
    int impl(const Expr & expr, Arity a) const
    {
      qDebug() << "Arity: " << a;
      TraverseTreeExpression tte;
      boost::fusion::for_each(expr, tte);
      return 0;
    }

    template<typename T>
    int displayTerminal(const T &) const
    {
      qDebug() << "Unknown terminal";
      return 0;
    }

    int displayTerminal(const FilterColumnData & fc) const
    {
      qDebug() << "FC(" << fc.columnIndex() << ")";
      return fc.columnIndex();
    }
  };
  
  
  struct MyCallableContext : boost::proto::callable_context<MyCallableContext, boost::proto::null_context>
  {
    typedef void result_type;

    template<typename T>
    void operator()(boost::proto::tag::terminal, const T & t)
    {
      qDebug() << "MCC: terminal";
      ///evalTerminal(boost::proto::value(t));
      evalTerminal(t);
    }
    
    int getVal() const
    {
      return mVal;
    }
    
  private:
    
    void evalTerminal(const FilterColumnData & fcd)
    {
      qDebug() << "TCD(" << fcd.columnIndex() << ")";
      mVal = fcd.columnIndex();
    }

    template<typename T>
    void evalTerminal(const T &)
    {
      qDebug() << "Unknown terminal";
    }
    
    int mVal = -1;
  };
  
  /*! \brief Data container for GreatestColumnTransform and GreatestParentModelColumnTransform
   */
  struct GreatestColumnData
  {
    int column = -1;
    int parentModelColumn = -1;
  };

  /*! \brief Callable to get greatest column
   */
  struct GetGreatestColumn : boost::proto::callable
  {
    /*
     * To be able to use boost::proto::_default,
     * we must provide standard C++ binary opertaors return type
     * (which is bool)
     */
    typedef bool result_type;

    /*
     * When using boost::proto::deep_copy(),
     * terminal types can not be deduced.
     * This is why we use templates.
     */
    template<typename L, typename R, typename D>
    bool operator()(const L & column, const R & right, D & currentGreatestColumn) const
    {
      setGreatestColumn( boost::proto::value(column), boost::proto::value(right), currentGreatestColumn);
      return false;  // Force evaluation of the complete expression
    }

   private:

    static void setGreatestColumn(const FilterColumnData & columnData, const QVariant &, GreatestColumnData & currentGreatestColumn);
    static void setGreatestColumn(const FilterColumnData & columnData, const ParentModelColumnData & parentModelColumnData, GreatestColumnData & currentGreatestColumn);
  };

  struct GetGreatestColumnInComparison : boost::proto::or_<
                                          boost::proto::when<
                                            boost::proto::equal_to<boost::proto::_, boost::proto::_ >,
                                            boost::proto::call< GetGreatestColumn(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::not_equal_to<boost::proto::_, boost::proto::_ >,
                                            boost::proto::call< GetGreatestColumn(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::less_equal<boost::proto::_, boost::proto::_ >,
                                            boost::proto::call< GetGreatestColumn(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::less<boost::proto::_, boost::proto::_ >,
                                            boost::proto::call< GetGreatestColumn(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::greater_equal<boost::proto::_, boost::proto::_ >,
                                            boost::proto::call< GetGreatestColumn(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                          > ,
                                          boost::proto::when<
                                            boost::proto::greater<boost::proto::_, boost::proto::_ >,
                                            boost::proto::call< GetGreatestColumn(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                          >
                                        >
  {
  };

  /// \todo See boost::proto::display_expr() in debug.hpp to see how to traverse s tree

  /*! \brief Get greatest column in a filter expression
   */
  struct GreatestColumnTransform : boost::proto::or_<
                                    GetGreatestColumnInComparison ,
                                    /*
                                    boost::proto::when<
                                      boost::proto::logical_or<boost::proto::_, boost::proto::_> ,
                                      GetGreatestColumnInComparison( GreatestColumnTransform(boost::proto::_left), GreatestColumnTransform(boost::proto::_right) )
                                    > ,
                                    boost::proto::when<
                                      boost::proto::logical_and<boost::proto::_, boost::proto::_> ,
                                      GetGreatestColumnInComparison( GreatestColumnTransform(boost::proto::_left), GreatestColumnTransform(boost::proto::_right) )
                                    >
                                    */
                                    /*,
                                    boost::proto::when<
                                      boost::proto::logical_and<boost::proto::_, boost::proto::_> ,
                                      boost::proto::call<GetGreatestColumnInComparison(boost::proto::_left, boost::proto::_right)>
                                    > , */
                                    boost::proto::otherwise<
                                      boost::proto::_default<GreatestColumnTransform>
                                    >
                                   >
  {
  };

//   /*! \brief Get greatest parent model column in a filter expression
//    */
//   struct GreatestParentModelColumnTransform
//   {
//   };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_GREATEST_COLUMN_TRANSFORM_H
