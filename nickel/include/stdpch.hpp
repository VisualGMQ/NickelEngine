#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <cstring>
#include <type_traits>
#include <variant>
#include <charconv>
#include <functional>
#include <optional>
#include <set>

#if _WIN32
#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
#endif
