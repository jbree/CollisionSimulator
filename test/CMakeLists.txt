add_subdirectory(googletest)
include(CTest)
include(GoogleTest)

function(create_test TEST_NAME)
    add_executable("${TEST_NAME}Test" "${TEST_NAME}Test.cc")
    target_link_libraries("${TEST_NAME}Test"
        ${PROJECT_NAME} ${ARGS}
        gtest_main gmock)
    gtest_discover_tests(${TEST_NAME}Test)
endfunction()

create_test(Station collision_sim)