#!/bin/sh
make
cpack --config package_debian_lib.cmake
cpack --config package_debian_dev.cmake
cpack --config package_debian_tools.cmake
cpack --config package_debian_dbg.cmake
cpack --config package_debian_data.cmake
