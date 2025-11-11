set(SRC_FILES "tests.cpp" "misc.cpp")

add_executable(tests ${SRC_FILES})

target_link_libraries(tests PRIVATE gtest gtest_main brass_lang)

include(GoogleTest)
gtest_discover_tests(tests)