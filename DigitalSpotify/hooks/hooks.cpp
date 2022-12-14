#include "hooks.h"

void hooks::instance( ) {
	if ( MH_Initialize( ) != MH_OK )
		return;

	static const auto create_track_target = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "68 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 89 8D ? ? ? ? 8B 45 18" ) );
	static const auto unk_target = static_cast< void* >( reinterpret_cast< std::uint8_t* >( 0x6A8BF9 ) );
	static const auto download_file = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "55 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 81 EC 10 01 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 F0 53 56 57 50 8D 45 F4 64 A3 00 00 00 00 8B F9 8D" ) );
	static const auto open_track_target = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "68 B0 01 00 00 B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F1 89" ) );
	static const auto has_ad_target = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "68 A8 00 00 00 B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 75 08 8D" ) );
	static const auto remove_ad_playback_target = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "55 8B EC 56 8B F1 57 8B 7D 0C 3B 7E 14 77 24" ) );

	HOOK( create_track_target, create_track, og::m_create_track );
	HOOK( unk_target, unk_sub, og::m_unk_sub );
	HOOK( open_track_target, open_track, og::m_open_track );
	HOOK( has_ad_target, has_ad, og::m_has_ad );

	//HOOK( remove_ad_playback_target, remove_ad_playback, og::m_remove_ad_playback );
	//HOOK( download_file, hooks::download_file, og::m_download_file );

	printf( "[ hook ] targets size: %u\n", hooks::m_targets.size( ) );
}

int __cdecl hooks::unk_sub( int a1, int arg4 ) {
	// @note: removes extra ads (advertiser, title, duration, ad_type, test_ad, non_explicit, skippable, line_item_id etc)
	return 0;
}

int __cdecl hooks::has_ad( int a1, int a2 ) {
	// @note: removes information about ads (xrefs: has_available_ads, has_active_ad, has_listeners, available_ads etc)
	return 0;
}

int __fastcall hooks::remove_ad_playback( void* ecx, void* edx, const char* Src, std::size_t size ) {
	if ( !std::strcmp( Src, "ad" ) || !std::strcmp( Src, "ad_playback" ) || !std::strcmp( Src, "spotify:ad:" ) )
		return 1;

	return og::m_remove_ad_playback( ecx, edx, Src, size );
}

int __fastcall hooks::download_file( void* ecx, void* edx ) {
	static const auto patch = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "8A 80 EC 00 00 00" ) );
	if ( _ReturnAddress( ) == patch )
		return 1;

	static const auto condition = static_cast< void* >( utils::find_sig( ctx::modules::m_spotify_image, "6A 09 FF D6" ) );
	if ( !( _ReturnAddress( ) == condition ) ) {
		char v31[ 44 ]{}; // [esp+74h] [ebp-B8h] BYREF
		static const auto sub = reinterpret_cast < void* ( __cdecl* )( int, int, unsigned int ) > ( utils::find_sig( ctx::modules::m_spotify_image, "55 8B EC 33 D2 56 8B 75 08 39 55 10 76 2A 57 8B 7D 0C 0F B6 0C 3A 8B C1 83 E1 0F C1 E8 04 8A 89" ) );
		sub( reinterpret_cast< int >( v31 ), *reinterpret_cast< std::uint32_t* >( reinterpret_cast< std::uintptr_t > ( ecx ) + 32 ), 0x14u );

		printf( "downloading and processing file id: spotify:file:%s\n", v31 );
	}

	return og::m_download_file( ecx, edx );
}

int __fastcall hooks::open_track(
	void* ecx,
	void* edx,
	int a2, // ctx::structs::player_meta_t
	void( __thiscall*** a3 )( DWORD*, int ),
	DWORD* a4,
	__int64 position,
	char paused,
	void* transition ) {
	printf( "[ spotify ] track opened\n" );

	if ( ctx::player_meta::m_should_skip /*a2->m_should_skip*/ ) {
		position = 1000000;
		// @note: ad skip bug fix
		paused = false;
	}

	printf( "[ spotify ] position: %lld ms, paused: %s, transition: %s\n", position, paused ? "true" : "false", utils::get_transition( a4[ 2 ] ) );

	return og::m_open_track( ecx, edx, a2, a3, a4, position, paused, transition );
}

void __fastcall hooks::run_ads( void* ecx, void* edx, void* a2, int* a3 ) {
	// @note: this function checks the type of ad that is playing (video, banner, audio) and then does something with slots and ads respectively
	// so lets skip all checks
	return;
}

void __fastcall hooks::create_track( void* ecx, void* edx, ctx::structs::player_meta_t* a2,
									 ctx::structs::player_track_meta_t* a3,
									 double speed,
									 int a5,
									 int a6,
									 int a7,
									 int a8,
									 int a9 ) {
	printf( "\n[ spotify ] track switched\n" );

	// @note: we must to skip the "track" if we have ads
	// a3->m_track_uri is like the name of what is currently playing ( @ida: https://prnt.sc/5g7h3pKqCouG )
	// before that it was just ad, but it crashed, then I noticed that you always need to add "spotify:" (this can be seen if you print m_track_uri to the console, there will be something like "spotify:track:7AQim7LbvFVZJE3O8TYEf3")
	ctx::player_meta::m_should_skip = a2->m_should_skip = static_cast< std::uint32_t >( static_cast< bool >( std::strstr( a3->m_track_uri, "spotify:ad:" ) ) );

	// @note: if the track is changed, advertising is turned on, then we set its speed to a very high value
	if (/*a4 == 1.0 && */a2->m_should_skip ) // 1.0 - def. speed value
		speed = 30.0; // or some other greater value

#ifdef _DEBUG
	printf( "[ spotify ] track uri: %s, speed: %f, type: %s, stream type: %s, urgency: %s, track_select_flag: %d\n", a3->m_track_uri, speed, utils::get_type( a5 ), utils::get_stream_type( a9 ), utils::get_urgency( a6 ), a7 );
#elif NDEBUG
	printf( "[ spotify ] speed: %f, type: %s, stream type: %s, urgency: %s, track_select_flag: %d\n", speed, utils::get_type( a5 ), utils::get_stream_type( a9 ), utils::get_urgency( a6 ), a7 );
#endif

	og::m_create_track( ecx, edx, a2, a3, speed, a5, a6, a7, a8, a9 );
}

void hooks::unhook( const LPVOID target ) {
	printf( MH_DisableHook( target ) == MH_OK
			? "[ hook ] disabled target: 0x%p\n"
			: "[ hook ] not disabled target: 0x%p\n", target );
}

void hooks::disable_all_hooks( ) {
	for ( const auto target : m_targets ) { unhook( target ); }
	MH_Uninitialize( );
}