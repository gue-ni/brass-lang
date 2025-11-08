#pragma once

using uint = unsigned int;

#ifdef WIN32
#define STRDUP _strdup
#else
#define STDDUP strdup
#endif
