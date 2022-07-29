#pragma once

#include "base.hpp"
#include "event.hpp"
#include "window.hpp"

namespace Surreal
{

class Application : public EventHandler
{
public:
    Application();
    virtual ~Application();

    void run();

    void on_update(float delta_time);

    void operator()(KeyEvent&) override;
    void operator()(WindowEvent&) override;

    // void process_key_event(KeyEvent&) override;
    // void process_window_event(WindowEvent&) override;

private:
    static Application* s_instance;

private:
    bool m_should_quit;
    Window* m_window;
};

} // namespace Surreal
