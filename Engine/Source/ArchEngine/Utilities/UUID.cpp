#include "ArchPch.h"
#include "UUID.h"

namespace ae {
	UUID::UUID() {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint64_t> dis;
		_uuid = dis(gen);
	}

	UUID::UUID(uint64_t uuid) : _uuid(uuid) {}
	UUID::UUID(const std::string& uuidStr) {
		_uuid = std::stoull(uuidStr, nullptr, 16);
	}

	std::string UUID::ToString() const {
		std::stringstream ss;
		ss << std::hex << _uuid;
		return ss.str();
	}
}