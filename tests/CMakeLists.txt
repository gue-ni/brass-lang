set(SRC_FILES "tests.cpp" "misc.cpp")

add_executable(tests ${SRC_FILES})

target_link_libraries(tests PRIVATE gtest gtest_main brass_lang)

include(GoogleTest)
gtest_discover_tests(tests)

if(UNIX)
  target_compile_options(tests PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
  target_link_options(tests PRIVATE -fsanitize=address)
endif()
