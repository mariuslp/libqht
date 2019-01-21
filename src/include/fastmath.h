#pragma once

#include <cstdint>

/** Fast integer power of 2 */
inline constexpr std::uint64_t pow2 (std::uint64_t i)
{
	return std::uint64_t(1) << i;
}
