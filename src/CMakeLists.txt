set(SRC "vm.cpp" "parser.cpp" "lexer.cpp" "ast.cpp" "gc.cpp" "object.cpp" "bytecode.cpp" "brass.cpp")
set(INC "vm.h" "parser.h" "lexer.h" "ast.h" "gc.h" "object.h" "bytecode.h" "brass.h")

add_library(brass_lang STATIC ${SRC} ${INC})

target_include_directories(brass_lang PUBLIC ${CMAKE_BINARY_DIR})
target_include_directories(brass_lang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(brass "main.cpp")

target_link_libraries(brass brass_lang)
