
add_executable(main GTPEngine.cpp GTPEngine.hh ${entry_main} ${sources}) 
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(main PRIVATE highs)