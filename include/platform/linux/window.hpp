#pragma once

#include <core/window.hpp>

#include <core/event.hpp>

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_util.h>

#include <string>
#include <string_view>
#include <unordered_map>

namespace Surreal
{

class LinuxWindow : public Window
{
public:
    LinuxWindow(const std::string& title, WindowCreateFlags);
    ~LinuxWindow() override;

    constexpr Size get_size() const noexcept override { return m_rect.size; }
    constexpr Position get_position() const noexcept override { return m_rect.pos; }
    constexpr Rect get_rect() const noexcept override { return m_rect; }

    void on_update() override;

    void show() noexcept override;
    void hide() noexcept override;

private:
    Rect m_rect;
    xcb_window_t m_wid;

    struct Atom
    {
        xcb_atom_t atom;
        xcb_atom_t type;
        u8 size;
    };

    std::unordered_map<std::string_view, Atom> m_atoms;
    xcb_atom_t m_wm_protocols_atom;
    xcb_atom_t m_wm_delete_window_atom;

private:
    static xcb_connection_t* s_connection;
    static u32 s_window_count;

    static constexpr std::string_view s_wm_protocols_name{ "WM_PROTOCOLS" };
    static constexpr std::string_view s_wm_delete_window_name{ "WM_DELETE_WINDOW" };

    void on_client_message(xcb_client_message_event_t*);
    void on_configure_notify(xcb_configure_notify_event_t*);
    void on_key_press(xcb_key_press_event_t*);
    void on_key_release(xcb_key_release_event_t*);
    void on_button_press(xcb_button_press_event_t*);
    void on_button_release(xcb_button_release_event_t*);
};

} // namespace Surreal
