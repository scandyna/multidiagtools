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
#include "JoinClauseTest.h"
#include "Mdt/Application.h"
#include <QDebug>

#include <iostream>
#include <string>

#include <QString>

#include <boost/proto/extends.hpp>

#include <boost/core/demangle.hpp>

#include <boost/proto/proto.hpp>
namespace proto = boost::proto;

void JoinClauseTest::initTestCase()
{
}

void JoinClauseTest::cleanupTestCase()
{
}

struct JoinConstraintFieldTag
{
};

/*
 * Terminals
 */

/*! \brief Wrapper for a terminal using table name and field name
 *
 * \tparam Tag Type to make target terminal a unique type
 */
template<typename Tag, typename Domain = boost::proto::default_domain>
struct TableField : boost::proto::extends<
                                    boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<Tag> >,
                                    TableField< Tag, Domain >,
                                    Domain
                                  >
{
 private:

  typedef boost::proto::basic_expr< boost::proto::tag::terminal, boost::proto::term<Tag> > terminal_type;
  typedef TableField< Tag, Domain >                                                        TableField_t;
  typedef boost::proto::extends< terminal_type, TableField_t, Domain >                     base_type;

 public:

  typedef typename boost::proto::result_of::value<terminal_type>::type          value_type;
  typedef typename boost::proto::result_of::value<terminal_type &>::type        reference;
  typedef typename boost::proto::result_of::value<const terminal_type &>::type  const_reference;

  /*! \brief Constructor
   */
  TableField(const QString & tn)
   : base_type(terminal_type::make(Tag())),
     tableName(tn)
  {
    qDebug() << "TableField::TableField(" << tn << ")";
  }

//   TableField(const TableField &) = delete;
//   TableField(TableField &&) = delete;

  QString tableName;
};

/*! \brief Field terminal used in a JoinConstraintExpression
 *
 * Typical usage:
 * \code
 * #include <xy....>
 *
 * using xxyy;
 *
 * JoinConstraintField cliendId(...., ....);
 * // cliendId is a terminal that can be used in a JoinConstraintExpression
 *
 * \endcode
 */
using JoinConstraintField = TableField<JoinConstraintFieldTag>;

/*
 * Grammar
 */

struct Grammar;

struct MatchValue : boost::proto::or_<
                        boost::proto::terminal< boost::proto::convertible_to<int> > ,
                        boost::proto::terminal< QString >
                      >
{
};

struct MatchEqual : boost::proto::or_<
                        boost::proto::equal_to< JoinConstraintField, MatchValue > ,
                        boost::proto::equal_to< Grammar, Grammar >
                      >
{
};

struct Compare : proto::or_<
                      MatchEqual
                    >
{
};

struct And : proto::logical_and< Grammar, Grammar >
{
};

struct Grammar : boost::proto::or_<
                            Compare,
                            And
                          >
{
};

/*
 * Transforms
 */

struct MyEscape
{
  MyEscape(int v)
   : value(v)
  {
  }

  int value;
};

struct MatchEqualAction : boost::proto::callable
{
  typedef QString result_type;

//   template<typename Ci, typename V>
//   void operator()(const Ci & ci, const V & v) const
//   {
//     //std::cout << "MatchEqualAction: " << boost::proto::value(ci).tableName.toStdString() << ".column " << boost::proto::value(ci).data.index << " == " << boost::proto::value(v).toStdString() << std::endl;
//     std::cout << "MatchEqualAction: " << boost::proto::value(ci).tableName.toStdString() << " == " << boost::proto::value(v).toStdString() << std::endl;
//   }

  template<typename V>
  QString operator()(const JoinConstraintField & f, const V & v, const MyEscape & esc) const
  {
//     std::cout << "MatchEqualAction(" << this << "): " << f.tableName.toStdString() << " == " << value(boost::proto::value(v)) << std::endl;
//     std::cout << "_data: " << boost::core::demangle( typeid(boost::proto::_data).name() ) << std::endl;
    qDebug() << "esc: " << esc.value;
    return f.tableName + " = " + value(boost::proto::value(v));
    //return "f == v";
  }

  QString operator()(const JoinConstraintField & lf, const JoinConstraintField & rf) const
  {
    std::cout << "MatchEqualAction(" << this << "): " << lf.tableName.toStdString() << " == " << rf.tableName.toStdString() << std::endl;
    //return "lf == rf";
    return lf.tableName + " = " + rf.tableName;
  }

//   QString operator()(const JoinConstraintField & lf, const QString & s) const
//   {
//     return lf.tableName + " == " + s;
//   }
// 
//   QString operator()(const JoinConstraintField & lf, int v) const
//   {
//     return lf.tableName + " == " + QString::number(v);
//   }

 private:

  QString value(int v) const
  {
    return QString::number(v);
  }

  QString value(const QString & v) const
  {
    return v;
  }
};

struct AndTransform : boost::proto::callable
{
  typedef QString result_type;

//   template<typename L, typename R>
//   QString operator()(const L & l, const R & r) const
//   {
//     //std::cout << "AndTransform: " << boost::proto::value(l).toStdString() << std::endl;
//     std::cout << boost::core::demangle( typeid(l).name() ) << std::endl;
//     return "l && r";
//     //return getResult(boost::proto::value(l), boost::proto::value(r));
//   }

  QString operator()(const QString & l, const QString & r) const
  {
    return l + " AND " + r;
  }

//   QString getResult(const JoinConstraintField & l, const JoinConstraintField & r) const
//   {
//     return l.tableName + " AND " + r.tableName;
//   }
};

struct CompareAction : boost::proto::or_<
                            boost::proto::when<
                              boost::proto::equal_to< JoinConstraintField, MatchValue > ,
                              boost::proto::call<MatchEqualAction(boost::proto::_left, boost::proto::_right, boost::proto::_data)>
                            > ,
                            boost::proto::when<
                              boost::proto::equal_to< JoinConstraintField, JoinConstraintField > ,
                              boost::proto::call<MatchEqualAction(boost::proto::_left, boost::proto::_right)>
                            >
                          >
{
};

struct AndAction : boost::proto::or_<
                            boost::proto::when<
                              boost::proto::logical_and<CompareAction, CompareAction> ,
                              //boost::proto::call<AndTransform(boost::proto::_left, boost::proto::_right)>
                              boost::proto::call<AndTransform( CompareAction(boost::proto::_left) , CompareAction(boost::proto::_right) )>
                            >
                          >
{
};

struct FilterExpressionEval : boost::proto::or_<
                                    CompareAction ,
                                    AndAction
                                  >
{
};

// struct FilterExpressionEval : boost::proto::or_<
//                                     boost::proto::when<
//                                       Compare ,
//                                       CompareAction
//                                     > ,
//                                     boost::proto::when<
//                                       And ,
//                                       AndAction
//                                     >
//                                   >
// {
// };


/*
 * Compile time tests
 */

/*
 * Tests
 */

template<typename Expr>
void evaluateExpression(const Expr & expr)
{
  std::cout << "***** Evaluate ***********" << std::endl;
  proto::display_expr(expr);
  FilterExpressionEval eval;
  std::cout << "eval: "  << eval(expr, 0, MyEscape(35)).toStdString() << std::endl;
  std::cout << "**************************" << std::endl;
}

void JoinClauseTest::sandbox()
{
  JoinConstraintField clientId("Client_tbl");
  JoinConstraintField adrClientId("Address_tbl");

  //proto::display_expr( clientId == 5 || clientId == adrClientId || adrClientId < 8 );
  evaluateExpression( clientId == 75 );
  evaluateExpression( clientId == adrClientId && clientId == 25 );

  qDebug() << "clientId table: " << clientId.tableName;
  qDebug() << "adrClientId table: " << adrClientId.tableName;
}

/*
 * Main
 */

int main(int argc, char **argv)
{
  Mdt::Application app(argc, argv);
  JoinClauseTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
