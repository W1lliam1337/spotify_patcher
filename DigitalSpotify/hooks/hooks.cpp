#include "hooks.h"

void hooks::instance()
{
	if (MH_Initialize() != MH_OK)
		return;

	static const auto create_track_target = static_cast<void*>(utils::find_sig(ctx::modules::m_spotify_image, "68 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 89 8D ? ? ? ? 8B 45 18"));
	static const auto playback_track_target = static_cast<void*>(utils::find_sig(ctx::modules::m_spotify_image, "6A 70 B8 ? ? ? ? E8 ? ? ? ? 8B D9 8B 45 14"));

	HOOK(create_track_target, create_track, og::m_create_track);
	//HOOK(playback_track_target, playback_track, og::m_playback_track);
}

void __fastcall hooks::create_track(void* ecx, void* edx, ctx::structs::player_meta_t* a2,  
	ctx::structs::player_track_meta_t* a3,
	double a4,
	int a5,
	int a6,
	int a7,
	int a8,
	int a9)
{
	printf("[ spotify ] track switched\n");

	// @note: we must to skip the "track" if we have ads
	// a3->m_track_uri is like the name of what is currently playing ( @ida: https://prnt.sc/5g7h3pKqCouG )
	// before that it was just ad, but it crashed, then I noticed that you always need to add "spotify:" (this can be seen if you print m_track_uri to the console, there will be something like "spotify:track:7AQim7LbvFVZJE3O8TYEf3")
	a2->m_should_skip = static_cast<std::uint32_t>(static_cast<bool>(strstr(a3->m_track_uri, "spotify:ad:")));

	// @note: if the track is changed, advertising is turned on, then we set its speed to a very high value
	if (/*a4 == 1.0 && */a2->m_should_skip) // 1.0 - def. speed value
		a4 = 30.0; // or some other greater value

	#ifdef _DEBUG
		printf("[ spotify ] track uri: %s, speed: %f, type: %s, stream type: %s, urgency: %s, track_select_flag: %d\n", a3->m_track_uri, a4, utils::get_type(a5), utils::get_stream_type(a9), utils::get_urgency(a6), a7);
	#elif NDEBUG
		printf("[ spotify ] speed: %f, type: %s, stream type: %s, urgency: %s, track_select_flag: %d\n", a4, utils::get_type(a5), utils::get_stream_type(a9), utils::get_urgency(a6), a7);
	#endif

	og::m_create_track(ecx, edx, a2, a3, a4, a5, a6, a7, a8, a9);
}

void __fastcall hooks::playback_track(void* ecx, void* edx, void* a2, double a3, int a4, int a5, int a6, int a7)
{
	og::m_playback_track(ecx, edx, a2, a3, a4, a5, a6, a7);
}

void hooks::unhook(const LPVOID target)
{
	printf(MH_DisableHook(target) == MH_OK
		? "[ hook ] disabled target: 0x%p\n"
		: "[ hook ] not disabled target: 0x%p\n", target);
}

void hooks::disable_all_hooks()
{
	for (const auto target : m_targets) { unhook(target); }
	MH_Uninitialize();
}