#pragma once

#include "base.hpp"
#include "event.hpp"
#include "window.hpp"

namespace Surreal
{

class SURREAL_API Application : public EventHandler
{
public:
    Application();
    virtual ~Application();

    void run();

    void on_update(f32 delta_time);

    void on_event(KeyEvent&) override;
    void on_event(MouseEvent&) override;
    void on_event(WindowEvent&) override;

    // void process_key_event(KeyEvent&) override;
    // void process_window_event(WindowEvent&) override;

private:
    static Application* s_instance;

private:
    bool m_should_quit;
    Window* m_window;
};

} // namespace Surreal
