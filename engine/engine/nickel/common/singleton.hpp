#pragma once

#include <cassert>
#include <memory>

namespace nickel {

/**
 *  @brief some utilities
@{ */

/**
 * @brief
 * @tparam T the class you want to be singleton
 * @tparam ExplicitInit does you need explicit init/destroy the instance
 */
template <typename T, bool ExplicitInit>
class Singlton;

/**
 * @brief
 * @tparam T the class you want to be singleton
 */
template <typename T>
class Singlton<T, false> {
public:
    //! @brief Get the singleton of class T
    //! @return The singleton instance
    static T& GetInst() {
        static T instance;
        return instance;
    }
};

/**
 * @brief
 * use `Init()` and `Delete()` to create and destroy this is for which need
 * initialize/destroy in order
 * @tparam T the class you want to be singleton
 */
template <typename T>
class Singlton<T, true> {
public:
    /**
     * @brief get signelton instance
     * @return The singleton instance
     */
    static T& GetInst() {
        assert(instance_ != nullptr);
        return *instance_;
    }

    /**
     * @brief Explicit init the singleton.You must call this before call
     * `Instance()`
     * @param ...args the params pass to constructor of T
     * */
    template <typename... Args>
    static void Init(Args&&... args) {
        instance_ = std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Explicit destroy the singlton.You can't call `Instance()` after
     * call this
     */
    static void Delete() {
        instance_.reset();
        instance_ = nullptr;
    }

private:
    inline static std::unique_ptr<T> instance_;
};

}  // namespace nickel