#pragma once

#ifdef NICKEL_DEF_DLLEXPORT
#ifdef NICKEL_PLATFORM_WIN32
#define NICKEL_API __declspec(dllexport)
#endif
#endif

#ifndef NICKEL_API
#define NICKEL_API
#endif
