#pragma once

#ifdef NICKEL_DEF_DLLEXPORT
#define NICKEL_API __declspec(dllexport)
#else
#define NICKEL_API
#endif