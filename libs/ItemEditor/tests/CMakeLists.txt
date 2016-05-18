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
target_link_libraries(mdtitemeditoractionstest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditoractionstest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditoractionstest Qt5::Test)
add_test(MdtItemEditorActionsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoractionstest)

# Target MdtItemEditorStandardWidgetsTest
add_executable(mdtitemeditorstandardwidgetstest src/StandardWidgetsTest.cpp)
target_link_libraries(mdtitemeditorstandardwidgetstest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditorstandardwidgetstest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditorstandardwidgetstest Qt5::Test)
add_test(MdtItemEditorStandardWidgetsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditorstandardwidgetstest)
