# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

# Target MdtFilterExpressionSandbox
add_executable(mdtfilterexpressionsandbox src/Sandbox.cpp)
target_link_libraries(mdtfilterexpressionsandbox "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtfilterexpressionsandbox "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtfilterexpressionsandbox Qt5::Test)
add_test(MdtFilterExpressionSandbox ${CMAKE_CURRENT_BINARY_DIR}/mdtfilterexpressionsandbox)

# Target MdtFilterExpressionOtherSandbox
add_executable(mdtfilterexpressionothersandbox src/OtherSandbox.cpp)
target_link_libraries(mdtfilterexpressionothersandbox "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtfilterexpressionothersandbox "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtfilterexpressionothersandbox Qt5::Test)
add_test(MdtFilterExpressionOtherSandbox ${CMAKE_CURRENT_BINARY_DIR}/mdtfilterexpressionothersandbox)
