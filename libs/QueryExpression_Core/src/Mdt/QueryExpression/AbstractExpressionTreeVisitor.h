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
#ifndef MDT_QUERY_EXPRESSION_ABSTRACT_EXPRESSION_TREE_VISITOR_H
#define MDT_QUERY_EXPRESSION_ABSTRACT_EXPRESSION_TREE_VISITOR_H

#include "ExpressionTreeGraph.h"
#include "ComparisonOperator.h"
#include "LogicalOperator.h"
#include "SelectField.h"
#include "EntityAndField.h"
#include "MdtQueryExpression_CoreExport.h"
#include <QVariant>

namespace Mdt{ namespace QueryExpression{

  /*! \brief Base class to create a visitor to traverse a expression tree
   *
   * Example for a visitor that generates a infix string from a expression tree:
   * \code
   * class ExpressionToInfixStringVisitor : public Mdt::QueryExpression::AbstractExpressionTreeVisitor
   * {
   *  public:
   *
   *   void processInorder(Mdt::QueryExpression::ComparisonOperator op) override
   *   {
   *     mExpressionString += comparisonOperatorToString(op);
   *   }
   *
   *   void processInorder(Mdt::QueryExpression::LogicalOperator op) override
   *   {
   *     mExpressionString += ")" + logicalOperatorToString(op) + "(";
   *   }
   *
   *   void processInorder(const Mdt::QueryExpression::EntityAndField & field) override
   *   {
   *     mExpressionString += field.fieldAliasOrName();
   *   }
   *
   *   void processInorder(const QVariant & value) override
   *   {
   *     if(isTypeString(value)){
   *       mExpressionString += "\"" + value.toString() + "\"";
   *     }else{
   *       mExpressionString += value.toString();
   *     }
   *   }
   *
   *  private:
   *
   *   static QString comparisonOperatorToString(Mdt::QueryExpression::ComparisonOperator op)
   *   {
   *     using Mdt::QueryExpression::ComparisonOperator;
   *
   *     switch(op){
   *       case ComparisonOperator::Equal:
   *         return "==";
   *       case ComparisonOperator::Less:
   *         return "<";
   *     }
   *   }
   *
   *   static QString logicalOperatorToString(LogicalOperator op)
   *   {
   *     using Mdt::QueryExpression::ComparisonOperator;
   *
   *     switch(op){
   *       case LogicalOperator::And:
   *         return "&&";
   *       case LogicalOperator::Or:
   *         return "||";
   *     }
   *   }
   *
   *   static bool isTypeString(const QVariant & v)
   *   {
   *     const auto type = static_cast<QMetaType::Type>(v.type());
   *     return type == QMetaType::QString;
   *   }
   *
   *   QString mExpressionString;
   * };
   * \endcode
   */
  class MDT_QUERYEXPRESSION_CORE_EXPORT AbstractExpressionTreeVisitor
  {
   public:

    /*! \brief Constructor
     */
    AbstractExpressionTreeVisitor() = default;

    AbstractExpressionTreeVisitor(const AbstractExpressionTreeVisitor &) = delete;
    AbstractExpressionTreeVisitor & operator=(const AbstractExpressionTreeVisitor &) = delete;
    AbstractExpressionTreeVisitor(AbstractExpressionTreeVisitor &&) = delete;
    AbstractExpressionTreeVisitor & operator=(AbstractExpressionTreeVisitor &&) = delete;

    /*! \brief Destructor
     */
    virtual ~AbstractExpressionTreeVisitor() = default;

    /*! \brief Will call processPreorder
     */
    void preorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g);

    /*! \brief Will call processinrder
     */
    void inorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g);

    /*! \brief Will call processPostorder
     */
    void postorder(ExpressionTreeVertex v, const ExpressionTreeGraph & g);

    /*! \brief
     */
    void onEdge(ExpressionTreeEdge e, const ExpressionTreeGraph & g);

    /*! \brief
     */
    void postEdge(ExpressionTreeEdge e, const ExpressionTreeGraph & g);

    /*! \brief Called by preorder()
     *
     * This default implementation does nothing
     */
    virtual void processPreorder(ComparisonOperator op)
    {
      Q_UNUSED(op);
    }

    /*! \brief Called by preorder()
     *
     * This default implementation does nothing
     */
    virtual void processPreorder(LogicalOperator op)
    {
      Q_UNUSED(op);
    }

    /*! \brief Called by preorder()
     *
     * This default implementation does nothing
     */
    virtual void processPreorder(const EntityAndField & field)
    {
      Q_UNUSED(field);
    }

    /*! \brief Called by preorder()
     *
     * This default implementation does nothing
     */
    virtual void processPreorder(const QVariant & value)
    {
      Q_UNUSED(value);
    }

    /*! \brief Called by inorder()
     *
     * This default implementation does nothing
     */
    virtual void processInorder(ComparisonOperator op)
    {
      Q_UNUSED(op);
    }

    /*! \brief Called by inorder()
     *
     * This default implementation does nothing
     */
    virtual void processInorder(LogicalOperator op)
    {
      Q_UNUSED(op);
    }

    /*! \brief Called by inorder()
     *
     * This default implementation does nothing
     */
    virtual void processInorder(const EntityAndField & field)
    {
      Q_UNUSED(field);
    }

    /*! \brief Called by inorder()
     *
     * This default implementation does nothing
     */
    virtual void processInorder(const QVariant & value)
    {
      Q_UNUSED(value);
    }

    /*! \brief Called by postorder()
     *
     * This default implementation does nothing
     */
    virtual void processPostorder(ComparisonOperator op)
    {
      Q_UNUSED(op);
    }

    /*! \brief Called by postorder()
     *
     * This default implementation does nothing
     */
    virtual void processPostorder(LogicalOperator op)
    {
      Q_UNUSED(op);
    }

    /*! \brief Called by postorder()
     *
     * This default implementation does nothing
     */
    virtual void processPostorder(const EntityAndField & field)
    {
      Q_UNUSED(field);
    }

    /*! \brief Called by postorder()
     *
     * This default implementation does nothing
     */
    virtual void processPostorder(const QVariant & value)
    {
      Q_UNUSED(value);
    }

  };

}} // namespace Mdt{ namespace QueryExpression{

#endif // #ifndef MDT_QUERY_EXPRESSION_ABSTRACT_EXPRESSION_TREE_VISITOR_H
