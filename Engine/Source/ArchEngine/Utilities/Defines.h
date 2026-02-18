#pragma once
#include "Logger.h"

#define CHECKF(x, ...) \
	if (!(x)) \
		throw std::runtime_error(__VA_ARGS__);
#ifdef _DEBUG
#ifdef _MSC_VER 
	#define DUMP_MEMORY_LEAKS _CrtDumpMemoryLeaks()
#endif
#else
	#define DUMP_MEMORY_LEAKS
#endif

#define BIT(x) x << 1