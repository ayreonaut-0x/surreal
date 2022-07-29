#pragma once

#if SURREAL_CXX_IS_CLANG || SURREAL_CXX_IS_GCC
    #ifdef __has_attribute
        #if __has_attribute(always_inline)
            #define SURREAL_ALWAYS_INLINE __inline__ __attribute__((always_inline))
        #else
            #define SURREAL_ALWAYS_INLINE inline
        #endif

        #if __has_attribute(noinline)
            #define SURREAL_NOINLINE __attribute__((noinline))
        #else
            #define SURREAL_NOINLINE
        #endif

        #if __has_attribute(__nonnull__)
            #define SURREAL_NONNULL(...) __attribute__((__nonnull__ __VA_OPT__(__VA_ARGS__)))
        #else
            #define SURREAL_NONNULL(...)
        #endif

        #if __has_attribute(noreturn)
            #define SURREAL_NORETURN __attribute__((noreturn))
        #else
            #define SURREAL_NORETURN
        #endif

    #endif

    #ifdef __has_cpp_attribute
        #if __has_cpp_attribute(maybe_unused)
            #define SURREAL_UNUSED(t, n) [[maybe_unused]] t n
        #else
            #define SURREAL_UNUSED(t, n) t
        #endif

        #if __has_cpp_attribute(nodiscard)
            #define SURREAL_NODISCARD [[nodiscard]]
        #else
            #define SURREAL_NODISCARD
        #endif

        #if __has_cpp_attribute(likely)
            #define SURREAL_LIKELY [[likely]]
        #else
            #define SURREAL_LIKELY
        #endif

        #if __has_cpp_attribute(unlikely)
            #define SURREAL_UNLIKELY [[unlikely]]
        #else
            #define SURREAL_UNLIKELY
        #endif

        #if __has_cpp_attribute(fallthrough)
            #define SURREAL_FALLTHROUGH [[fallthrough]]
        #else
            #define SURREAL_FALLTHROUGH
        #endif
    #endif
#elif SURREAL_CXX_IS_MSVC
#endif
