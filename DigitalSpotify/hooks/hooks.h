#pragma once
#include "../includes.h"
#include "../utils/utils.h"

//typedef void(__fastcall* create_track_fn)(void*, void*, ctx::structs::player_meta_t*, ctx::structs::player_track_meta_t*, double, int, int, int, int, int);
//typedef void(__fastcall* playback_track_fn)(void*, void*, void*, double, int, int, int, int);

#define HOOK(target, detour, og) \
	if (MH_CreateHook(target, detour, reinterpret_cast<void**>(&(og))) == MH_OK &&  MH_EnableHook(target) == MH_OK) {\
		printf("[ hook ] created target: 0x%p, detour: 0x%p, og: 0x%p\n", target, detour, og); hooks::m_targets.emplace_back(target); printf("[ hook ] targets size: %u\n", hooks::m_targets.size());\
	}

namespace hooks
{
	inline std::vector<LPVOID> m_targets{};
	void unhook(LPVOID target);
	void __fastcall create_track(void* ecx, void* edx, ctx::structs::player_meta_t* a2, ctx::structs::player_track_meta_t* a3, double a4, int a5, int a6, int a7, int a8, int a9);
	void __fastcall playback_track(void* ecx, void* edx, void* a2, double a3, int a4, int a5, int a6, int a7);
	void disable_all_hooks();
	void instance();
	int __cdecl unk_sub(int a1, int arg4);

	namespace og
	{
		inline decltype(&create_track) m_create_track;
		inline decltype(&playback_track) m_playback_track;
		inline decltype(&unk_sub) m_unk_sub;
	}
};