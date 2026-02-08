#pragma once

#define CHECKF(x, ...) \
	if (!(x)) \
		throw std::runtime_error(__VA_ARGS__);