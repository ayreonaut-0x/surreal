#pragma once

#include "base.hpp"

#include <string>

#include <fmt/format.h>

namespace Surreal
{

#define bit(n) (1 << n)
enum struct EventCategoryFlagBits : u32
{
    None = 0u,
    Window = bit(0),
    Keyboard = bit(1),
    Mouse = bit(2),
};
#undef bit

template <>
struct FlagTraits<EventCategoryFlagBits>
{
    typedef EventCategoryFlagBits Bits;
    typedef std::underlying_type<EventCategoryFlagBits>::type MaskType;

    static constexpr MaskType AllFlags{ MaskType(Bits::None) | MaskType(Bits::Window) | MaskType(Bits::Keyboard) |
                                        MaskType(Bits::Mouse) };
};

typedef Flags<EventCategoryFlagBits> EventCategoryFlags;

enum struct EventType : u32
{
    KeyPress,
    KeyRelease,
    WindowClose,
    WindowPosition,
    WindowResize,
};

class Event
{
public:
    Event(const Event&) = delete;
    Event(Event&&) = delete;

    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&) = delete;

    virtual ~Event() = default;

    virtual constexpr EventCategoryFlags get_categories() const noexcept = 0;
    virtual constexpr EventType get_type() const noexcept = 0;
    virtual constexpr const char* get_name() const noexcept = 0;
    virtual std::string to_string() const noexcept { return get_name(); }

    bool handled{ false };

protected:
    Event() = default;
};

#define SURREAL_DECLARE_EVENT_CATEGORIES(cat_flags)                                                                    \
    static constexpr EventCategoryFlags get_static_categories() noexcept                                               \
    {                                                                                                                  \
        return cat_flags;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    constexpr EventCategoryFlags get_categories() const noexcept override                                              \
    {                                                                                                                  \
        return get_static_categories();                                                                                \
    }

#define SURREAL_DECLARE_EVENT_TYPE(type_value)                                                                         \
    static constexpr EventType get_static_type() noexcept                                                              \
    {                                                                                                                  \
        return EventType::type_value;                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    constexpr EventType get_type() const noexcept override                                                             \
    {                                                                                                                  \
        return get_static_type();                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    constexpr const char* get_name() const noexcept override                                                           \
    {                                                                                                                  \
        return "EventType::" #type_value;                                                                              \
    }

class WindowEvent : public Event
{
public:
    WindowEvent(u64 id) : m_id(id) {}

    SURREAL_DECLARE_EVENT_CATEGORIES(EventCategoryFlagBits::Window);

    constexpr u64 get_id() const noexcept { return m_id; }

private:
    u64 m_id;
};

class WindowCloseEvent final : public WindowEvent
{
public:
    WindowCloseEvent(u64 id) : WindowEvent(id) {}

    SURREAL_DECLARE_EVENT_TYPE(WindowClose);
};

class WindowPositionEvent final : public WindowEvent
{
public:
    WindowPositionEvent(u64 id, Position pos) : WindowEvent(id), m_pos(pos) {}

    SURREAL_DECLARE_EVENT_TYPE(WindowPosition);

    constexpr const Position& get_position() const noexcept { return m_pos; }

private:
    Position m_pos;
};

class WindowResizeEvent final : public WindowEvent
{
public:
    WindowResizeEvent(u64 id, Size size) : WindowEvent(id), m_size(size) {}

    SURREAL_DECLARE_EVENT_TYPE(WindowResize);

    constexpr const Size& get_size() const noexcept { return m_size; }

private:
    Size m_size;
};

class EventDispatcher
{
public:
    EventDispatcher(Event& e) : m_event(e) {}

    template <typename EventTp, typename FuncTp>
    void dispatch(const FuncTp& f)
    {
        if (EventTp::get_static_type() == m_event.get_type())
            f(static_cast<EventTp&>(m_event));
    }

private:
    Event& m_event;
};

class KeyEvent : public Event
{
public:
    virtual ~KeyEvent() = default;

    SURREAL_DECLARE_EVENT_CATEGORIES(EventCategoryFlagBits::Keyboard);

    constexpr u32 get_key() const noexcept { return m_key; }

    std::string to_string() const noexcept { return fmt::format("{}: {}", get_name(), m_key); }

protected:
    KeyEvent(u32 key) : m_key(key) {}

private:
    u32 m_key;
};

class KeyPressEvent final : public KeyEvent
{
public:
    KeyPressEvent(u32 key) : KeyEvent(key) {}

    SURREAL_DECLARE_EVENT_TYPE(KeyPress);
};

class KeyReleaseEvent final : public KeyEvent
{
public:
    KeyReleaseEvent(u32 key) : KeyEvent(key) {}

    SURREAL_DECLARE_EVENT_TYPE(KeyRelease);
};

class EventHandler
{
public:
    virtual ~EventHandler() = default;

    virtual void operator()(KeyEvent&) = 0;
    virtual void operator()(WindowEvent&) = 0;

    // virtual void process_key_event(KeyEvent&) = 0;
    // virtual void process_window_event(WindowEvent&) = 0;

protected:
    EventHandler() = default;
};

} // namespace Surreal
