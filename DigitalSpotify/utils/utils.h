#pragma once
#include "../includes.h"
#include "../hooks/hooks.h"

namespace utils {
	void open_console( );
	void shutdown( );
	void init_modules( HMODULE base );
	std::uint8_t* find_sig( HMODULE module, const std::string& byte_array/*, std::vector<int> pattern_bytes_ = {NULL}*/ );
	const char* get_type( int a1 );
	const char* get_stream_type( int a1 );
	const char* get_urgency( int a1 );
	void update_target_track( char* v31, int arg2, unsigned int arg3 );
	const char* get_transition( const int a1 );
};