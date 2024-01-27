add_executable(Tests SqliteConnectionTest.cpp)
target_link_libraries(Tests GTest::gtest_main SqliteConnection sqlite3 StiltFox::Scribe::File)
gtest_discover_tests(Tests)