file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
# handle plugin system separately
list(REMOVE_ITEM EXAMPLE_SOURCES Plugin.cpp PluginUse.cpp)

# usual examples
foreach(EXAMPLE ${EXAMPLE_SOURCES})
    string(REGEX REPLACE "\\.cpp$" "" EXAMPLE_NAME ${EXAMPLE})
    add_executable(${EXAMPLE_NAME} ${EXAMPLE})
    target_link_libraries(${EXAMPLE_NAME} ${RSC_NAME})
endforeach(EXAMPLE)

add_library(myplugin SHARED Plugin.cpp)
target_link_libraries(myplugin ${RSC_NAME})
add_executable(PluginUse PluginUse.cpp)
target_link_libraries(PluginUse ${RSC_NAME})
