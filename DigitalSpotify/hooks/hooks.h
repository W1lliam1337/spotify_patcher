#pragma once
#include "../includes.h"
#include "../utils/utils.h"

#define HOOK(target, detour, og) \
	if (MH_CreateHook(target, detour, reinterpret_cast<void**>(&(og))) == MH_OK &&  MH_EnableHook(target) == MH_OK) {\
		printf("[ hook ] created target: 0x%p, detour: 0x%p, og: 0x%p\n", target, detour, og); hooks::m_targets.emplace_back(target);\
	}

namespace hooks {
	inline std::vector<LPVOID> m_targets{};
	void unhook( LPVOID target );
	void disable_all_hooks( );
	void instance( );
	void __fastcall create_track( void* ecx, void* edx, ctx::structs::player_meta_t* a2, ctx::structs::player_track_meta_t* a3, double speed, int stream_type, int a6, int a7, int a8, int a9 );
	int __cdecl unk_sub( int a1, int arg4 );
	int __cdecl has_ad( int a1, int a2 );
	int __fastcall download_file( void* ecx, void* edx );
	int __fastcall open_track( void* ecx, void* edx, int a2, void( __thiscall*** a3 )( DWORD*, int ), DWORD* a4, __int64 position, char paused, void* transition );
	int __fastcall remove_ad_playback( void* ecx, void* edx, const char* Src, std::size_t size );
	void __fastcall run_ads( void* ecx, void* edx, void* a2, int* a3 );

	namespace og {
		inline decltype( &run_ads ) m_run_ads;
		inline decltype( &remove_ad_playback ) m_remove_ad_playback;
		inline decltype( &has_ad ) m_has_ad;
		inline decltype( &open_track ) m_open_track;
		inline decltype( &download_file ) m_download_file;
		inline decltype( &create_track ) m_create_track;
		inline decltype( &unk_sub ) m_unk_sub;
	}
};