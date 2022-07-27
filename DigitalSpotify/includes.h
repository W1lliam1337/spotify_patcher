#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <minhook.h>
#include <cstdio>
#include <intrin.h>

namespace ctx
{
	namespace structs
	{
		// @credits: es3n1n
		struct player_meta_t
		{
			char pad[0x74]{};
			std::uint32_t m_should_skip{};
		};

		// @credits: es3n1n
		struct player_track_meta_t
		{
			char pad[0x48]{};
			const char* m_track_uri{};
		};
	}
	namespace modules
	{
		inline HMODULE m_spotify_image{};
		inline HMODULE m_module_handle{};
	}
};