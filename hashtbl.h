// "Copyright[2020] <divanik>"
#include <initializer_list>

#include <algorithm>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 private:
    std::vector<std::pair<KeyType, ValueType>> value_store;
    std::vector<std::vector<size_t>> hashed_pointers;
    Hash hasher;
    void reallocate() {
        if (3 * value_store.size() >= hashed_pointers.size() * 2) {
            size_t new_size = hashed_pointers.size() * 2;
            hashed_pointers.clear();
            hashed_pointers.resize(new_size, std::vector<size_t>(0));
            for (size_t i = 0; i < value_store.size(); i++) {
                int cur_position = hasher(value_store[i].first) % new_size;
                hashed_pointers[cur_position].push_back(i);
            }
        }
        return;
    }

 public:
    class iterator {
    public:
        size_t index;
        HashMap* my_hashmap = nullptr;
        iterator(size_t index_ = 0, HashMap* my_hashmap_ = nullptr) :
            index(index_), my_hashmap(my_hashmap_) {}
        iterator& operator++() {
            index++;
            return (*this);
        }
        iterator operator++(int) {
            iterator copied = (*this);
            index++;
            return copied;
        }
        iterator& operator--() {
            index--;
            return (*this);
        }
        iterator operator--(int) {
            iterator copied = (*this);
            index--;
            return copied;
        }
        bool operator ==(const iterator& other) const {
            return ((index == other.index) && (my_hashmap == other.my_hashmap));
        }
        bool operator !=(const iterator& other) const {
            return !(*this == other);
        }
        std::pair<const KeyType, ValueType>& operator*() const {
            return reinterpret_cast<std::pair<const KeyType, ValueType>&>
                                        (my_hashmap->value_store[index]);
        }
        std::pair<const KeyType, ValueType>* operator->() const {
            return reinterpret_cast<std::pair<const KeyType, ValueType>*>
                                        (&my_hashmap->value_store[index]);
        }
        void operator=(const iterator& other) {
            index = other.index;
            my_hashmap = other.my_hashmap;
        }
    };

    class const_iterator {
    public:
        size_t index;
        const HashMap* my_hashmap = nullptr;
        const_iterator(size_t index_ = 0,
                       const HashMap* my_hashmap_ = nullptr) :
            index(index_), my_hashmap(my_hashmap_) {}
        const_iterator& operator++() {
            index++;
            return (*this);
        }
        const_iterator operator++(int) {
            const_iterator copied = (*this);
            index++;
            return copied;
        }
        const_iterator& operator--() {
            index--;
            return (*this);
        }
        const_iterator operator--(int) {
            const_iterator copied = (*this);
            index--;
            return copied;
        }
        bool operator ==(const const_iterator& other) const {
            return ((index == other.index) && (my_hashmap == other.my_hashmap));
        }
        bool operator !=(const const_iterator& other) const {
            return !(*this == other);
        }
        const std::pair<const KeyType, ValueType>& operator*() const {
            return reinterpret_cast<const std::pair<const KeyType, ValueType>&>
                                            (my_hashmap->value_store[index]);
        }
        const std::pair<const KeyType, ValueType>* operator->() const {
            return reinterpret_cast<const std::pair<const KeyType, ValueType>*>
                                            (&my_hashmap->value_store[index]);
        }
        void operator=(const const_iterator& other) {
            index = other.index;
            my_hashmap = other.my_hashmap;
        }
    };

    HashMap(const Hash& hasher_ = Hash()) :
                        value_store(0),
                        hashed_pointers(1, std::vector<size_t>(0)),
                        hasher(hasher_) {}

    template<class Forward_Iter>
    HashMap(Forward_Iter first, Forward_Iter last,
                    const Hash& hasher_ = Hash()) :
                        value_store(0),
                        hashed_pointers(1, std::vector<size_t>(0)),
                        hasher(hasher_) {
        for (; first != last; first++) {
            insert(*first);
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>>
                        init_list, const Hash& hasher_ = Hash()) :
                                value_store(0),
                                hashed_pointers(1, std::vector<size_t>(0)),
                                hasher(hasher_)  {
        for (auto i : init_list) {
            insert(i);
        }
    }

    const size_t size() const {
        return value_store.size();
    }

    const bool empty() const {
        return value_store.size() == 0;
    }

    const Hash hash_function() const {
        return hasher;
    }

    iterator begin() {
        return {0, this};
    }

    const_iterator begin() const {
        return {0, this};
    }

    iterator end() {
        return {value_store.size(), this};
    }

    const_iterator end() const {
        return {value_store.size(), this};
    }

    iterator find(const KeyType& key) {
        size_t current_hash = hasher(key) % hashed_pointers.size();
        for (auto& x : hashed_pointers[current_hash]) {
            if (value_store[x].first == key) {
                return {x, this};
            }
        }
        return {value_store.size(), this};
    }

    const_iterator find(const KeyType& key) const {
        size_t current_hash = hasher(key) % hashed_pointers.size();
        for (auto& x : hashed_pointers[current_hash]) {
            if (value_store[x].first == key) {
                return { x, this };
            }
        }
        return { value_store.size(), this };
    }

    void erase(const KeyType& key) {
        if (find(key) == end()) {
            return;
        } else {
            auto curiter = find(key);
            size_t hash0 = hasher(key) % hashed_pointers.size();
            size_t hash1 =
                    hasher(value_store.back().first) % hashed_pointers.size();
            size_t index0 = curiter.index;
            size_t index1 = value_store.size() - 1;
            swap(value_store[index0], value_store.back());
            value_store.pop_back();
            for (auto& x : hashed_pointers[hash0]) {
                if (x == index0) {
                    std::swap(x, hashed_pointers[hash0].back());
                    hashed_pointers[hash0].pop_back();
                    break;
                }
            }
            for (auto& x : hashed_pointers[hash1]) {
                if (x == index1) {
                    x = index0;
                    break;
                }
            }
        }
    }

    void insert(const std::pair<KeyType, ValueType>& key_value) {
        if (find(key_value.first) == end()) {
            value_store.push_back(key_value);
            size_t current_hash = hasher(key_value.first);
            hashed_pointers[current_hash % hashed_pointers.size()].
                push_back(value_store.size() - 1);
            reallocate();
        }
        return;
    }

    const ValueType& at(const KeyType& key) const {
        if (find(key) == end()) {
            throw std::out_of_range("This element doesn't exist");
        } else {
            return (*(find(key))).second;
        }
    }

    ValueType& operator[](const KeyType& key) {
        if (find(key) == end()) {
            ValueType init = ValueType();
            insert({ key, init });
            return (*find(key)).second;
        } else {
            return (*find(key)).second;
        }
    }
    void clear() {
        value_store.clear();
        hashed_pointers.clear();
        hashed_pointers.resize(1);
    }
};

// int main(){}

