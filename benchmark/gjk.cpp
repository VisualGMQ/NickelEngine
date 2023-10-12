#include "nanobench.hpp"

#include "nickel.hpp"

constexpr int BenchNum = 10000;

using namespace nickel;

using geom_type = std::vector<cgmath::Vec2>;

int main() {
    geom_type geomProto{
        {-2,  7},
        {-6,  3},
        {-3, -3},
        { 4, -4},
    };

    std::vector<std::pair<geom_type, geom_type>> geomPairs;

    std::random_device d;
    std::uniform_int_distribution dist(-50, 50);

    for (int i = 0; i < BenchNum; i++) {
        auto g1 = geomProto;
        auto g2 = geomProto;
        cgmath::Vec2 offset1(dist(d), dist(d)),
                     offset2(dist(d), dist(d));

        for (auto& v : g1) {
            v += offset1;
        }

        for (auto& v : g2) {
            v += offset2;
        }

        geomPairs.push_back({std::move(g1), std::move(g2)});
    }

    ankerl::nanobench::Bench().run("gjk algorithm", [&] {
        for (auto& [g1, g2] : geomPairs) {
            geom2d::Gjk(g1, g2);
        }
    });

    return 0;
}
