#pragma once
#include "stdpch.hpp"

namespace nickel {

struct CmdLineArgs {
    CmdLineArgs(int argc, char** argv) {
        for (int i = 0; i < argc; i++) {
            args_.emplace_back(argv[i]);
        }
    }

    auto& Args() const { return args_; }

private:
    std::vector<std::string_view> args_;
};

}  // namespace nickel