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
#ifndef MDT_ITEM_MODEL_EXPRESSION_COMPARISON_EVAL_H
#define MDT_ITEM_MODEL_EXPRESSION_COMPARISON_EVAL_H

#include "../FilterColumn.h"
#include "FilterEvalData.h"
#include "LeftTerminal.h"
#include "RightTerminal.h"
#include <QString>
#include <QVariant>
#include <boost/proto/traits.hpp>
#include <boost/proto/matches.hpp>
#include <boost/proto/transform/when.hpp>
#include <boost/proto/transform.hpp>

// #include <boost/proto/proto.hpp>

// #include <QDebug>


namespace Mdt{ namespace ItemModel{ namespace Expression{

  /*! \brief Get value of QVariant type
   */
  QVariant getVariantValue(const FilterColumnData & col, const FilterEvalData & data);

  /*! \brief Get value of QString type
   */
  inline
  QString getStringValue(const FilterColumnData & col, const FilterEvalData & data)
  {
    return getVariantValue(col, data).toString();
  }

  /*! \brief == comparison callable
   */
  struct CompareEqualTo : boost::proto::callable
  {
    typedef bool result_type;

    bool operator()(const FilterColumn & col, const LikeExpression & expr, const FilterEvalData & data) const
    {
      return isLike( boost::proto::value(col), expr, data );
    }

    template<typename V>
    bool operator()(const FilterColumn & col, const V & value, const FilterEvalData & data) const
    {
      return isEqual( boost::proto::value(col), boost::proto::value(value), data );
    }

   private:

    bool isLike(const FilterColumnData & col, const LikeExpression & like, const FilterEvalData & data) const;
    bool isEqual(const FilterColumnData & col, const QString & value, const FilterEvalData & data) const;
    bool isEqual(const FilterColumnData & col, int value, const FilterEvalData & data) const;

//     QVariant getValue(const FilterColumnData & col, const FilterEvalData & data) const;
  };

  /*! \brief == comparison eval
   */
  struct CompareEqualToEval : boost::proto::or_<
                                boost::proto::when<
                                  boost::proto::equal_to< LeftTerminal, RightTerminal > ,
                                  boost::proto::call< CompareEqualTo(boost::proto::_left, boost::proto::_right, boost::proto::_data) >
                                >
                              >
  {
  };

  /*! \brief Comparison part of a FilterEval
   */
  struct ComparisonEval : boost::proto::or_<
                            CompareEqualToEval
                          >
  {
  };

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{

#endif // #ifndef MDT_ITEM_MODEL_EXPRESSION_COMPARISON_EVAL_H
