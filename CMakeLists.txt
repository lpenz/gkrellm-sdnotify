# Copyright (C) 2009 Leandro Lisboa Penz <lpenz@lpenz.org>
# This file is subject to the terms and conditions defined in
# file 'LICENSE', which is part of this source code package.

cmake_minimum_required (VERSION 3.0)

project (gkrellm-sdnotify)

include(GNUInstallDirs)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -W")
endif(CMAKE_COMPILER_IS_GNUCXX)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB REQUIRED glib-2.0)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
pkg_check_modules (SYSTEMD REQUIRED libsystemd)

include_directories ("${GLIB_INCLUDE_DIRS}")
include_directories ("${GTK2_INCLUDE_DIRS}")

# Main targets

add_library (gkrellm-sdnotify
    SHARED
    gkrellm-sdnotify.cc)
target_link_libraries (gkrellm-sdnotify ${SYSTEMD_LIBRARIES})

# Install

install (TARGETS gkrellm-sdnotify
    DESTINATION lib/gkrellm2/plugins
    )
install (
    FILES README.md LICENSE AUTHORS
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/
    )

# cpack deb

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT "Leandro Lisboa Penz <lpenz@lpenz.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "gkrellm plugin that calls sd_notify when loaded")
set(CPACK_DEBIAN_PACKAGE_SECTION utils)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
execute_process(COMMAND git describe --tags
                OUTPUT_VARIABLE GIT_DESCRIBE)
string(REGEX REPLACE "-g[0-9a-f]+\n$" "" CPACK_PACKAGE_VERSION "${GIT_DESCRIBE}")
set(CPACK_PACKAGE_FILE_NAME "gkrellm-sdnotify-${CPACK_PACKAGE_VERSION}")
include(CPack)

