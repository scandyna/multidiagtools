
find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    CommandLineParser.cpp
    MdtCpBinDepsMain.cpp
    main.cpp
)

add_executable(mdtcpbindeps ${SOURCE_FILES})
target_link_libraries(mdtcpbindeps Application DeployUtils_Core Qt5::Core)
