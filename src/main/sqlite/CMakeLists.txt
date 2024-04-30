add_library(SqliteConnection STATIC SqliteConnection.c++)
target_link_libraries(SqliteConnection sqlite3 StiltFox::Scribe::File)
set_target_properties(SqliteConnection PROPERTIES PUBLIC_HEADER "src/main/sqlite/SqliteConnection.h++;src/main/DatabaseConnection.h++")
target_include_directories(SqliteConnection PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")