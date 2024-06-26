// Copyright (c) 2024 Reisen (https://github.com/alumkal)

#ifndef __DSL_MAIN_H__
#define __DSL_MAIN_H__

#include "avz.h"

class __ANoDiscardTimeline ASequence {
private:
    std::vector<ATimeline> _sequence;

public:
    explicit ASequence(const std::vector<ATimeline>& sequence)
        : _sequence(sequence)
    {
    }
    explicit ASequence(std::initializer_list<ATimeline> sequence)
        : _sequence(sequence)
    {
    }

    friend ASequence operator|(ASequence sequence, const ATimeline& timeline)
    {
        sequence._sequence.push_back(timeline);
        return sequence;
    }

    const ATimeline& operator[](size_t index) const
    {
        static ATimeline null;
        if (_sequence.empty())
            return null;
        return _sequence[index % _sequence.size()];
    }
};

inline ASequence operator|(const ATimeline& lhs, const ATimeline& rhs)
{
    return ASequence {lhs, rhs};
}

class AWave {
public:
    class Iterator {
        friend class AWave;

    private:
        std::vector<int>::const_iterator _it, _end;
        int _phase;

        void _Forward()
        {
            for (; _it != _end && *_it <= 0; ++_it)
                _phase = -*_it;
        }

        Iterator(std::vector<int>::const_iterator begin, std::vector<int>::const_iterator end)
            : _it(begin)
            , _end(end)
            , _phase(0)
        {
            _Forward();
        }

    public:
        using difference_type = decltype(_it)::difference_type;
        using value_type = std::pair<int, int>;

        Iterator() = default;

        std::pair<int, int> operator*() const
        {
            return {*_it, _phase};
        }

        Iterator& operator++()
        {
            ++_it, ++_phase;
            _Forward();
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator ret = *this;
            ++*this;
            return ret;
        }

        bool operator==(const Iterator& rhs) const
        {
            return _it == rhs._it;
        }
    };

    static_assert(std::forward_iterator<Iterator>);

private:
    std::vector<int> _waves;
    ATimeOffset _offset;

    void AppendWaves(int x)
    {
        _waves.push_back(x);
    }

    void AppendWaves(const std::vector<int>& x)
    {
        _waves.insert(_waves.end(), x.begin(), x.end());
    }

    void AppendWaves(const AWave& x)
    {
        AppendWaves(x._waves);
    }

public:
    explicit AWave(auto&&... args)
        requires (sizeof...(args) > 0)
    {
        (AppendWaves(std::forward<decltype(args)>(args)), ...);
    }

    AWave operator%(int phase) const
    {
        if (!_waves.empty() && _waves[0] <= 0) {
            AWave ret = *this;
            ret._waves[0] = 1 - phase;
            return ret;
        } else
            return AWave(1 - phase, *this);
    }

    friend AWave operator,(AWave lhs, const AWave& rhs)
    {
        lhs.AppendWaves(rhs);
        return lhs;
    }

    Iterator begin() const
    {
        return Iterator(_waves.begin(), _waves.end());
    }

    Iterator end() const
    {
        return Iterator(_waves.end(), _waves.end());
    }

    void operator<<=(const ATimeline& timeline) const
    {
        for (auto [wave, _] : *this)
            AConnect(ATime(wave, 0) + _offset, timeline);
    }

    void operator<<=(const ASequence& sequence) const
    {
        for (auto [wave, phase] : *this)
            AConnect(ATime(wave, 0) + _offset, sequence[phase]);
    }

    friend AWave operator+(AWave wave, ATimeOffset offset)
    {
        wave._offset += offset;
        return wave;
    }

    friend AWave operator+(ATimeOffset offset, AWave wave)
    {
        wave._offset += offset;
        return wave;
    }

    friend AWave operator-(AWave wave, ATimeOffset offset)
    {
        wave._offset -= offset;
        return wave;
    }
};

inline AWave operator%(const std::vector<int>& waves, int phase)
{
    return AWave(waves) % phase;
}

namespace ALiterals {
#define __MakeRange(out, first, last, step)   \
    std::vector<int> out;                     \
    for (int i = first; i <= last; i += step) \
        out.push_back(i);

#define __DeclareRangeWithStep(last, step)                                       \
    inline std::vector<int> operator""_##last##_step##step(unsigned long long x) \
    {                                                                            \
        __MakeRange(ret, x, last, step);                                         \
        return ret;                                                              \
    }                                                                            \
    inline AWave operator""_##last##_step##step##_wave(unsigned long long x)     \
    {                                                                            \
        __MakeRange(ret, x, last, step);                                         \
        return AWave(ret);                                                       \
    }

#define __DeclareRange(last)                                        \
    inline std::vector<int> operator""_##last(unsigned long long x) \
    {                                                               \
        __MakeRange(ret, x, last, 1);                               \
        return ret;                                                 \
    }                                                               \
    inline AWave operator""_##last##_wave(unsigned long long x)     \
    {                                                               \
        __MakeRange(ret, x, last, 1);                               \
        return AWave(ret);                                          \
    }                                                               \
    __DeclareRangeWithStep(last, 1);                                \
    __DeclareRangeWithStep(last, 2);                                \
    __DeclareRangeWithStep(last, 3);                                \
    __DeclareRangeWithStep(last, 4);                                \
    __DeclareRangeWithStep(last, 5);                                \
    __DeclareRangeWithStep(last, 6);                                \
    __DeclareRangeWithStep(last, 7);                                \
    __DeclareRangeWithStep(last, 8);                                \
    __DeclareRangeWithStep(last, 9);                                \
    __DeclareRangeWithStep(last, 10);                               \
    __DeclareRangeWithStep(last, 11);                               \
    __DeclareRangeWithStep(last, 12);                               \
    __DeclareRangeWithStep(last, 13);                               \
    __DeclareRangeWithStep(last, 14);                               \
    __DeclareRangeWithStep(last, 15);                               \
    __DeclareRangeWithStep(last, 16);                               \
    __DeclareRangeWithStep(last, 17);                               \
    __DeclareRangeWithStep(last, 18);                               \
    __DeclareRangeWithStep(last, 19);                               \
    __DeclareRangeWithStep(last, 20);                               \
    __DeclareRangeWithStep(last, 21);                               \
    __DeclareRangeWithStep(last, 22);                               \
    __DeclareRangeWithStep(last, 23);                               \
    __DeclareRangeWithStep(last, 24);                               \
    __DeclareRangeWithStep(last, 25);                               \
    __DeclareRangeWithStep(last, 26);                               \
    __DeclareRangeWithStep(last, 27);                               \
    __DeclareRangeWithStep(last, 28);                               \
    __DeclareRangeWithStep(last, 29);                               \
    __DeclareRangeWithStep(last, 30);                               \
    __DeclareRangeWithStep(last, 31);                               \
    __DeclareRangeWithStep(last, 32);                               \
    __DeclareRangeWithStep(last, 33);                               \
    __DeclareRangeWithStep(last, 34);                               \
    __DeclareRangeWithStep(last, 35);                               \
    __DeclareRangeWithStep(last, 36);                               \
    __DeclareRangeWithStep(last, 37);                               \
    __DeclareRangeWithStep(last, 38);                               \
    __DeclareRangeWithStep(last, 39);

inline AWave operator""_wave(unsigned long long x)
{
    return AWave(x);
}

__DeclareRange(1);
__DeclareRange(2);
__DeclareRange(3);
__DeclareRange(4);
__DeclareRange(5);
__DeclareRange(6);
__DeclareRange(7);
__DeclareRange(8);
__DeclareRange(9);
__DeclareRange(10);
__DeclareRange(11);
__DeclareRange(12);
__DeclareRange(13);
__DeclareRange(14);
__DeclareRange(15);
__DeclareRange(16);
__DeclareRange(17);
__DeclareRange(18);
__DeclareRange(19);
__DeclareRange(20);
__DeclareRange(21);
__DeclareRange(22);
__DeclareRange(23);
__DeclareRange(24);
__DeclareRange(25);
__DeclareRange(26);
__DeclareRange(27);
__DeclareRange(28);
__DeclareRange(29);
__DeclareRange(30);
__DeclareRange(31);
__DeclareRange(32);
__DeclareRange(33);
__DeclareRange(34);
__DeclareRange(35);
__DeclareRange(36);
__DeclareRange(37);
__DeclareRange(38);
__DeclareRange(39);
__DeclareRange(40);

#undef __MakeRange
#undef __DeclareRangeWithStep
#undef __DeclareRange
} // namespace ALiterals

inline ATimeline operator<<=(ATimeOffset offset, const ATimeline& timeline)
{
    return timeline + offset;
}

template <typename T>
    requires(!std::integral<T> || std::same_as<T, char>)
inline void operator<<=(const T& condition, const ATimeline& timeline)
{
    AConnect(condition, timeline);
}

class __ADSLCastHelper {
private:
    std::variant<ATimeOffset, ATimeline, ASequence> _value;

public:
    explicit __ADSLCastHelper(auto&& value)
        : _value(std::forward<decltype(value)>(value))
    {
    }
    explicit __ADSLCastHelper(std::initializer_list<ATimeline> timelines)
        : _value(ATimeline(timelines))
    {
    }

    operator ATimeline() const
    {
        if (auto timeline = std::get_if<ATimeline>(&_value))
            return *timeline;
        else if (auto sequence = std::get_if<ASequence>(&_value))
            aLogger->Error("ASequence 只能连接至 AWave 对象");
        return {};
    }

    friend void operator<<=(const AWave& wave, const __ADSLCastHelper& castHelper)
    {
        if (auto timeline = std::get_if<ATimeline>(&castHelper._value))
            wave <<= *timeline;
        else if (auto sequence = std::get_if<ASequence>(&castHelper._value))
            wave <<= *sequence;
    }

    ATimeline operator<<=(const ATimeline& timeline) const
    {
        if (auto offset = std::get_if<ATimeOffset>(&_value))
            return timeline + *offset;
        return {};
    }
};

#define At(...) (__VA_ARGS__) <<= (__ADSLCastHelper)
#define OnWave(...) At(AWave(__VA_ARGS__))
#define Do (ATimeline)[=]() mutable
#define CoDo (ATimeline)[=]() mutable->ACoroutine

#endif
