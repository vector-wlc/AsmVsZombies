#ifndef __AVZ_ITERATOR_H__
#define __AVZ_ITERATOR_H__

#include <iterator>

#include "avz_global.h"

template <typename T>
using __APredicateT = std::function<bool(T* ptr)>;

template <class T>
struct __AFilterTrait;

template <>
struct __AFilterTrait<APlant> {
    __ANodiscard static APlant* GetEnd()
    {
        return __aInternalGlobal.mainObject->PlantArray() + //
            __aInternalGlobal.mainObject->PlantTotal();
    }
    __ANodiscard static APlant* GetBegin()
    {
        return __aInternalGlobal.mainObject->PlantArray();
    }
    __ANodiscard static __APredicateT<APlant> GetAlivePredicate()
    {
        return [](APlant* ptr) -> bool { return !ptr->IsDisappeared() && !ptr->IsCrushed(); };
    }
};

template <>
struct __AFilterTrait<AZombie> {
    __ANodiscard static AZombie* GetEnd()
    {
        return __aInternalGlobal.mainObject->ZombieArray() + //
            __aInternalGlobal.mainObject->ZombieTotal();
    }
    __ANodiscard static AZombie* GetBegin()
    {
        return __aInternalGlobal.mainObject->ZombieArray();
    }
    __ANodiscard static __APredicateT<AZombie> GetAlivePredicate()
    {
        return [](AZombie* ptr) -> bool { return !ptr->IsDisappeared() && !ptr->IsDead(); };
    }
};

template <>
struct __AFilterTrait<AItem> {
    __ANodiscard static AItem* GetEnd()
    {
        return __aInternalGlobal.mainObject->ItemArray() + //
            __aInternalGlobal.mainObject->ItemTotal();
    }
    __ANodiscard static AItem* GetBegin()
    {
        return __aInternalGlobal.mainObject->ItemArray();
    }
    __ANodiscard static __APredicateT<AItem> GetAlivePredicate()
    {
        return [](AItem* ptr) -> bool { return !ptr->IsDisappeared() && !ptr->IsCollected(); };
    }
};

template <>
struct __AFilterTrait<ASeed> {
    __ANodiscard static ASeed* GetEnd()
    {
        return __aInternalGlobal.mainObject->SeedArray() + //
            __aInternalGlobal.mainObject->SeedArray()->Count();
    }
    __ANodiscard static ASeed* GetBegin()
    {
        return __aInternalGlobal.mainObject->SeedArray();
    }
    __ANodiscard static __APredicateT<ASeed> GetAlivePredicate()
    {
        return [](ASeed* ptr) -> bool { return ptr->IsUsable(); };
    }
};

template <>
struct __AFilterTrait<APlaceItem> {
    __ANodiscard static APlaceItem* GetEnd()
    {
        return __aInternalGlobal.mainObject->PlaceItemArray() + //
            __aInternalGlobal.mainObject->PlaceItemTotal();
    }
    __ANodiscard static APlaceItem* GetBegin()
    {
        return __aInternalGlobal.mainObject->PlaceItemArray();
    }
    __ANodiscard static __APredicateT<APlaceItem> GetAlivePredicate()
    {
        return [](APlaceItem* ptr) -> bool { return !ptr->IsDisappeared(); };
    }
};

template <typename T>
class AFilterIterator {
protected:
    T* _cur;
    __APredicateT<T> _pred;
    T* _end;

    void forward()
    {
        ++_cur;
        while (_cur != _end && !_pred(_cur)) {
            ++_cur;
        }
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = void;
    using pointer = T*;
    using reference = value_type&;

    AFilterIterator(pointer ptr, __APredicateT<T>&& func)
        : _cur(ptr)
        , _pred(std::move(func))
        , _end(__AFilterTrait<T>::GetEnd())
    {
        while (_cur != _end && !_pred(_cur)) {
            ++_cur;
        }
    }

    AFilterIterator(pointer ptr, const __APredicateT<T>& func)
        : _cur(ptr)
        , _pred(func)
        , _end(__AFilterTrait<T>::GetEnd())
    {
        while (_cur != _end && !_pred(_cur)) {
            ++_cur;
        }
    }

    __ANodiscard pointer toPtr() const
    {
        return _cur;
    }

    reference operator*()
    {
        return *(value_type*)(_cur);
    }

    reference operator*() const
    {
        return *(value_type*)(_cur);
    }

    pointer operator->()
    {
        return _cur;
    }

    const pointer operator->() const
    {
        return _cur;
    }

    AFilterIterator<T>& operator++()
    {
        forward();
        return *this;
    }

    AFilterIterator<T> operator++(int)
    {
        auto tmp = *this;
        forward();
        return tmp;
    }

    __ANodiscard bool operator==(const AFilterIterator<T>& rhs) const
    {
        return _cur == rhs._cur;
    }

    __ANodiscard bool operator!=(const AFilterIterator<T>& rhs) const
    {
        return _cur != rhs._cur;
    }
};

template <typename T>
class ABasicFilter {
protected:
    __APredicateT<T> _pred;

public:
    using Iterator = AFilterIterator<T>;

    ABasicFilter()
        : _pred([](T*) { return true; })
    {
    }

    ABasicFilter(__APredicateT<T>&& func)
        : _pred(std::move(func))
    {
    }

    ABasicFilter(const __APredicateT<T>& func)
        : _pred(func)
    {
    }

    virtual void SetPredicate(__APredicateT<T>&& func)
    {
        this->_pred = std::move(func);
    }

    virtual void SetPredicate(const __APredicateT<T>& func)
    {
        this->_pred = func;
    }

    __ANodiscard Iterator begin()
    {
        return Iterator(__AFilterTrait<T>::GetBegin(), this->_pred);
    }

    __ANodiscard Iterator end()
    {
        return Iterator(__AFilterTrait<T>::GetEnd(), this->_pred);
    }
};

template <typename T>
class AAliveFilter : public ABasicFilter<T> {
protected:
    __APredicateT<T> _alivePred;

public:
    AAliveFilter()
        : _alivePred(__AFilterTrait<T>::GetAlivePredicate())
    {
        this->_pred = this->_alivePred;
    }

    AAliveFilter(__APredicateT<T>&& func)
        : _alivePred(__AFilterTrait<T>::GetAlivePredicate())
    {
        SetPredicate(std::move(func));
    }

    AAliveFilter(const __APredicateT<T>& func)
        : _alivePred(__AFilterTrait<T>::GetAlivePredicate())
    {
        SetPredicate(func);
    }

    virtual void SetPredicate(__APredicateT<T>&& func) override
    {
        this->_pred = [this, func = std::move(func)](T* ptr) -> bool {
            return this->_alivePred(ptr) && func(ptr);
        };
    }

    virtual void SetPredicate(const __APredicateT<T>& func) override
    {
        this->_pred = [this, func](T* ptr) -> bool {
            return this->_alivePred(ptr) && func(ptr);
        };
    }
};

#endif
