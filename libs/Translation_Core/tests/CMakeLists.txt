
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Translation_Core test library ====================

set(SOURCE_FILES
  src/TestBase.cpp
)

add_library(mdtTranslation_CoreTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtTranslation_CoreTest Translation_Core Application_Core Qt5::Test)

#=============== Helper functions ===========

function(addTranslation_CoreTest testName)
  set(complteTestName "mdtTranslation_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtTranslation_CoreTest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

# addTranslation_CoreTest("")
addTranslation_CoreTest("LanguageCodeTest")
addTranslation_CoreTest("LanguageCodeBenchmark")
addTranslation_CoreTest("QmFileNameTest")
addTranslation_CoreTest("QmFileNameBenchmark")
addTranslation_CoreTest("TranslationInfoTest")
addTranslation_CoreTest("FindTranslationTest")
