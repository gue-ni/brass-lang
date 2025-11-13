#pragma once

#include <iostream>
#include <ostream>

int eval( const char * src, std::ostream & out = std::cout, std::ostream & err = std::cerr );

int repl();

int brass( int argc, char * argv[] );
