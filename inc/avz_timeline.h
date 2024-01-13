#ifndef __AVZ_TIMELINE_H__
#define __AVZ_TIMELINE_H__

#include "avz_coroutine.h"

#define __ANoDiscardTimeline [[nodiscard("ATimeline 需要绑定到时间才会执行")]]

class ATimeOffset {
public:
    int wave;
    int time;

    constexpr ATimeOffset()
        : wave(0)
        , time(0)
    {
    }
    constexpr ATimeOffset(int time_)
        : wave(0)
        , time(time_)
    {
    }
    constexpr ATimeOffset(int wave_, int time_)
        : wave(wave_)
        , time(time_)
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

    friend ATime operator+(ATime lhs, ATimeOffset rhs)
    {
        return ATime(lhs.wave + rhs.wave, lhs.time + rhs.time);
    }

    friend ATime operator+(ATimeOffset lhs, ATime rhs)
    {
        return ATime(lhs.wave + rhs.wave, lhs.time + rhs.time);
    }

    ATimeOffset& operator+=(ATimeOffset rhs)
    {
        return *this = *this + rhs;
    }

    friend ATime& operator+=(ATime& lhs, ATimeOffset rhs)
    {
        return lhs = lhs + rhs;
    }

    constexpr ATimeOffset operator-() const
    {
        return {-wave, -time};
    }

    constexpr friend ATimeOffset operator-(ATimeOffset lhs, ATimeOffset rhs)
    {
        return {lhs.wave - rhs.wave, lhs.time - rhs.time};
    }

    friend ATime operator-(ATime lhs, ATimeOffset rhs)
    {
        return ATime(lhs.wave - rhs.wave, lhs.time - rhs.time);
    }

    ATimeOffset& operator-=(ATimeOffset rhs)
    {
        return *this = *this - rhs;
    }

    friend ATime& operator-=(ATime& lhs, ATimeOffset rhs)
    {
        return lhs = lhs - rhs;
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
};

namespace ALiterals {
constexpr ATimeOffset prev_wave {-1, 0};
constexpr ATimeOffset next_wave {1, 0};

constexpr ATimeOffset operator""_cs(unsigned long long x)
{
    return x;
}
};

class __ANoDiscardTimeline ATimeline {
public:
    struct Entry {
        ATimeOffset offset;
        AOperation action;
    };

protected:
    std::vector<Entry> _entries;

public:
    const std::vector<Entry>& GetEntries() const
    {
        return _entries;
    }

    __ADeprecated("请使用 GetEntries()") const std::vector<Entry>& GetOpVec() const
    {
        return _entries;
    }

    ATimeline()
    {
    }

    template <class Action>
        requires __AIsOperation<Action>
    ATimeline(Action&& action)
    {
        _entries.emplace_back(Entry {0, std::forward<Action>(action)});
    }

    template <class Action>
        requires __AIsCoroutineOp<Action>
    ATimeline(Action&& action)
    {
        _entries.emplace_back(Entry {0, ACoFunctor(std::forward<Action>(action))});
    }

    template <class Action>
        requires __AIsOperation<Action>
    ATimeline(ATimeOffset offset, Action&& action)
    {
        _entries.emplace_back(Entry {offset, std::forward<Action>(action)});
    }

    template <class Action>
        requires __AIsCoroutineOp<Action>
    ATimeline(ATimeOffset offset, Action&& action)
    {
        _entries.emplace_back(Entry {offset, ACoFunctor(std::forward<Action>(action))});
    }

    ATimeline(ATimeOffset offset, const ATimeline& timeline)
    {
        _entries.reserve(timeline._entries.size());
        for (auto&& entry : timeline._entries) {
            _entries.emplace_back(Entry {offset + entry.offset, entry.action});
        }
    }

    ATimeline(ATimeOffset offset, ATimeline&& timeline)
    {
        _entries.reserve(timeline._entries.size());
        for (auto&& entry : timeline._entries) {
            _entries.emplace_back(Entry {offset + entry.offset, std::move(entry.action)});
        }
    }

    ATimeline(std::initializer_list<ATimeline> timelines)
    {
        for (auto&& timeline : timelines) {
            for (auto&& entry : timeline._entries) {
                _entries.emplace_back(entry);
            }
        }
    }

    ATimeline& operator+=(const ATimeline& rhs)
    {
        _entries.insert(_entries.end(), rhs._entries.begin(), rhs._entries.end());
        return *this;
    }

    friend ATimeline operator+(ATimeline lhs, const ATimeline& rhs)
    {
        return lhs += rhs;
    }

    ATimeline Offset(ATimeOffset offset) const
    {
        return ATimeline(offset, *this);
    }

    ATimeline Offset(int wave, int time) const
    {
        return ATimeline(ATimeOffset(wave, time), *this);
    }
};

using ARelOp __ADeprecated("请使用 ATimeline") = ATimeline;

#endif
