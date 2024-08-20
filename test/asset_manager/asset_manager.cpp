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

    static bool IsImportable() { return false; }

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

struct AlwaysFailedData { };

template <>
class nickel::LoadStoreStrategy<AlwaysFailedData>: public LoadStoreStrategyHelper {
public:
    using AssetType = AlwaysFailedData;

    static std::string_view GetMetaExtension() { return ".alwaysFailed"; }

    static bool IsImportable() { return false; }

    bool Load(const std::filesystem::path& filename, AlwaysFailedData* mem) {
        return false;
    }

    bool Save(const AlwaysFailedData& person) const {
        return false;
    }
};

TEST_CASE("asset manager") {
    nickel::AssetManager::Init();

    auto& mgr = nickel::AssetManager::Instance();
    mgr.RegisterInternalAssetType<Person>();
    mgr.RegisterInternalAssetType<AlwaysFailedData>();

    SECTION("save and load") {
        std::filesystem::path path = "./test/asset_manager/nickel_test_person.person";
        auto handle =
            mgr.Load<Person>(path);
        REQUIRE(handle);

        auto p = mgr.Get(handle);
        REQUIRE(p);
        REQUIRE(p->name == "VisualGMQ");
        REQUIRE(p->height == 188);
        REQUIRE(mgr.GetRelativePath(handle) == path);
    }

    SECTION("create") {
        Person person{"VisualGMQ", 188.0f};
        auto handle = mgr.Create<Person>(std::move(person));
        REQUIRE(handle);

        auto p = mgr.Get(handle);
        REQUIRE(p->name == "VisualGMQ");
        REQUIRE(p->height == 188);
        REQUIRE(mgr.GetRelativePath(handle).empty());
    }

    SECTION("load on failed") {
        auto handle = mgr.Load<AlwaysFailedData>("invalid_path");

        REQUIRE_FALSE(handle);
    }

    nickel::AssetManager::Delete();
}
