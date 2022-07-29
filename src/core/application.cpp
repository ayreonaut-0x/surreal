#include <core/application.hpp>

#if SURREAL_PLATFORM_LINUX
    #include <platform/linux/window.hpp>
#endif

#include <chrono>

#include <fmt/format.h>

namespace Surreal
{

Application* Application::s_instance{ nullptr };

Application::Application() : m_should_quit(false), m_window(nullptr)
{
    s_instance = this;
}

Application::~Application()
{
    s_instance = nullptr;
}

void Application::run()
{
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<float> Seconds;

#if SURREAL_PLATFORM_LINUX
    m_window = new LinuxWindow("Titan Application", WindowCreateFlagBits::VSync);
#endif
    m_window->push_event_handler(this);

    float elapsed_time{ 0.0f };
    auto start_time{ Clock::now() };

    while (!m_should_quit)
    {
        auto end_time{ Clock::now() };
        Seconds delta_time{ end_time - start_time };

        on_update(delta_time.count());
        m_window->on_update();

        elapsed_time += delta_time.count();
        start_time = end_time;
    }

    delete m_window;
}

void Application::on_update(SURREAL_UNUSED(float, delta_time)) {}

void Application::operator()(KeyEvent& ke)
{
    if (ke.get_type() == EventType::KeyPress)
    {
        auto& kp{ static_cast<KeyPressEvent&>(ke) };
        if (kp.get_key() == 9)
        {
            m_should_quit = true;
            kp.handled = true;
            return;
        }
    }
}

void Application::operator()(WindowEvent& we)
{
    if (we.get_type() == EventType::WindowClose)
    {
        auto& wc{ static_cast<WindowCloseEvent&>(we) };
        m_should_quit = true;
        wc.handled = true;
        return;
    }
}

// void Application::process_key_event(KeyEvent& ke)
// {
//     if (ke.get_type() == EventType::KeyPress)
//     {
//         auto& kp{ static_cast<KeyPressEvent&>(ke) };
//         if (kp.get_key() == 9)
//         {
//             m_should_quit = true;
//             kp.handled = true;
//             return;
//         }
//     }
// }

// void Application::process_window_event(WindowEvent& we)
// {
//     if (we.get_type() == EventType::WindowClose)
//     {
//         auto& wc{ static_cast<WindowCloseEvent&>(we) };
//         m_should_quit = true;
//         wc.handled = true;
//         return;
//     }
// }

} // namespace Surreal
