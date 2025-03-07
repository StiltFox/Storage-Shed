add_library(PrintHelper STATIC PrintHelper.c++)
target_link_libraries(PrintHelper
        nlohmann_json
)
target_include_directories(PrintHelper PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")