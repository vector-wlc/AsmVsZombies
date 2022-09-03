/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2022-06-10 11:54:47
 * @Description:
 */
#ifndef __AVZ_ITERATOR_H__
#define __AVZ_ITERATOR_H__

#include "avz_global.h"

namespace AvZ {

extern MainObject* __main_object;
template <typename T>
using __PredicateT = std::function<bool(SafePtr<T> ptr)>;

template <class T>
struct __filter_trait;

template <>
struct __filter_trait<Plant> {
    static SafePtr<Plant> __get_end()
    {
        return __main_object->plantArray() + __main_object->plantTotal();
    }
    static SafePtr<Plant> __get_begin()
    {
        return __main_object->plantArray();
    }
    static __PredicateT<Plant> __get_alive_predicate()
    {
        return [](SafePtr<Plant> ptr) -> bool { return !ptr->isDisappeared() && !ptr->isCrushed(); };
    }
};

template <>
struct __filter_trait<Zombie> {
    static SafePtr<Zombie> __get_end()
    {
        return __main_object->zombieArray() + __main_object->zombieTotal();
    }
    static SafePtr<Zombie> __get_begin()
    {
        return __main_object->zombieArray();
    }
    static __PredicateT<Zombie> __get_alive_predicate()
    {
        return [](SafePtr<Zombie> ptr) -> bool { return !ptr->isDisappeared() && !ptr->isDead(); };
    }
};

template <>
struct __filter_trait<Item> {
    static SafePtr<Item> __get_end()
    {
        return __main_object->itemArray() + __main_object->itemTotal();
    }
    static SafePtr<Item> __get_begin()
    {
        return __main_object->itemArray();
    }
    static __PredicateT<Item> __get_alive_predicate()
    {
        return [](SafePtr<Item> ptr) -> bool { return !ptr->isDisappeared() && !ptr->isCollected(); };
    }
};

template <>
struct __filter_trait<Seed> {
    static SafePtr<Seed> __get_end()
    {
        return __main_object->seedArray() + __main_object->seedArray()->count();
    }
    static SafePtr<Seed> __get_begin()
    {
        return __main_object->seedArray();
    }
    static __PredicateT<Seed> __get_alive_predicate()
    {
        return [](SafePtr<Seed> ptr) -> bool { return ptr->isUsable(); };
    }
};

template <typename T>
class FilterIterator : public std::iterator<std::forward_iterator_tag, SafePtr<T>> {
private:
    __PredicateT<T> __pred;
    SafePtr<T> __cur;
    SafePtr<T> __end;

    void forward()
    {
        ++__cur;
        while (__cur != __end && !__pred(__cur)) {
            ++__cur;
        }
    }

public:
    FilterIterator(SafePtr<T> ptr, __PredicateT<T>&& func)
        : __cur(ptr)
        , __pred(std::move(func))
        , __end(__filter_trait<T>::__get_end())
    {
        while (__cur != __end && !__pred(__cur)) {
            ++__cur;
        }
    }

    FilterIterator(SafePtr<T> ptr, const __PredicateT<T>& func)
        : __cur(ptr)
        , __pred(func)
        , __end(__filter_trait<T>::__get_end())
    {
        while (__cur != __end && !__pred(__cur)) {
            ++__cur;
        }
    }

    SafePtr<T> toPtr() const
    {
        return __cur;
    }

    T& operator*()
    {
        return *__cur;
    }

    SafePtr<T>& operator->()
    {
        return __cur;
    }

    FilterIterator<T>& operator++()
    {
        forward();
        return *this;
    }

    FilterIterator<T> operator++(int)
    {
        auto tmp = *this;
        forward();
        return tmp;
    }

    bool operator==(const FilterIterator<T>& rhs) const
    {
        return __cur.toUnsafe() == rhs.__cur.toUnsafe();
    }

    bool operator!=(const FilterIterator<T>& rhs) const
    {
        return __cur.toUnsafe() != rhs.__cur.toUnsafe();
    }
};

template <typename T>
class BasicFilter {
protected:
    __PredicateT<T> __pred;

public:
    using Iterator = FilterIterator<T>;

    BasicFilter()
        : __pred([](SafePtr<T>) { return true; })
    {
    }

    BasicFilter(__PredicateT<T>&& func)
        : __pred(std::move(func))
    {
    }

    BasicFilter(const __PredicateT<T>& func)
        : __pred(func)
    {
    }

    virtual void setPredicate(__PredicateT<T>&& func)
    {
        this->__pred = std::move(func);
    }

    virtual void setPredicate(const __PredicateT<T>& func)
    {
        this->__pred = func;
    }

    Iterator begin()
    {
        void* unsafe_ptr = __filter_trait<T>::__get_begin();
        return Iterator(SafePtr<T>((T*)(unsafe_ptr)), this->__pred);
    }

    Iterator end()
    {
        void* unsafe_ptr = __filter_trait<T>::__get_end();
        return Iterator(SafePtr<T>((T*)(unsafe_ptr)), this->__pred);
    }
};

template <typename T>
class AliveFilter : public BasicFilter<T> {
private:
    __PredicateT<T> __alive_pred;

public:
    AliveFilter()
        : __alive_pred(__filter_trait<T>::__get_alive_predicate())
    {
        this->__pred = this->__alive_pred;
    }

    AliveFilter(__PredicateT<T>&& func)
        : __alive_pred(__filter_trait<T>::__get_alive_predicate())
    {
        setPredicate(std::move(func));
    }

    AliveFilter(const __PredicateT<T>& func)
        : __alive_pred(__filter_trait<T>::__get_alive_predicate())
    {
        setPredicate(func);
    }

    virtual void setPredicate(__PredicateT<T>&& func) override
    {
        this->__pred = [this, func = std::move(func)](SafePtr<T> ptr) -> bool {
            return this->__alive_pred(ptr) && func(ptr);
        };
    }

    virtual void setPredicate(const __PredicateT<T>& func) override
    {
        this->__pred = [this, func](SafePtr<T> ptr) -> bool {
            return this->__alive_pred(ptr) && func(ptr);
        };
    }
};
}

#endif
