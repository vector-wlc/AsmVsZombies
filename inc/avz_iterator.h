#ifndef __AVZ_ITERATOR_H__
#define __AVZ_ITERATOR_H__

#include "avz_global.h"
#include "avz_memory.h"

template <typename T>
using __APredicateT = std::function<bool(T* ptr)>;

template <class T>
struct __AFilterTrait;

template <>
struct __AFilterTrait<APlant> {
    __ANodiscard static APlant* GetBegin() {
        return AGetMainObject()->PlantArray();
    }

    __ANodiscard static APlant* GetEnd() {
        return AGetMainObject()->PlantArray() + AGetMainObject()->PlantTotal();
    }

    __ANodiscard static bool IsAlive(APlant* ptr) {
        return !ptr->IsDisappeared() && !ptr->IsCrushed() && ptr->BungeeState() != 2 && (ptr->Type() != ASQUASH || ptr->State() != 7);
    }
};

template <>
struct __AFilterTrait<AZombie> {
    __ANodiscard static AZombie* GetBegin() {
        return AGetMainObject()->ZombieArray();
    }

    __ANodiscard static AZombie* GetEnd() {
        return AGetMainObject()->ZombieArray() + AGetMainObject()->ZombieTotal();
    }

    __ANodiscard static bool IsAlive(AZombie* ptr) {
        return !ptr->IsDisappeared() && !ptr->IsDead() && ptr->AtWave() >= 0;
    }
};

template <>
struct __AFilterTrait<AItem> {
    __ANodiscard static AItem* GetBegin() {
        return AGetMainObject()->ItemArray();
    }

    __ANodiscard static AItem* GetEnd() {
        return AGetMainObject()->ItemArray() + AGetMainObject()->ItemTotal();
    }

    __ANodiscard static bool IsAlive(AItem* ptr) {
        return !ptr->IsDisappeared() && !ptr->IsCollected();
    }
};

template <>
struct __AFilterTrait<ASeed> {
    __ANodiscard static ASeed* GetBegin() {
        return AGetMainObject()->SeedArray();
    }

    __ANodiscard static ASeed* GetEnd() {
        return AGetMainObject()->SeedArray() + AGetMainObject()->SeedArray()->Count();
    }

    __ANodiscard static bool IsAlive(ASeed* ptr) {
        return AIsSeedUsable(ptr);
    }
};

template <>
struct __AFilterTrait<APlaceItem> {
    __ANodiscard static APlaceItem* GetBegin() {
        return AGetMainObject()->PlaceItemArray();
    }

    __ANodiscard static APlaceItem* GetEnd() {
        return AGetMainObject()->PlaceItemArray() + AGetMainObject()->PlaceItemTotal();
    }

    __ANodiscard static bool IsAlive(APlaceItem* ptr) {
        return !ptr->IsDisappeared();
    }
};

template <>
struct __AFilterTrait<AProjectile> {
    __ANodiscard static AProjectile* GetBegin() {
        return AGetMainObject()->ProjectileArray();
    }

    __ANodiscard static AProjectile* GetEnd() {
        return AGetMainObject()->ProjectileArray() + AGetMainObject()->ProjectileTotal();
    }

    __ANodiscard static bool IsAlive(AProjectile* ptr) {
        return !ptr->IsDisappeared();
    }
};

template <typename T>
class AFilterIterator {
protected:
    T* _cur;
    __APredicateT<T> _pred;
    T* _end;

    void forward() {
        ++_cur;
        while (_cur != _end && !_pred(_cur))
            ++_cur;
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = value_type&;

    AFilterIterator() {}

    AFilterIterator(pointer ptr, __APredicateT<T>&& func)
        : _cur(ptr)
        , _pred(std::move(func))
        , _end(__AFilterTrait<T>::GetEnd()) {
        while (_cur != _end && !_pred(_cur))
            ++_cur;
    }

    AFilterIterator(pointer ptr, const __APredicateT<T>& func)
        : _cur(ptr)
        , _pred(func)
        , _end(__AFilterTrait<T>::GetEnd()) {
        while (_cur != _end && !_pred(_cur))
            ++_cur;
    }

    __ANodiscard pointer toPtr() const {
        return _cur;
    }

    reference operator*() {
        return *(value_type*)(_cur);
    }

    reference operator*() const {
        return *(value_type*)(_cur);
    }

    pointer operator->() {
        return _cur;
    }

    const pointer operator->() const {
        return _cur;
    }

    AFilterIterator<T>& operator++() {
        forward();
        return *this;
    }

    AFilterIterator<T> operator++(int) {
        auto tmp = *this;
        forward();
        return tmp;
    }

    __ANodiscard bool operator==(const AFilterIterator<T>& rhs) const {
        return _cur == rhs._cur;
    }

    __ANodiscard bool operator!=(const AFilterIterator<T>& rhs) const {
        return _cur != rhs._cur;
    }
};

template <typename T>
class ABasicFilter {
protected:
    __APredicateT<T> _pred;

public:
    using Iterator = AFilterIterator<T>;
    static_assert(std::forward_iterator<Iterator>);

    ABasicFilter()
        : _pred(AAlwaysTrue<T*>()) {
    }

    ABasicFilter(__APredicateT<T>&& func)
        : _pred(std::move(func)) {
    }

    ABasicFilter(const __APredicateT<T>& func)
        : _pred(func) {
    }

    virtual void SetPredicate(__APredicateT<T>&& func) {
        this->_pred = std::move(func);
    }

    virtual void SetPredicate(const __APredicateT<T>& func) {
        this->_pred = func;
    }

    __ANodiscard Iterator begin() {
        return Iterator(__AFilterTrait<T>::GetBegin(), this->_pred);
    }

    __ANodiscard Iterator end() {
        return Iterator(__AFilterTrait<T>::GetEnd(), this->_pred);
    }

    __ANodiscard bool Empty() {
        return begin() == end();
    }

    __ANodiscard std::size_t Count() {
        std::size_t count = 0;
        for (auto& _ : *this) {
            (void)_; // suppress "unused variable" warning
            ++count;
        }
        return count;
    }
};

template <typename T>
class AAliveFilter : public ABasicFilter<T> {
public:
    AAliveFilter() {
        this->_pred = __AFilterTrait<T>::IsAlive;
    }

    AAliveFilter(__APredicateT<T>&& func) {
        SetPredicate(std::move(func));
    }

    AAliveFilter(const __APredicateT<T>& func) {
        SetPredicate(func);
    }

    virtual void SetPredicate(__APredicateT<T>&& func) override {
        this->_pred = [func = std::move(func)](T* ptr) -> bool {
            return __AFilterTrait<T>::IsAlive(ptr) && func(ptr);
        };
    }

    virtual void SetPredicate(const __APredicateT<T>& func) override {
        this->_pred = [func](T* ptr) -> bool {
            return __AFilterTrait<T>::IsAlive(ptr) && func(ptr);
        };
    }
};

template <typename T, typename Lhs, typename Rhs>
struct __AObjSelectorArgPack {
    Lhs (T::*prop)();
    std::function<bool(Lhs, Rhs)> pred;
    Rhs value;

    template <typename Pred>
    __AObjSelectorArgPack(Lhs (T::*prop)(), Pred pred, Rhs value)
        requires std::is_invocable_r_v<bool, Pred, Lhs, Rhs>
        : prop(prop)
        , pred(pred)
        , value(value) {}

    __AObjSelectorArgPack(Lhs (T::*prop)(), Rhs value)
        : prop(prop)
        , pred(std::equal_to())
        , value(value) {}

    bool operator()(T* obj) {
        return std::invoke(pred, std::invoke(prop, obj), value);
    }
};

template <typename T>
std::vector<std::function<bool(T*)>> __AObjSelectorCollectArgs(auto (T::*prop)(), auto op, auto value) {
    return {__AObjSelectorArgPack(prop, op, value)};
}

template <typename T>
std::vector<std::function<bool(T*)>> __AObjSelectorCollectArgs(auto (T::*prop)(), auto value) {
    return {__AObjSelectorArgPack(prop, value)};
}

template <typename T>
std::vector<std::function<bool(T*)>> __AObjSelectorCollectArgs(auto (T::*prop)(), auto op, auto value, auto... args) {
    if constexpr (std::is_member_pointer_v<decltype(value)>) {
        auto ret = __AObjSelectorCollectArgs<T>(value, args...);
        ret.push_back(__AObjSelectorArgPack(prop, op));
        return ret;
    } else {
        auto ret = __AObjSelectorCollectArgs<T>(args...);
        ret.push_back(__AObjSelectorArgPack(prop, op, value));
        return ret;
    }
}

// 简易的 PvZ 对象选择器，返回对应类型的 AAliveFilter
// 第一个参数为 &[对象类型]::[属性名]，后续参数为比较函数（可选）和目标值
// 可以传入多组比较条件，满足所有条件的对象才会被选择
// *** 使用示例：
// AObjSelector(&APlant::Type, ACOB_CANNON) // 选择所有玉米加农炮
// AObjSelector(&AZombie::Type, AGIGA_GARGANTUAR, &AZombie::AtWave, less(), ANowWave() - 1) // 选择非本波红眼
template <typename T>
AAliveFilter<T> AObjSelector(auto (T::*prop)(), auto... args) {
    return AAliveFilter<T>([preds = __AObjSelectorCollectArgs(prop, args...)](T* x) {
        for (auto& pred : preds)
            if (!pred(x))
                return false;
        return true;
    });
}

#endif
