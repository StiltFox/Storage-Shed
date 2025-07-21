cmake_minimum_required(VERSION 3.5.0)
project(Storage-Shed VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Scribe REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories("/usr/include/mariadb") #path to include folder
add_library(mariadbcpp STATIC IMPORTED)
set_property(TARGET mariadbcpp PROPERTY IMPORTED_LOCATION "/usr/lib/libmariadbcpp.so")

add_subdirectory(src)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/StorageShedConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StorageShed
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/StorageShedConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/StorageShedConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StorageShed
)

install(
    TARGETS SqliteConnection MariaDBConnection
    EXPORT StorageShed
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/Stilt_Fox/StorageShed
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/Stilt_Fox/StorageShed
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/Stilt_Fox/StorageShed
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Stilt_Fox/StorageShed
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Stilt_Fox/StorageShed
)

install(EXPORT StorageShed
        FILE StorageShedTargets.cmake
        NAMESPACE StiltFox::StorageShed::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/StorageShed
)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/StorageShedConfigVersion.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion
)