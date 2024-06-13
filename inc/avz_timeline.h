#ifndef __AVZ_TIMELINE_H__
#define __AVZ_TIMELINE_H__

#include "avz_connector.h"
#include <variant>

#define __ANoDiscardTimeline [[nodiscard("ATimeline 需要绑定到绝对时间才会执行")]]

struct ATimeOffset {
    int wave = 0;
    int time = 0;

    constexpr ATimeOffset() = default;
    constexpr ATimeOffset(int time)
        : time(time)
    {
    }
    constexpr ATimeOffset(int wave, int time)
        : wave(wave)
        , time(time)
    {
    }

    constexpr ATimeOffset operator+() const
    {
        return *this;
    }

    constexpr friend ATimeOffset operator+(ATimeOffset lhs, ATimeOffset rhs)
    {
        return {lhs.wave + rhs.wave, lhs.time + rhs.time};
    }

    ATimeOffset& operator+=(ATimeOffset rhs)
    {
        return *this = *this + rhs;
    }

    constexpr ATimeOffset operator-() const
    {
        return {-wave, -time};
    }

    constexpr friend ATimeOffset operator-(ATimeOffset lhs, ATimeOffset rhs)
    {
        return {lhs.wave - rhs.wave, lhs.time - rhs.time};
    }

    ATimeOffset& operator-=(ATimeOffset rhs)
    {
        return *this = *this - rhs;
    }

    constexpr friend ATimeOffset operator*(ATimeOffset offset, int n)
    {
        return {offset.wave * n, offset.time * n};
    }

    constexpr friend ATimeOffset operator*(int n, ATimeOffset offset)
    {
        return {offset.wave * n, offset.time * n};
    }

    ATimeOffset& operator*=(int n)
    {
        return *this = *this * n;
    }

    auto operator<=>(const ATimeOffset&) const = default;
};

inline ATime operator+(ATime lhs, ATimeOffset rhs)
{
    return ATime(lhs.wave + rhs.wave, lhs.time + rhs.time);
}

inline ATime operator+(ATimeOffset lhs, ATime rhs)
{
    return ATime(lhs.wave + rhs.wave, lhs.time + rhs.time);
}

inline ATime& operator+=(ATime& lhs, ATimeOffset rhs)
{
    return lhs = lhs + rhs;
}

inline ATime operator-(ATime lhs, ATimeOffset rhs)
{
    return ATime(lhs.wave - rhs.wave, lhs.time - rhs.time);
}

inline ATime& operator-=(ATime& lhs, ATimeOffset rhs)
{
    return lhs = lhs - rhs;
}

namespace ALiterals {
constexpr ATimeOffset prev_wave {-1, 0};
constexpr ATimeOffset next_wave {1, 0};

constexpr ATimeOffset operator""_cs(unsigned long long x)
{
    return x;
}
}; // namespace ALiterals

template <typename T>
concept __AIsTimelineHook = std::is_convertible_v<T, std::function<void(ATime)>> && std::is_same_v<std::invoke_result_t<T, ATime>, void>;

class __ANoDiscardTimeline ATimeline {
protected:
    using TimelineHook = std::function<void(ATime)>;

    struct Entry {
        ATimeOffset offset;
        std::variant<AOperation, TimelineHook> action;

        Entry(ATimeOffset offset, auto&& action)
            : offset(offset)
            , action(std::forward<decltype(action)>(action))
        {
        }
    };

    std::vector<Entry> _entries;

public:
    ATimeline()
    {
    }

    ATimeline(__AIsOperation auto&& action)
    {
        _entries.emplace_back(0, std::forward<decltype(action)>(action));
    }

    ATimeline(__AIsCoroutineOp auto&& action)
    {
        _entries.emplace_back(0, ACoFunctor(std::forward<decltype(action)>(action)));
    }

    ATimeline(__AIsTimelineHook auto&& hook)
    {
        _entries.emplace_back(0, std::forward<decltype(hook)>(hook));
    }

    ATimeline(ATimeOffset offset, __AIsOperation auto&& action)
    {
        _entries.emplace_back(offset, std::forward<decltype(action)>(action));
    }

    ATimeline(ATimeOffset offset, __AIsCoroutineOp auto&& action)
    {
        _entries.emplace_back(offset, ACoFunctor(std::forward<decltype(action)>(action)));
    }

    ATimeline(ATimeOffset offset, __AIsTimelineHook auto&& hook)
    {
        _entries.emplace_back(offset, std::forward<decltype(hook)>(hook));
    }

    ATimeline(ATimeOffset offset, const ATimeline& timeline)
    {
        _entries.reserve(timeline._entries.size());
        for (auto&& entry : timeline._entries) {
            _entries.emplace_back(offset + entry.offset, entry.action);
        }
    }

    ATimeline(ATimeOffset offset, ATimeline&& timeline)
    {
        _entries.reserve(timeline._entries.size());
        for (auto&& entry : timeline._entries) {
            _entries.emplace_back(offset + entry.offset, std::move(entry.action));
        }
    }

    ATimeline(std::initializer_list<ATimeline> timelines)
    {
        size_t size = 0;
        for (auto&& timeline : timelines) {
            size += timeline._entries.size();
        }
        _entries.reserve(size);
        for (auto&& timeline : timelines) {
            _entries.insert(_entries.end(), timeline._entries.begin(), timeline._entries.end());
        }
    }

    ATimeline& operator&=(const ATimeline& rhs)
    {
        _entries.reserve(_entries.size() + rhs._entries.size());
        _entries.insert(_entries.end(), rhs._entries.begin(), rhs._entries.end());
        return *this;
    }

    __ADeprecated("请使用 & 运算符合并 ATimeline")
    ATimeline& operator+=(const ATimeline& rhs)
    {
        return *this &= rhs;
    }

    ATimeline Offset(ATimeOffset offset) const
    {
        return ATimeline(offset, *this);
    }

    ATimeline Offset(int wave, int time) const
    {
        return ATimeline(ATimeOffset(wave, time), *this);
    }

    ATimeline& operator+=(ATimeOffset offset)
    {
        for (auto&& entry : _entries) {
            entry.offset += offset;
        }
        return *this;
    }

    ATimeline& operator-=(ATimeOffset offset)
    {
        for (auto&& entry : _entries) {
            entry.offset -= offset;
        }
        return *this;
    }

    friend std::vector<ATimeConnectHandle> AConnect(const ATime& time, const ATimeline& timeline)
    {
        std::vector<ATimeConnectHandle> handles;
        for (auto&& entry : timeline._entries) {
            ATime entryTime = time + entry.offset;
            if (auto action = std::get_if<AOperation>(&entry.action)) {
                handles.push_back(AConnect(entryTime, *action));
            } else if (auto hook = std::get_if<TimelineHook>(&entry.action)) {
                std::invoke(*hook, entryTime);
            }
        }
        return handles;
    }

    friend AConnectHandle AConnect(AKey key, const ATimeline& timeline)
    {
        return AConnect(key, [=] {
            AConnect(ANowTime(), timeline);
        });
    }
};

inline ATimeline operator&(const ATimeline& lhs, const ATimeline& rhs)
{
    return {lhs, rhs};
}

__ADeprecated("请使用 & 运算符合并 ATimeline")
inline ATimeline operator+(const ATimeline& lhs, const ATimeline& rhs)
{
    return {lhs, rhs};
}

inline ATimeline operator+(const ATimeline& timeline, ATimeOffset offset)
{
    return timeline.Offset(offset);
}

inline ATimeline operator+(ATimeOffset offset, const ATimeline& timeline)
{
    return timeline.Offset(offset);
}

inline ATimeline operator-(const ATimeline& timeline, ATimeOffset offset)
{
    return timeline.Offset(-offset);
}

using ARelOp __ADeprecated("请使用 ATimeline") = ATimeline;

#endif
