
enable_testing()

find_package(Qt5 COMPONENTS Test)

#=============== Qt5 plain text test library =====

set(QT5_SOURCES_FILES
    qt5/qtdynamictreemodel.cpp
    qt5/qtmodeltest.cpp
)

add_library(mdtplaintextqt5test SHARED ${QT5_SOURCES_FILES})
target_link_libraries(mdtplaintextqt5test Qt5::Core Qt5::Test)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/qt5")

#================= PlainText test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtplaintexttest SHARED ${SOURCE_FILES})
target_link_libraries(mdtplaintexttest mdtplaintextqt5test PlainText_Core Application Qt5::Test)

#=============== Helper functions ===========

function(addPlainTextTest testName)
  set(complteTestName "MdtPlainText_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtplaintexttest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#   add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addPlainTextTest("FileInputIteratorTest")
addPlainTextTest("FileMultiPassIteratorTest")
addPlainTextTest("StringIteratorTest")
addPlainTextTest("DataTest")
addPlainTextTest("RecordListTableModelTest")
addPlainTextTest("SettingsTest")
addPlainTextTest("CsvParserTest")
addPlainTextTest("FileReaderTest")
