#pragma once
#include <iostream>
#include <random>
#include <sstream>

namespace ae {
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