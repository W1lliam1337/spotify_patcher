#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <cstdio>
#include <intrin.h>
#include <MinHook.h>
#pragma comment( lib, "MinHook.lib" )

namespace ctx {
	namespace structs {
		// @credits: es3n1n
		struct player_meta_t {
		private:
			char pad[ 0x74 ]{};
		public:
			std::uint32_t m_should_skip{};
		};

		// @credits: es3n1n
		struct player_track_meta_t {
		private:
			char pad[ 0x98 ]{};
		public:
			const char* m_track_uri{};
		};
	}

	namespace player_meta {
		inline std::uint32_t m_should_skip{};
	}

	namespace modules {
		inline HMODULE m_spotify_image{};
		inline HMODULE m_module_handle{};
	}
};