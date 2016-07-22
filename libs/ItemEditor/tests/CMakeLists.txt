# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Item/View test library ====================

set(SOURCES_FILES
  src/TestTableModel.cpp
  src/ItemViewTestEdit.cpp
  src/TestTableEditionWidget.cpp
  src/TestFormEditionWidget.cpp
  src/TestWindowEditor.cpp
)

add_library(mdtitemeditortest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemeditortest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditortest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditortest Qt5::Test)

#================= Tests ====================

# Target MdtItemEditorItemModelTest
add_executable(mdtitemeditoritemmodeltest src/ItemModelTest.cpp)
target_link_libraries(mdtitemeditoritemmodeltest mdtitemeditortest)
add_test(MdtItemEditorItemModelTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemmodeltest)

# Target MdtItemEditorItemSelectionModelTest
add_executable(mdtitemeditoritemselectionmodeltest src/ItemSelectionModelTest.cpp)
target_link_libraries(mdtitemeditoritemselectionmodeltest mdtitemeditortest)
add_test(MdtItemEditorItemSelectionModelTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemselectionmodeltest)

# Target MdtItemEditorRowChangeTest
add_executable(mdtitemeditorrowchangetest src/RowChangeTest.cpp)
target_link_libraries(mdtitemeditorrowchangetest mdtitemeditortest)
add_test(MdtItemEditorRowChangeTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditorrowchangetest)

# Target MdtItemEditorActionsTest
add_executable(mdtitemeditoractionstest src/ActionsTest.cpp)
target_link_libraries(mdtitemeditoractionstest mdtitemeditortest)
add_test(MdtItemEditorActionsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoractionstest)

# Target MdtItemEditorStandardWidgetsTest
add_executable(mdtitemeditorstandardwidgetstest src/StandardWidgetsTest.cpp)
target_link_libraries(mdtitemeditorstandardwidgetstest mdtitemeditortest)
add_test(MdtItemEditorStandardWidgetsTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditorstandardwidgetstest)

# Target MdtItemEditorItemDelegateTest
add_executable(mdtitemeditoritemdelegatetest src/ItemDelegateTest.cpp)
target_link_libraries(mdtitemeditoritemdelegatetest mdtitemeditortest)
add_test(MdtItemEditorItemDelegateTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemdelegatetest)

# Target MdtItemEditorItemViewTest
add_executable(mdtitemeditoritemviewtest src/ItemViewTest.cpp)
target_link_libraries(mdtitemeditoritemviewtest mdtitemeditortest)
add_test(MdtItemEditorItemViewTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditoritemviewtest)

# Target MdtItemEditorDataWidgetMapperTest
add_executable(mdtitemeditordatawidgetmappertest src/DataWidgetMapperTest.cpp)
target_link_libraries(mdtitemeditordatawidgetmappertest mdtitemeditortest)
add_test(MdtItemEditorDataWidgetMapperTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditordatawidgetmappertest)

# Target MdtItemEditorControllerTest
add_executable(mdtitemeditorcontrollertest src/ControllerTest.cpp)
target_link_libraries(mdtitemeditorcontrollertest mdtitemeditortest)
add_test(MdtItemEditorControllerTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditorcontrollertest)

# Target MdtItemEditorTestEditorTest
add_executable(mdtitemeditortesteditortest src/TestEditorTest.cpp)
target_link_libraries(mdtitemeditortesteditortest mdtitemeditortest)
add_test(MdtItemEditorTestEditorTest ${CMAKE_CURRENT_BINARY_DIR}/mdtitemeditortesteditortest)
