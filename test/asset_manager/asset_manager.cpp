#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "common/asset_manager.hpp"

struct Person {
    std::string name;
    float height;
};

template <>
class nickel::LoadStoreStrategy<Person> : public LoadStoreStrategyHelper {
public:
    using AssetType = Person;

    static std::string_view GetMetaExtension() { return ".person"; }

    static bool IsExternal() { return false; }

    bool Load(const std::filesystem::path& filename, Person* mem) {
        std::ifstream file(filename);
        if (file.fail()) {
            return false;
        }

        Person person;
        file >> person.name;
        file >> person.height;

        ChangeRelativePath(filename);

        new (mem) Person(std::move(person));
        return true;
    }

    bool Save(const Person& person) const {
        std::ofstream file(GetRelativePath());
        if (file.fail()) {
            return false;
        }

        file << person.name;
        file << person.height;
        return true;
    }
};

TEST_CASE("asset manager") {
    nickel::AssetManager::Init();

    auto& mgr = nickel::AssetManager::Instance();
    mgr.RegisterInternalAssetType<Person>();

    SECTION("save and load") {
        Person person{"VisualGMQ", 188.0f};
        auto handle =
            mgr.Load<Person>("./test/asset_manager/nickel_test_person.person");
        auto p = mgr.Get(handle);

        REQUIRE(p->name == "VisualGMQ");
        REQUIRE(p->height == 188);
    }

    nickel::AssetManager::Delete();
}
