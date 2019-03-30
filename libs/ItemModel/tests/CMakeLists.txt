
enable_testing()

find_package(Qt5 COMPONENTS Test Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#=============== ItemModel test library =====

set(SOURCES_FILES
    src/TestBase.cpp
    src/SqlTestBase.cpp
    src/PersonStorage.cpp
    src/PersonTableModel.cpp
)

add_library(mdtitemmodeltest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemmodeltest ItemModel QueryExpression_Sql Entity_Sql Sql_Core Application_Widgets TestLib_Core Qt5::Test)

#=============== Qt5 item model test library =====

set(QT5_SOURCES_FILES
    qt5/qtdynamictreemodel.cpp
    qt5/qtmodeltest.cpp
)

add_library(mdtitemmodelqt5test SHARED ${QT5_SOURCES_FILES})
target_link_libraries(mdtitemmodelqt5test Qt5::Widgets Qt5::Test)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/qt5")

#=============== Helper functions ===========

# Add a test for this ItemModel lib
function(addItemModelTest testName)
  set(complteTestName "MdtItemModel_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtitemmodeltest mdtitemmodelqt5test)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
endfunction()

#================= Tests ====================

addItemModelTest("qt5_tst_modeltest")
addItemModelTest("VariantTableModelTest")
addItemModelTest("RangeTest")
addItemModelTest("SelectionTest")
addItemModelTest("RemoveRowsTest")
addItemModelTest("AbstractTableModelTest")
addItemModelTest("AbstractCachedTableModelTest")
addItemModelTest("PersonStorageTest")
addItemModelTest("CachedTableModelTest")
addItemModelTest("CachedTableModelStressTest")
addItemModelTest("FormatMapTest")
addItemModelTest("SortFilterProxyModelTest")
addItemModelTest("FilterExpressionTest")
addItemModelTest("FilterProxyModelTest")
addItemModelTest("RelationFilterProxyModelTest")
addItemModelTest("SortProxyModelTest")
addItemModelTest("FormatProxyModelTest")
addItemModelTest("ProxyModelContainerTest")
addItemModelTest("KeyTest")
addItemModelTest("RelationKeyTest")
addItemModelTest("PrimaryKeyProxyModelTest")
addItemModelTest("ForeignKeyProxyModelTest")
addItemModelTest("HeaderProxyModelTest")
addItemModelTest("StlTableTest")
addItemModelTest("StlListModelTest")
addItemModelTest("StlListModelBenchmark")
addItemModelTest("StlTableModelTest")
addItemModelTest("StlTableModelBenchmark")
addItemModelTest("SelectQueryTableModelTest")
addItemModelTest("SelectionModelCurrentRowChangedTest")
