#pragma once

#include "base.hpp"
#include "event.hpp"
#include "exception.hpp"
#include "flags.hpp"

#include <vector>

namespace Surreal
{

#define BIT(n) (1 << n)
enum struct WindowCreateFlagBits : u32
{
    Borderless = BIT(0),
    Fullscreen = BIT(1),
    VSync = BIT(2),
};
#undef BIT

template <>
struct FlagTraits<WindowCreateFlagBits>
{
    typedef WindowCreateFlagBits Bits;
    typedef std::underlying_type<WindowCreateFlagBits>::type MaskType;

    static constexpr MaskType AllFlags{ MaskType(Bits::Borderless) | MaskType(Bits::Fullscreen) |
                                        MaskType(Bits::VSync) };
};

typedef Flags<WindowCreateFlagBits> WindowCreateFlags;

class WindowError : public RuntimeError
{
public:
    explicit WindowError(const std::string& msg) : RuntimeError(msg) {}
};

class Window
{
public:
    virtual ~Window() = default;

    void push_event_handler(EventHandler* eh) { m_event_handlers.emplace_back(eh); }

    virtual constexpr Size get_size() const noexcept = 0;
    virtual constexpr Position get_position() const noexcept = 0;
    virtual constexpr Rect get_rect() const noexcept = 0;

    virtual void on_update() = 0;
    virtual void show() noexcept = 0;
    virtual void hide() noexcept = 0;

protected:
    Window(u64 id) : m_id(id), m_event_handlers() {}

    u64 m_id;
    std::vector<EventHandler*> m_event_handlers;
};

} // namespace Surreal
