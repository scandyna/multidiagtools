# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Item/View test library ====================

# set(SOURCES_FILES 
#     src/TestDatabaseSchema.cpp
# )
# 
# add_library(mdtsqltest SHARED ${SOURCES_FILES})
# target_link_libraries(mdtsqltest "mdtsql${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtsqltest Qt5::Test)

#================= Tests ====================

# Target MdtItemEditorActionsTest
add_executable(mdtitemeditoractionstest src/ActionsTest.cpp)
# target_link_libraries(mdtitemeditoractionstest "mdtsqltest")
add_test(MdtItemEditorActionsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoractionstest)
target_link_libraries(mdtitemeditoractionstest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditoractionstest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditoractionstest Qt5::Test)
