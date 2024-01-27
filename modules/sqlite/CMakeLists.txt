add_library(SqliteConnection STATIC SqliteConnection.cpp)
target_link_libraries(SqliteConnection sqlite3 StiltFox::Scribe::File)
set_target_properties(SqliteConnection PROPERTIES PUBLIC_HEADER "modules/sqlite/SqliteConnection.hpp;modules/DatabaseConnection.hpp")
target_include_directories(SqliteConnection PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")