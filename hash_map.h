#ifndef HASH_TABLE_
#define HASH_TABLE_

#include <vector>
#include <list>
#include <iostream>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    using object_type = std::pair<const KeyType, ValueType>;
    using iterator = 
        typename std::list<object_type>::iterator;
    using const_iterator = 
        typename std::list<object_type>::const_iterator;

private:
    const size_t kMinSize = 15;
    const size_t kMult = 2;
    std::vector<std::list<
        typename std::list<std::pair<const KeyType, ValueType>>::iterator>> values_;
    std::list<std::pair<const KeyType, ValueType>> all_;
    Hash hasher_;
    size_t elem_num_;
    size_t mod_;

    void rehash() {
        mod_ = elem_num_ * kMult;
        values_.clear();
        values_.resize(mod_);
        for (auto it = all_.begin(); it != all_.end(); ++it) {
            values_[hasher_(it->first) % mod_].push_back(it);
        }
    }

public:
    HashMap(const Hash custom_hasher = Hash()) : 
        hasher_(custom_hasher),
        elem_num_(0),
        mod_(kMinSize) {
        all_.clear();
        values_.resize(kMinSize);
    }

    template<class IteratorType>
    HashMap(IteratorType begin, IteratorType end, 
        const Hash custom_hasher = Hash()) : 
        hasher_(custom_hasher),
        elem_num_(std::distance(begin, end)),
        mod_(std::max(elem_num_ * kMult, kMinSize)) {
        all_.clear();
        values_.resize(mod_);

        for (auto it = begin; it != end; ++it) {
            all_.push_front((*it));
            values_[hasher_(it->first) % mod_].push_back(all_.begin());
        }
    }

    HashMap(std::initializer_list<object_type> l, 
        const Hash custom_hasher = Hash()) : 
        HashMap(l.begin(), l.end(), custom_hasher) {
    }

    HashMap(const HashMap& other) {
        *this = other;
    }

    size_t size() const {
        return elem_num_;
    }

    bool empty() const {
        return elem_num_ == 0;
    }

    Hash hash_function() const {
        return hasher_;
    }

    iterator insert(object_type value) {
        auto elem_it = find(value.first);
        if (elem_it != end())
            return elem_it;

        size_t idx = hasher_(value.first) % mod_;
        all_.push_front(value);
        values_[idx].push_back(all_.begin());
        ++elem_num_;

        if (elem_num_ == mod_) {
            rehash();
        }

        return begin();
    }

    void erase(KeyType key) {
        size_t idx = hasher_(key) % mod_;
        for (auto elem_it = values_[idx].begin(); 
            elem_it != values_[idx].end(); 
            ++elem_it) {
            if ((*elem_it)->first == key) {
                all_.erase(*elem_it);
                values_[idx].erase(elem_it);
                --elem_num_;
                return;
            }
        }
    }

    iterator begin() noexcept {
        return all_.begin();
    }

    iterator end() noexcept {
        return all_.end();
    }

    const_iterator begin() const noexcept {
        return all_.begin();
    }

    const_iterator end() const noexcept {
        return all_.end();
    }

    iterator find(const KeyType &key) {
        size_t idx = hasher_(key) % mod_;
        for (auto&& elem : values_[idx]) {
            if (elem->first == key) {
                return elem;
            }
        }
        return end();
    }

    const_iterator find(const KeyType &key) const {
        size_t idx = hasher_(key) % mod_;
        for (auto&& elem : values_[idx]) {
            if (elem->first == key) {
                return elem;
            }
        }
        return end();
    }

    ValueType& operator[](const KeyType key) {
        auto it = insert({key, ValueType()});
        return it->second;
    }

    const ValueType& at(const KeyType key) const {
        auto it = find(key);
        if (it == all_.end()) {
            throw std::out_of_range("No such element! :(");
        }
        return it->second;
    }

    void clear() {
        values_.clear();
        values_.resize(kMinSize);
        all_.clear();
        elem_num_ = 0;
        mod_ = kMinSize;
    }

    HashMap& operator=(const HashMap& other)
    {
        if (this != &other) {
            clear();
            for (auto x : other) {
                insert(x);
            }
        }
        return *this;
    }
};

#endif  // HASH_TABLE_

