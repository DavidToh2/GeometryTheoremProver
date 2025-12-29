
add_executable(tests ${entry_test} ${sources_test} ${sources}) 
target_link_libraries(tests PRIVATE highs)