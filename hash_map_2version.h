// "Copyright[2020] <divanik>"
#include <initializer_list>

#include <algorithm>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

/* This class implements algorithm that is known as Hash Table.
I used implementation with separate chaining using linked lists (actually, std::vector).
Table doubles its size when the number of elements becomes more than 2/3 of hash table capacity
You can read about it more using following link: https://en.wikipedia.org/wiki/Hash_table
*/

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 public:
    // This method creates empty hash table. Time: O(1).
    HashMap(const Hash& hasher = Hash()) :
                        value_store_(0),
                        hashed_pointers_(1, std::vector<size_t>(0)),
                        hasher_(hasher) {}

    /* This method creates hash table using elements between two given iterators. 
    Time: O(quantity of elements in table). */
    template<class Forward_Iter>
    HashMap(Forward_Iter first, Forward_Iter last,
                    const Hash& hasher = Hash()) :
                        value_store_(0),
                        hashed_pointers_(1, std::vector<size_t>(0)),
                        hasher_(hasher) {
        for (; first != last; first++) {
            insert(*first);
        }
    }

    /* This method creates hash table using elements in initializer list.
    Time: O(quantity of elements in table). */
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>>
                        init_list, const Hash& hasher = Hash()) :
                                value_store_(0),
                                hashed_pointers_(1, std::vector<size_t>(0)),
                                hasher_(hasher)  {
        for (auto elem : init_list) {
            insert(elem);
        }
    }

    /*This is forward iterator class. It helps to iterate in the data structure. 
    It is better to use iterators than pointers.*/
    class iterator {
        friend class HashMap;

     public:
        // Iterator constructor. Time: O(1).
        iterator(size_t index = 0, HashMap* my_hashmap = nullptr) :
            index_(index), my_hashmap_(my_hashmap) {}

        // Time: O(1).
        iterator& operator++() {
            index_++;
            return (*this);
        }

        // Time: O(1).
        iterator operator++(int) {
            iterator copied = (*this);
            index_++;
            return copied;
        }

        // Time: O(1).
        bool operator ==(const iterator& other) const {
            return ((index_ == other.index_)
                    && (my_hashmap_ == other.my_hashmap_));
        }

        // Time: O(1).
        bool operator !=(const iterator& other) const {
            return !(*this == other);
        }

        // Time: O(1).
        std::pair<const KeyType, ValueType>& operator*() const {
            return reinterpret_cast<std::pair<const KeyType, ValueType>&>
                                        (my_hashmap_->value_store_[index_]);
        }

        // Time: O(1).
        std::pair<const KeyType, ValueType>* operator->() const {
            return reinterpret_cast<std::pair<const KeyType, ValueType>*>
                                        (&my_hashmap_->value_store_[index_]);
        }

        // Time: O(1).
        void operator=(const iterator& other) {
            index_ = other.index_;
            my_hashmap_ = other.my_hashmap_;
        }

        // Time: O(1)
        const size_t index() const {
            return index_;
        }

     private:
        size_t index_;
        HashMap* my_hashmap_ = nullptr;
    };

    /* This is constant forward iterator class. It helps to iterate 
    in the data structure. Also it is good alternative to constant pointers. */
    class const_iterator {
        friend class HashMap;

     public:
        // Constant iterator constructor. Time: O(1).
        const_iterator(size_t index = 0,
                       const HashMap* my_hashmap = nullptr) :
            index_(index), my_hashmap_(my_hashmap) {}

        // Time: O(1).
        const_iterator& operator++() {
            index_++;
            return (*this);
        }

        // Time: O(1).
        const_iterator operator++(int) {
            const_iterator copied = (*this);
            index_++;
            return copied;
        }

        // Time: O(1).
        bool operator ==(const const_iterator& other) const {
            return ((index_ == other.index_) &&
                (my_hashmap_ == other.my_hashmap_));
        }

        // Time: O(1).
        bool operator !=(const const_iterator& other) const {
            return !(*this == other);
        }

        // Time: O(1).
        const std::pair<const KeyType, ValueType>& operator*() const {
            return reinterpret_cast<const std::pair<const KeyType, ValueType>&>
                                        (my_hashmap_->value_store_[index_]);
        }

        // Time: O(1).
        const std::pair<const KeyType, ValueType>* operator->() const {
            return reinterpret_cast<const std::pair<const KeyType, ValueType>*>
                                         (&my_hashmap_->value_store_[index_]);
        }

        // Time: O(1).
        void operator =(const const_iterator& other) {
            index_ = other.index_;
            my_hashmap_ = other.my_hashmap_;
        }

        // Time: O(1)
        const size_t index() const {
            return index_;
        }

     private:
        size_t index_;
        const HashMap* my_hashmap_ = nullptr;
    };

    // Returns number of elements in hash table. Time: O(1).
    const size_t size() const {
        return value_store_.size();
    }

    /* Returns true if and only if there is no elements in hash table.
    Time: O(1). */
    const bool empty() const {
        return value_store_.size() == 0;
    }

    // Returns hash function, used by hash table. Time: O(1).
    const Hash hash_function() const {
        return hasher_;
    }

    // Iterator points to the first element in hash_table. Time: O(1).
    iterator begin() {
        return {0, this};
    }

    // Constant iterator points to the first element in hash_table. Time: O(1).
    const_iterator begin() const {
        return {0, this};
    }

    // Iterator points behind the last element in hash_table. Time: O(1).
    iterator end() {
        return {value_store_.size(), this};
    }

    /* Constant iterator points behind the last 
    element in hash_table. Time: O(1). */
    const_iterator end() const {
        return {value_store_.size(), this};
    }

    /* Returns iterator to the element if this element is in
     the table ot iterator end() otherwise. Time: randomized O(1). */
    iterator find(const KeyType& key) {
        size_t current_hash = hasher_(key) % hashed_pointers_.size();
        for (auto& x : hashed_pointers_[current_hash]) {
            if (value_store_[x].first == key) {
                return {x, this};
            }
        }
        return {value_store_.size(), this};
    }

    /* Returns constant iterator to the element if this element is in the 
    table ot constant iterator end() otherwise. Time: randomized O(1). */
    const_iterator find(const KeyType& key) const {
        size_t current_hash = hasher_(key) % hashed_pointers_.size();
        for (auto& x : hashed_pointers_[current_hash]) {
            if (value_store_[x].first == key) {
                return { x, this };
            }
        }
        return { value_store_.size(), this };
    }

    /* Removes element from the hash_table. The size of storage 
    doesn't change. Time: randomized O(1).*/
    void erase(const KeyType& key) {
        if (find(key) == end()) {
            return;
        } else {
            auto curiter = find(key);
            size_t hash0 = hasher_(key) % hashed_pointers_.size();
            size_t hash1 = hasher_(value_store_.back().first) %
                                    hashed_pointers_.size();
            size_t index0 = curiter.index_;
            size_t index1 = value_store_.size() - 1;
            swap(value_store_[index0], value_store_.back());
            value_store_.pop_back();
            for (auto& x : hashed_pointers_[hash0]) {
                if (x == index0) {
                    std::swap(x, hashed_pointers_[hash0].back());
                    hashed_pointers_[hash0].pop_back();
                    break;
                }
            }
            for (auto& x : hashed_pointers_[hash1]) {
                if (x == index1) {
                    x = index0;
                    break;
                }
            }
        }
    }

    /* Inserts element into the hash table only if there was not such element.
    Calls reallocate if necessary. Time: randomized and amortized O(1).
    O(quantity of elements in the table) while reallocation. */
    void insert(const std::pair<KeyType, ValueType>& key_value) {
        if (find(key_value.first) == end()) {
            value_store_.push_back(key_value);
            size_t current_hash = hasher_(key_value.first);
            hashed_pointers_[current_hash % hashed_pointers_.size()].
                push_back(value_store_.size() - 1);
            reallocate();
        }
        return;
    }

    /* Returns reference to the value if this key is in the table. 
    Throws out_of_range exception otherwise. Randomized O(1) */
    const ValueType& at(const KeyType& key) const {
        if (find(key) == end()) {
            throw std::out_of_range("This element doesn't exist");
        } else {
            return (*(find(key))).second;
        }
    }

    /* Returns reference to the value if this key is in the table.
    Otherwise put default value into hashtable. Randomized O(1) */
    ValueType& operator[](const KeyType& key) {
        if (find(key) == end()) {
            ValueType init = ValueType();
            insert({ key, init });
            return (*find(key)).second;
        } else {
            return (*find(key)).second;
        }
    }

    // Clears hash table. Time: O(quantity of elements in the table)
    void clear() {
        value_store_.clear();
        hashed_pointers_.clear();
        hashed_pointers_.resize(1);
    }

 private:
    std::vector<std::pair<KeyType, ValueType>> value_store_;
    std::vector<std::vector<size_t>> hashed_pointers_;
    Hash hasher_;
    constexpr static size_t INCREMENT_FACTOR_ = 2;
    constexpr static size_t REALLOCATION_FACTOR_ = 3;

    /* This method rebuilds table when quantity of elements becomes more 
     than INCREMENT_FACTOR_/REALLOCATION_FACTOR_ of hash table capacity. 
     It increments size of the table INCREMENT_FACTOR_ times. Time: O(quantity of elements in the table). */
    void reallocate() {
        if (REALLOCATION_FACTOR_ * value_store_.size() >= hashed_pointers_.size() * INCREMENT_FACTOR_) {
            size_t new_size = hashed_pointers_.size() * INCREMENT_FACTOR_;
            hashed_pointers_.clear();
            hashed_pointers_.resize(new_size, std::vector<size_t>(0));
            for (size_t i = 0; i < value_store_.size(); i++) {
                int cur_position = hasher_(value_store_[i].first) % new_size;
                hashed_pointers_[cur_position].push_back(i);
            }
        }
        return;
    }
};


