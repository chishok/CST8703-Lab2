set(TARGET_NAME ${PROJECT_NAME}-ExampleMutex)

# add the executable
add_executable(${TARGET_NAME}
    "main.c")

# C/C++ standard
target_compile_features(${TARGET_NAME} PUBLIC
    c_std_99 cxx_std_14)

# compiler options
target_compile_options(${TARGET_NAME} PUBLIC
    $<$<COMPILE_LANGUAGE:C>:${PROJECT_C_COMPILER_OPTIONS}>
    $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_CXX_COMPILER_OPTIONS}>)

# link libraries
target_link_libraries(${TARGET_NAME} PUBLIC
    ${PROJECT_REQUIRED_LIBRARIES})

install(TARGETS ${TARGET_NAME})
