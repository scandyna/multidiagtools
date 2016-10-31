# Project file for Multi Diag Tools

# Project
project(multidiagtools)

# List of source files
set(SOURCES_FILES 
    src/Mdt/Sql/Error.cpp
    src/Mdt/Sql/Query.cpp
    src/Mdt/Sql/InsertQuery.cpp
    src/Mdt/Sql/SelectTable.cpp
    src/Mdt/Sql/Expression/TerminalSqlTransform.cpp
    src/Mdt/Sql/Expression/ComparisonSqlTransform.cpp
    src/Mdt/Sql/Expression/LogicalSqlTransform.cpp
    src/Mdt/Sql/Impl/JoinConstraintFieldPairList.cpp
    src/Mdt/Sql/Impl/JoinConstraintFieldPairListSqlTransform.cpp
    src/Mdt/Sql/JoinConstraintExpression.cpp
    src/Mdt/Sql/JoinClause.cpp
    src/Mdt/Sql/FromClause.cpp
    src/Mdt/Sql/FromClauseSqlTransform.cpp
    src/Mdt/Sql/WhereExpression.cpp
    src/Mdt/Sql/SelectStatement.cpp
    src/Mdt/Sql/SelectQuery.cpp
    src/Mdt/Sql/Schema/Driver.cpp
    src/Mdt/Sql/Schema/DriverImplementationInterface.cpp
    src/Mdt/Sql/Schema/DriverSQLite.cpp
    src/Mdt/Sql/Schema/DriverMySql.cpp
    src/Mdt/Sql/Schema/DriverPostgreSQL.cpp
    src/Mdt/Sql/Schema/FieldTypeName.cpp
    src/Mdt/Sql/Schema/FieldTypeListModel.cpp
    src/Mdt/Sql/Schema/PrimaryKeyContainer.cpp
    src/Mdt/Sql/Schema/ForeignKey.cpp
    src/Mdt/Sql/Schema/Index.cpp
    src/Mdt/Sql/Schema/IndexListModel.cpp
    src/Mdt/Sql/Schema/Table.cpp
    src/Mdt/Sql/Schema/TableModel.cpp

    src/Mdt/Sql/Schema/ViewTable.cpp

    src/Mdt/Sql/Schema/JoinClause.cpp
    src/Mdt/Sql/Schema/JoinHelper.cpp
    src/Mdt/Sql/Schema/View.cpp
    src/Mdt/Sql/Schema/Schema.cpp
)

# Headers directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Expected/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Algorithm/src")
# Because some headers are generated during building (config.h, Qt's UIC), we also build path
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Output library name
set(LIBRARY_NAME "Sql")

# Output library technical names
string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LC)
set(LIBRARY_BIN_NAME "mdt${LIBRARY_NAME_LC}")
# Add output library
add_library(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdterror${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdterrorgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdtalgorithm${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} Qt5::Sql)

# Translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_de.ts
)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES} OPTIONS -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target("Mdt${LIBRARY_NAME}Ts" DEPENDS ${QM_FILES} ${TS_FILES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/Mdt/${LIBRARY_NAME}" COMPONENT dev)

# Install targets
install(TARGETS "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  add_custom_command(TARGET "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}"
                    POST_BUILD
                    COMMAND objcopy --only-keep-debug "${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.so" "${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.dbg"
                    COMMAND strip --strip-debug --strip-unneeded "${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND objcopy --add-gnu-debuglink="${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.dbg" "${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.so"
                    COMMAND chmod -x "${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.dbg")
  install(FILES "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}.dbg" DESTINATION "${LIB_DEST_DIR}" COMPONENT dbg)
endif(UNIX)

# Unit tests
add_subdirectory("tests")
