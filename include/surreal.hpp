#pragma once

#include <core/application.hpp>
#include <core/window.hpp>

#define SURREAL_DEFINE_APP_ENTRY(app_class)                                                                            \
    static_assert(std::derived_from<app_class, Surreal::Application>,                                                  \
                  "Must provide a Surreal::Application-derived class to SURREAL_DEFINE_APP_ENTRY.");                   \
                                                                                                                       \
    int main()                                                                                                         \
    {                                                                                                                  \
        auto app = new app_class();                                                                                    \
        app->run();                                                                                                    \
        delete app;                                                                                                    \
    }
