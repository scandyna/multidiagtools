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
#ifndef MDT_ITEM_MODEL_EXPRESSION_COMPARISON_EVAL_H
#define MDT_ITEM_MODEL_EXPRESSION_COMPARISON_EVAL_H

#include "../FilterColumn.h"
#include "../ParentModelColumn.h"
#include "ColumnTerminal.h"
#include "FilterEvalData.h"

#include "LeftTerminal.h"
#include "RightTerminal.h"

#include <QString>
#include <QVariant>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Get value of QVariant type
   */
  QVariant getVariantValue(const FilterColumnData & col, const FilterEvalData & data);

  /*! \brief Get value of QVariant type
   */
  QVariant getVariantValue(const ParentModelColumnData & col, const FilterEvalData & data);

  /*! \brief Get value of QString type
   */
  inline
  QString getStringValue(const FilterColumnData & col, const FilterEvalData & data)
  {
    return getVariantValue(col, data).toString();
  }

  /*! \brief Like comparison callable
   */
  struct CompareLikeTo : boost::proto::callable
  {
    typedef bool result_type;

    /*
     * When using boost::proto::deep_copy(),
     * FilterColumn can not be deduced.
     * This is why we use templates.
     */
    template<typename FC, typename LikeExpr>
    bool operator()(const FC & col, const LikeExpr & expr, const FilterEvalData & data) const
    {
      return isLike( boost::proto::value(col), boost::proto::value(expr), data );
    }

   private:

    bool isLike(const FilterColumnData & col, const QString & like, const FilterEvalData & data) const;
  };

  /*! \brief == comparison callable
   */
  struct CompareEqualTo : boost::proto::callable
  {
    typedef bool result_type;

    /*
     * When using boost::proto::deep_copy(),
     * FilterColumn can not be deduced.
     * This is because we use a template.
     */
    template<typename FC, typename V>
    bool operator()(const FC & col, const V & value, const FilterEvalData & data) const
    {
      return isEqual( boost::proto::value(col), boost::proto::value(value), data );
    }

    static bool isEqual(const FilterColumnData & col, const QString & value, const FilterEvalData & data);
    static bool isEqual(const FilterColumnData & col, int value, const FilterEvalData & data);
    static bool isEqual(const FilterColumnData & col, const ParentModelColumnData & parentCol, const FilterEvalData & data);
  };

  /*! \brief != comparison callable
   */
  struct CompareNotEqualTo : boost::proto::callable
  {
    typedef bool result_type;

    template<typename FC, typename V>
    bool operator()(const FC & col, const V & value, const FilterEvalData & data) const
    {
      return !CompareEqualTo::isEqual( boost::proto::value(col), boost::proto::value(value), data );
    }
  };

  /*! \brief == and != comparison eval
   */
  struct CompareEqualityEval : boost::proto::or_<
                                boost::proto::when<
                                  boost::proto::equal_to< FilterColumn, LikeExpression > ,
                                  boost::proto::call< CompareLikeTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::equal_to< FilterColumn, ParentModelColumn > ,
                                  boost::proto::call< CompareEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::equal_to< FilterColumn, RightTerminal > ,
                                  boost::proto::call< CompareEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::not_equal_to< FilterColumn, ParentModelColumn > ,
                                  boost::proto::call< CompareNotEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::not_equal_to< FilterColumn, RightTerminal > ,
                                  boost::proto::call< CompareNotEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                >
                              >
  {
  };

  /*! \brief < comparison callable
   */
  struct CompareLessThan : boost::proto::callable
  {
    typedef bool result_type;

    template<typename FC, typename V>
    bool operator()(const FC & col, const V & value, const FilterEvalData & data) const
    {
      return isLessThan( boost::proto::value(col), boost::proto::value(value), data );
    }

   private:

    bool isLessThan(const FilterColumnData & col, const QString & value, const FilterEvalData & data) const;
    bool isLessThan(const FilterColumnData & col, int value, const FilterEvalData & data) const;
    bool isLessThan(const FilterColumnData & col, const ParentModelColumnData & parentCol, const FilterEvalData & data) const;
  };

  /*! \brief <= comparison callable
   */
  struct CompareLessEqualTo : boost::proto::callable
  {
    typedef bool result_type;

    template<typename FC, typename V>
    bool operator()(const FC & col, const V & value, const FilterEvalData & data) const
    {
      return isLessEqual( boost::proto::value(col), boost::proto::value(value), data );
    }

   private:

    bool isLessEqual(const FilterColumnData & col, const QString & value, const FilterEvalData & data) const;
    bool isLessEqual(const FilterColumnData & col, int value, const FilterEvalData & data) const;
    bool isLessEqual(const FilterColumnData & col, const ParentModelColumnData & parentCol, const FilterEvalData & data) const;
  };

  /*! \brief < and <= comparison eval
   */
  struct CompareLessEval : boost::proto::or_<
                              boost::proto::when<
                                boost::proto::less< FilterColumn, RightTerminal > ,
                                boost::proto::call< CompareLessThan(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                              > ,
                              boost::proto::when<
                                boost::proto::less< FilterColumn, ParentModelColumn > ,
                                boost::proto::call< CompareLessThan(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                              > ,
                              boost::proto::when<
                                boost::proto::less_equal< FilterColumn, RightTerminal > ,
                                boost::proto::call< CompareLessEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                              > ,
                              boost::proto::when<
                                boost::proto::less_equal< FilterColumn, ParentModelColumn > ,
                                boost::proto::call< CompareLessEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                              >
                            >
  {
  };

  /*! \brief > comparison callable
   */
  struct CompareGreaterThan : boost::proto::callable
  {
    typedef bool result_type;

    template<typename FC, typename V>
    bool operator()(const FC & col, const V & value, const FilterEvalData & data) const
    {
      return isGreaterThan( boost::proto::value(col), boost::proto::value(value), data );
    }

   private:

    bool isGreaterThan(const FilterColumnData & col, const QString & value, const FilterEvalData & data) const;
    bool isGreaterThan(const FilterColumnData & col, int value, const FilterEvalData & data) const;
    bool isGreaterThan(const FilterColumnData & col, const ParentModelColumnData & parentCol, const FilterEvalData & data) const;
  };

  /*! \brief >= comparison callable
   */
  struct CompareGreaterEqualTo : boost::proto::callable
  {
    typedef bool result_type;

    template<typename FC, typename V>
    bool operator()(const FC & col, const V & value, const FilterEvalData & data) const
    {
      return isGreaterEqual( boost::proto::value(col), boost::proto::value(value), data );
    }

   private:

    bool isGreaterEqual(const FilterColumnData & col, const QString & value, const FilterEvalData & data) const;
    bool isGreaterEqual(const FilterColumnData & col, int value, const FilterEvalData & data) const;
    bool isGreaterEqual(const FilterColumnData & col, const ParentModelColumnData & parentCol, const FilterEvalData & data) const;
  };

  /*! \brief > and >= comparison eval
   */
  struct CompareGreaterEval : boost::proto::or_<
                                boost::proto::when<
                                  boost::proto::greater< FilterColumn, RightTerminal > ,
                                  boost::proto::call< CompareGreaterThan(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::greater< FilterColumn, ParentModelColumn > ,
                                  boost::proto::call< CompareGreaterThan(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::greater_equal< FilterColumn, RightTerminal > ,
                                  boost::proto::call< CompareGreaterEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                > ,
                                boost::proto::when<
                                  boost::proto::greater_equal< FilterColumn, ParentModelColumn > ,
                                  boost::proto::call< CompareGreaterEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                >
                              >
  {
  };

  /*! \brief Comparison part of a FilterEval
   */
  struct ComparisonEval : boost::proto::or_<
                            CompareEqualityEval,
                            CompareGreaterEval,
                            CompareLessEval
                          >
  {
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_COMPARISON_EVAL_H
