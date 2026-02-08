#pragma once
#include <iostream>
#include <format>

namespace ae {
#define FORMAT_STRING(msg, ...) std::vformat(msg, std::make_format_args(__VA_ARGS__))
#define LOG_MESSAGE_CUSTOM_HEADER(header, level, msg, ...) std::cout << "[" << header << "]" << "[" << level << "]" << FORMAT_STRING(msg, __VA_ARGS__) << std::endl;
	class Logger_app {
	public:
		template<typename... Args>
		static void trace(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("APP", "TRACE", msg, args...);
		}
		template<typename... Args>
		static void info(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("APP", "INFO", msg, args...);
		}
		template<typename... Args>
		static void warn(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("APP", "WARN", msg, args...);
		}
		template<typename... Args>
		static void error(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("APP", "ERROR", msg, args...);
		}
	};

	class Logger_renderer {
	public:
		template<typename... Args>
		static void trace(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("RENDERER", "TRACE", msg, args...);
		}
		template<typename... Args>
		static void info(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("RENDERER", "INFO", msg, args...);
		}
		template<typename... Args>
		static void warn(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("RENDERER", "WARN", msg, args...);
		}
		template<typename... Args>
		static void error(const char* msg, Args&&... args) {
			LOG_MESSAGE_CUSTOM_HEADER("RENDERER", "ERROR", msg, args...);
		}
	};
}