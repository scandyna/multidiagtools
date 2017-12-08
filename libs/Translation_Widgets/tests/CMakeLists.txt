
enable_testing()

find_package(Qt5 COMPONENTS Test)

add_subdirectory(translations)

#================= Translation_Widgets test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtTranslation_WidgetsTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtTranslation_WidgetsTest Translation_Widgets Application_Widgets TestLib_Core Qt5::Test)

#=============== Helper functions ===========

function(addTranslation_WidgetsTest testName)
  set(complteTestName "mdtTranslation_Widgets_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtTranslation_WidgetsTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addTranslation_WidgetsTest("LanguageSelectionTest")
# addTranslation_WidgetsTest("")
