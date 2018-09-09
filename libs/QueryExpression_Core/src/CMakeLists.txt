
# List of source files
set(SOURCE_FILES
  Mdt/QueryExpression/LiteralValue.cpp
  Mdt/QueryExpression/FieldName.cpp
  Mdt/QueryExpression/EntityName.cpp
  Mdt/QueryExpression/SelectEntity.cpp
  Mdt/QueryExpression/EntityAndField.cpp
  Mdt/QueryExpression/SelectAllField.cpp
  Mdt/QueryExpression/SelectFieldVariant.cpp
  Mdt/QueryExpression/SelectField.cpp
  Mdt/QueryExpression/SelectFieldList.cpp
  Mdt/QueryExpression/Terminal.cpp
  Mdt/QueryExpression/LikeExpression.cpp
  Mdt/QueryExpression/LikeExpressionRegexTransform.cpp
  Mdt/QueryExpression/Comparison.cpp
  Mdt/QueryExpression/ExpressionGrammar.cpp
  Mdt/QueryExpression/ComparisonOperator.cpp
  Mdt/QueryExpression/LogicalOperator.cpp
  Mdt/QueryExpression/ExpressionTreeGraph.cpp
  Mdt/QueryExpression/ExpressionTree.cpp
  Mdt/QueryExpression/AbstractExpressionTreeVisitor.cpp
  Mdt/QueryExpression/TravserseTreeGraph.cpp
  Mdt/QueryExpression/TypeTraits/Graph.cpp
  Mdt/QueryExpression/TypeTraits/FilterExpression.cpp
  Mdt/QueryExpression/TypeTraits/JoinConstraintExpression.cpp
  Mdt/QueryExpression/BuildExpressionTree/AddExpressionToTree.cpp
  Mdt/QueryExpression/BuildExpressionTree/Transform.cpp
  Mdt/QueryExpression/JoinOperator.cpp
  Mdt/QueryExpression/JoinConstraintExpression.cpp
  Mdt/QueryExpression/JoinClause.cpp
  Mdt/QueryExpression/JoinClauseList.cpp
  Mdt/QueryExpression/FilterExpression.cpp
  Mdt/QueryExpression/SelectStatement.cpp
  Mdt/QueryExpression/QuerySelectStatement.cpp
  Mdt/QueryExpression/AbstractSelectQuery.cpp
  Mdt/QueryExpression/AbstractSelectQueryFactory.cpp
  Mdt/QueryExpression/CachedSelectQuery.cpp
  Mdt/QueryExpression/AbstractAsyncSelectQuery.cpp
  Mdt/QueryExpression/AbstractAsyncSelectQueryFactory.cpp
)

find_package(Qt5 COMPONENTS Core)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
  endif(Boost_FOUND)
endif()

mdt_add_library(
  NAME QueryExpression_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Algorithm Container Error_Core Qt5::Core;
)
target_compile_definitions(QueryExpression_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME QueryExpression_Core
  DESCRIPTION "Provide some expression language for queries."
)
