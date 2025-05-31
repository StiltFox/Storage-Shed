if(NOT SFSkipMariaDB)
    add_library(MariaDBConnection STATIC MariaDBConnection.c++)
    target_link_libraries(MariaDBConnection
            mariadbcpp
    )
    set_target_properties(MariaDBConnection PROPERTIES PUBLIC_HEADER "src/main/mariadb/MariaDBConnection.h++;src/main/DatabaseConnection.h++")
    target_include_directories(MariaDBConnection PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
endif ()