#pragma once

#include <cstring>
#include <iostream>

#define MAX_REQUEST_QUEUE 1000

#define FOR_EACH(item, container) \
    for (auto item = std::begin(container); item != std::end(container); ++item)

#define FOR_EACH_AUTO(container, lambda) \
    do { \
        for (auto it = std::begin(container); it != std::end(container); ++it) { \
            lambda(*it); \
        } \
    } while(0)

#define FIND_AUTO(container, value) \
    [&]() { \
        for (auto it = std::begin(container); it != std::end(container); ++it) { \
            if (*it == value) { \
                return it; \
            } \
        } \
        return std::end(container); \
    }()

/// Branch prediction hints.
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

/// Check condition and exit if not true.
inline auto ASSERT(bool cond, const std::string &msg) noexcept {
	if (UNLIKELY(!cond)) {
		std::cerr << "ASSERT : " << msg << std::endl;

		exit(EXIT_FAILURE);
	}
}

inline auto FATAL(const std::string &msg) noexcept {
	std::cerr << "FATAL : " << msg << std::endl;

	exit(EXIT_FAILURE);
}
