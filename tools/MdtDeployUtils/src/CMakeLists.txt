
find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    MdtDeployUtilsMain.cpp
    main.cpp
)

add_executable(mdtdeployutils ${SOURCE_FILES})
target_link_libraries(mdtdeployutils Application DeployUtils_Core Qt5::Core)
