set(TARGET_NAME ${PROJECT_NAME}-FreshThreads)

# add the executable
add_executable(${TARGET_NAME}
    "include/ac_args.h"
    "include/ac_file.h"
    "include/ac_fresh.h"
    "include/ac_mock.h"
    "include/ac_timing.h"
    "include/ac_thread.h"
    "src/ac_args.c"
    "src/ac_file.c"
    "src/ac_fresh.c"
    "src/ac_mock.c"
    "src/ac_timing.c"
    "src/ac_thread.c"
    "src/main.c")

# include directory
target_include_directories(${TARGET_NAME} PUBLIC
    "include")

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
