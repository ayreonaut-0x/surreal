#include <platform/linux/window.hpp>

#include <core/event.hpp>
#include <core/exception.hpp>

#include <fmt/format.h>

namespace Surreal
{

static constexpr u32 s_width{ 1280u };
static constexpr u32 s_height{ 720u };

xcb_connection_t* LinuxWindow::s_connection{ nullptr };
u32 LinuxWindow::s_window_count{ 0u };

LinuxWindow::LinuxWindow(const std::string& title, WindowCreateFlags flags)
    : Window(std::hash<std::string>()(title)), m_rect(), m_wid(static_cast<xcb_window_t>(-1)),
      m_atoms({ { s_wm_protocols_name, { static_cast<xcb_atom_t>(-1), XCB_ATOM_ATOM, 32 } },
                { s_wm_delete_window_name, { static_cast<xcb_atom_t>(-1), XCB_ATOM_STRING, 8 } } })
{
    if (!s_connection)
    {
        s_connection = xcb_connect(nullptr, nullptr);
        if (xcb_connection_has_error(s_connection))
            throw WindowError("Failed to connect to X server.");
    }

    const xcb_screen_t* screen{ xcb_setup_roots_iterator(xcb_get_setup(s_connection)).data };

    const u32 half_screen_width{ screen->width_in_pixels / 2u };
    const u32 half_screen_height{ screen->height_in_pixels / 2u };
    const u32 half_window_width{ s_width / 2u };
    const u32 half_window_height{ s_height / 2u };

    m_rect = (flags & WindowCreateFlagBits::Fullscreen)
                 ? Rect{ { 0u, 0u }, { screen->width_in_pixels, screen->height_in_pixels } }
                 : Rect{ { half_screen_width - half_window_width, half_screen_height - half_window_height },
                         { s_width, s_height } };

    constexpr u32 cw_mask{ XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK };
    const u32 cw_list[2]{ screen->black_pixel, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                                                   XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                                                   XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY };

    m_wid = xcb_generate_id(s_connection);
    if (m_wid == static_cast<xcb_window_t>(-1))
        throw WindowError("X server returned an invalid ID.");

    xcb_create_window(s_connection, screen->root_depth, m_wid, screen->root, static_cast<i16>(m_rect.pos.x),
                      static_cast<i16>(m_rect.pos.y), static_cast<u16>(m_rect.size.w), static_cast<u16>(m_rect.size.h),
                      0u, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, cw_mask, cw_list);

    for (auto& [name, atom] : m_atoms)
    {
        auto reply{ xcb_intern_atom_reply(
            s_connection, xcb_intern_atom(s_connection, true, static_cast<u16>(name.length()), name.data()), nullptr) };

        atom.atom = reply->atom;
        free(reply);
    }

    xcb_change_property(s_connection, XCB_PROP_MODE_REPLACE, m_wid, m_atoms[s_wm_protocols_name].atom,
                        m_atoms[s_wm_protocols_name].type, m_atoms[s_wm_protocols_name].size, 1u,
                        &m_atoms[s_wm_delete_window_name].atom);

    show();

    ++s_window_count;
}

LinuxWindow::~LinuxWindow()
{
    xcb_destroy_window(s_connection, m_wid);
    --s_window_count;

    if (!s_window_count)
        xcb_disconnect(s_connection);
}

void LinuxWindow::on_update()
{
    xcb_generic_event_t* generic_event{ nullptr };
    while ((generic_event = xcb_poll_for_event(s_connection)))
    {
        switch (XCB_EVENT_RESPONSE_TYPE(generic_event))
        {
        case XCB_BUTTON_PRESS:
            on_button_press(reinterpret_cast<xcb_button_press_event_t*>(generic_event));
            break;
        case XCB_BUTTON_RELEASE:
            on_button_release(reinterpret_cast<xcb_button_release_event_t*>(generic_event));
            break;
        case XCB_CLIENT_MESSAGE:
            on_client_message(reinterpret_cast<xcb_client_message_event_t*>(generic_event));
            break;
        case XCB_CONFIGURE_NOTIFY:
            on_configure_notify(reinterpret_cast<xcb_configure_notify_event_t*>(generic_event));
            break;
        case XCB_KEY_PRESS:
            on_key_press(reinterpret_cast<xcb_key_press_event_t*>(generic_event));
            break;
        case XCB_KEY_RELEASE:
            on_key_release(reinterpret_cast<xcb_key_release_event_t*>(generic_event));
            break;
        default:
            break;
        }

        free(generic_event);
    }
}

void LinuxWindow::show() noexcept
{
    xcb_map_window(s_connection, m_wid);
    xcb_flush(s_connection);
}

void LinuxWindow::hide() noexcept
{
    xcb_unmap_window(s_connection, m_wid);
    xcb_flush(s_connection);
}

void LinuxWindow::on_client_message(xcb_client_message_event_t* client_message)
{
    for (auto phandler{ m_event_handlers.rbegin() }; phandler != m_event_handlers.rend(); ++phandler)
    {
        auto handler{ *phandler };

        if (client_message->data.data32[0] == m_atoms[s_wm_delete_window_name].atom)
        {
            WindowCloseEvent e{ m_id };
            (*handler)(e);
        }
    }
}

void LinuxWindow::on_configure_notify(xcb_configure_notify_event_t* config_notify)
{
    if (config_notify->window == m_wid)
    {
        m_rect.pos.x = config_notify->x;
        m_rect.pos.y = config_notify->y;
        m_rect.size.w = config_notify->width;
        m_rect.size.h = config_notify->height;

        for (auto phandler{ m_event_handlers.rbegin() }; phandler != m_event_handlers.rend(); ++phandler)
        {
            auto handler{ *phandler };
            WindowResizeEvent s{ m_id, m_rect.size };
            (*handler)(s);
            WindowPositionEvent p{ m_id, m_rect.pos };
            (*handler)(p);
        }
    }
}

void LinuxWindow::on_key_press(xcb_key_press_event_t* key_press)
{
    for (auto phandler{ m_event_handlers.rbegin() }; phandler != m_event_handlers.rend(); ++phandler)
    {
        auto handler{ *phandler };
        KeyPressEvent e{ key_press->detail };
        (*handler)(e);
    }
}

void LinuxWindow::on_key_release(xcb_key_release_event_t* key_release)
{
    for (auto phandler{ m_event_handlers.rbegin() }; phandler != m_event_handlers.rend(); ++phandler)
    {
        auto handler{ *phandler };
        KeyReleaseEvent e{ key_release->detail };
        (*handler)(e);
    }
}

void LinuxWindow::on_button_press(xcb_button_press_event_t* button_press)
{
    fmt::print("Button pressed: {}\n", button_press->detail);
}

void LinuxWindow::on_button_release(xcb_button_release_event_t* button_release)
{
    fmt::print("Button released: {}\n", button_release->detail);
}

} // namespace Surreal
