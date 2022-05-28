#pragma once

#pragma warning(disable: 4100)  // unreferenced formal parameter
#pragma warning(disable: 4121)  // alignment of a member was sensitive to packing
#pragma warning(disable: 4201)  // nonstandard extension used : nameless struct / union
#pragma warning(disable: 4244)  // conversion from 'x' to 'y', possible loss of data
#pragma warning(disable: 4267)  // conversion from 'x' to 'y', possible loss of data
#pragma warning(disable: 4324)  // structure was padded due to alignment specifier
#pragma warning(disable: 4389)  // signed/unsigned mismatch
#pragma warning(disable: 4838)  // conversion from 'unsigned int' to 'int' requires a narrowing conversion
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '*' on a 4 byte value and then casting the result to a 8 byte value.Cast the value to the wider type before calling operator '*' to avoid overflow
#pragma warning(disable: 26478) // std::move
#pragma warning(disable: 26495) // x is uninitialized. Always initialize a member variable
#pragma warning(disable: 26498) // x is constexpr, mark variable 'x' constexpr if compile - time evaluation is desired
#pragma warning(disable: 26812) // The enum type 'x' is unscoped.Prefer 'enum class' over 'enum'
#pragma warning(disable: 26819) // Unannotated fallthrough
#pragma warning(disable: 28020) // 

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <corecrt_io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <csetjmp> 
#include <ShlObj.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <Psapi.h>
#include <urlmon.h>
#include <iphlpapi.h>
#include <d3d11.h>
#include <winternl.h>
#include <setjmp.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <array>
#include <map>
#include <atomic>
#include <vector>
#include <mutex>
#include <queue>
#include <regex>
#include <chrono>
#include <thread>
#include <fstream>
#include <utility>
#include <filesystem>
#include <functional>
#include <sstream>
#include <optional>
#include <unordered_set>
#include <iostream>
#include <variant>
#include <bitset>
#include <stack>

using namespace std::literals;