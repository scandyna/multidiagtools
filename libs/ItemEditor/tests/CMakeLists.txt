# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Item/View test library ====================

set(SOURCES_FILES
  src/TestTableModel.cpp
  src/ItemViewTestEdit.cpp
)

add_library(mdtitemeditortest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemeditortest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditortest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditortest Qt5::Test)

#================= Tests ====================

# Target MdtItemEditorActionsTest
add_executable(mdtitemeditoractionstest src/ActionsTest.cpp)
target_link_libraries(mdtitemeditoractionstest mdtitemeditortest)
add_test(MdtItemEditorActionsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoractionstest)

# Target MdtItemEditorStandardWidgetsTest
add_executable(mdtitemeditorstandardwidgetstest src/StandardWidgetsTest.cpp)
target_link_libraries(mdtitemeditorstandardwidgetstest mdtitemeditortest)
add_test(MdtItemEditorStandardWidgetsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditorstandardwidgetstest)

# Target MdtItemEditorItemModelTest
add_executable(mdtitemeditoritemmodeltest src/ItemModelTest.cpp)
target_link_libraries(mdtitemeditoritemmodeltest mdtitemeditortest)
add_test(MdtItemEditorItemModelTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemmodeltest)

# Target MdtItemEditorItemDelegateTest
add_executable(mdtitemeditoritemdelegatetest src/ItemDelegateTest.cpp)
target_link_libraries(mdtitemeditoritemdelegatetest mdtitemeditortest)
add_test(MdtItemEditorItemDelegateTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemdelegatetest)

# Target MdtItemEditorItemViewTest
add_executable(mdtitemeditoritemviewtest src/ItemViewTest.cpp)
target_link_libraries(mdtitemeditoritemviewtest mdtitemeditortest)
add_test(MdtItemEditorItemViewTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemviewtest)
