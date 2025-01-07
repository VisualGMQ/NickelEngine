#include "nickel/nickel.hpp"
#include "nickel/main_entry/runtime.hpp"

class Application: public nickel::Application {
    
};

std::unique_ptr<nickel::Application> CreateCustomApplication(nickel::Context&) {
    return std::make_unique<Application>();
}

