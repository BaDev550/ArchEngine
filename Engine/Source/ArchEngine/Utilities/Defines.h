#pragma once
#include "Logger.h"

#ifdef _DEBUG
#define CHECKF(x, ...) \
	if (!(x)) \
		throw std::runtime_error(__VA_ARGS__);
#ifdef _MSC_VER 
	#define DUMP_MEMORY_LEAKS _CrtDumpMemoryLeaks()
#endif
#else
	#define CHECKF(x, ...) \
		if (!(x)) \
			ae::Logger_app::error(__VA_ARGS__);
	#define DUMP_MEMORY_LEAKS
#endif

#define BIT(x) x << 1