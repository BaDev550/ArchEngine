#pragma once
#include <iostream>
#include <random>
#include <sstream>

namespace ae {
	constexpr uint64_t HashString(const char* str) {
		uint64_t hash = 14695981039346656037ull;
		while (*str) {
			hash ^= (uint64_t)(*str++);
			hash *= 1099511628211ull;
		}
		return hash;
	}
	inline uint64_t HashString(const std::string& str) { return HashString(str.c_str()); }

	class UUID {
	public: 
		UUID();
		UUID(uint64_t uuid);
		UUID(const std::string& uuidStr);
		std::string ToString() const;

		operator uint64_t() const { return _uuid; }
	private:
		uint64_t _uuid;
	};
}

namespace std {
	template<>
	struct hash<ae::UUID> {
		size_t operator()(const ae::UUID& uuid) const {
			return hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}