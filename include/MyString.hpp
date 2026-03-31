// Minimal implementation per assignment, headers restricted
#ifndef SIMPLE_STRING_SIMPLESTRING_HPP
#define SIMPLE_STRING_SIMPLESTRING_HPP

#include <stdexcept>
#include <cstring>

class MyString {
private:
    union {
        char* heap_ptr;
        char small_buffer[16];
    } storage{};
    size_t length_ = 0;
    size_t capacity_ = 15; // capacity excluding null terminator when using heap
    bool using_sso_ = true;

    void set_terminator() { data_ptr()[length_] = '\0'; }
    char* data_ptr() { return using_sso_ ? storage.small_buffer : storage.heap_ptr; }
    const char* data_ptr() const { return using_sso_ ? storage.small_buffer : storage.heap_ptr; }

    void allocate_heap(size_t cap) {
        // cap is capacity excluding NUL
        storage.heap_ptr = new char[cap + 1];
        capacity_ = cap;
        using_sso_ = false;
    }

    void ensure_capacity(size_t need) {
        // need is capacity excluding NUL required for length
        if (using_sso_) {
            if (need <= 15) return;
            // move from SSO to heap
            size_t new_cap = need;
            // grow policy: round up to 2x for some headroom
            new_cap = need * 2;
            char tmp[16];
            std::memcpy(tmp, storage.small_buffer, length_ + 1);
            allocate_heap(new_cap);
            std::memcpy(storage.heap_ptr, tmp, length_ + 1);
        } else {
            if (need <= capacity_) return;
            size_t new_cap = capacity_ ? capacity_ : 15;
            while (new_cap < need) new_cap = new_cap * 2;
            char* new_mem = new char[new_cap + 1];
            std::memcpy(new_mem, storage.heap_ptr, length_ + 1);
            delete[] storage.heap_ptr;
            storage.heap_ptr = new_mem;
            capacity_ = new_cap;
        }
    }

    void maybe_shrink() {
        // If on heap and can fit into SSO, move back to SSO
        if (!using_sso_ && length_ <= 15) {
            char* src = storage.heap_ptr;
            std::memcpy(storage.small_buffer, src, length_ + 1);
            delete[] storage.heap_ptr;
            using_sso_ = true;
            capacity_ = 15;
        } else if (!using_sso_) {
            // shrink heap capacity if much larger than needed
            if (length_ < capacity_ / 4) {
                size_t new_cap = length_ * 2 + 1; // keep some headroom
                if (new_cap < 16) new_cap = 16; // avoid tiny
                if (new_cap < length_) new_cap = length_;
                if (new_cap < 15) new_cap = 15;
                if (new_cap < capacity_) {
                    char* new_mem = new char[new_cap + 1];
                    std::memcpy(new_mem, storage.heap_ptr, length_ + 1);
                    delete[] storage.heap_ptr;
                    storage.heap_ptr = new_mem;
                    capacity_ = new_cap;
                }
            }
        }
    }

public:
    MyString() {
        using_sso_ = true;
        length_ = 0;
        capacity_ = 15;
        storage.small_buffer[0] = '\0';
    }

    MyString(const char* s) {
        if (!s) {
            using_sso_ = true;
            length_ = 0;
            capacity_ = 15;
            storage.small_buffer[0] = '\0';
            return;
        }
        size_t n = std::strlen(s);
        if (n <= 15) {
            using_sso_ = true;
            length_ = n;
            capacity_ = 15;
            std::memcpy(storage.small_buffer, s, n + 1);
        } else {
            using_sso_ = false;
            length_ = n;
            capacity_ = n;
            allocate_heap(capacity_);
            std::memcpy(storage.heap_ptr, s, n + 1);
        }
    }

    MyString(const MyString& other) {
        length_ = other.length_;
        capacity_ = other.capacity_;
        using_sso_ = other.using_sso_ || other.length_ <= 15;
        if (using_sso_) {
            capacity_ = 15;
            std::memcpy(storage.small_buffer, other.data_ptr(), length_ + 1);
        } else {
            allocate_heap(other.length_);
            std::memcpy(storage.heap_ptr, other.data_ptr(), length_ + 1);
        }
    }

    MyString(MyString&& other) noexcept {
        length_ = other.length_;
        capacity_ = other.capacity_;
        using_sso_ = other.using_sso_;
        if (other.using_sso_) {
            std::memcpy(storage.small_buffer, other.storage.small_buffer, other.length_ + 1);
        } else {
            storage.heap_ptr = other.storage.heap_ptr;
            other.storage.heap_ptr = nullptr;
        }
        other.length_ = 0;
        other.using_sso_ = true;
        other.capacity_ = 15;
        other.storage.small_buffer[0] = '\0';
    }

    MyString& operator=(MyString&& other) noexcept {
        if (this == &other) return *this;
        if (!using_sso_ && storage.heap_ptr) delete[] storage.heap_ptr;
        length_ = other.length_;
        capacity_ = other.capacity_;
        using_sso_ = other.using_sso_;
        if (other.using_sso_) {
            std::memcpy(storage.small_buffer, other.storage.small_buffer, other.length_ + 1);
        } else {
            storage.heap_ptr = other.storage.heap_ptr;
            other.storage.heap_ptr = nullptr;
        }
        other.length_ = 0;
        other.using_sso_ = true;
        other.capacity_ = 15;
        other.storage.small_buffer[0] = '\0';
        return *this;
    }

    MyString& operator=(const MyString& other) {
        if (this == &other) return *this;
        if (!using_sso_ && storage.heap_ptr) {
            delete[] storage.heap_ptr;
            storage.heap_ptr = nullptr;
        }
        length_ = other.length_;
        using_sso_ = other.using_sso_ || other.length_ <= 15;
        capacity_ = using_sso_ ? 15 : other.length_;
        if (using_sso_) {
            std::memcpy(storage.small_buffer, other.data_ptr(), length_ + 1);
        } else {
            allocate_heap(capacity_);
            std::memcpy(storage.heap_ptr, other.data_ptr(), length_ + 1);
        }
        return *this;
    }

    ~MyString() {
        if (!using_sso_ && storage.heap_ptr) {
            delete[] storage.heap_ptr;
            storage.heap_ptr = nullptr;
        }
    }

    const char* c_str() const { return data_ptr(); }

    size_t size() const { return length_; }

    size_t capacity() const { return using_sso_ ? 15 : capacity_; }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity()) return;
        ensure_capacity(new_capacity);
    }

    void resize(size_t new_size) {
        if (new_size <= length_) {
            length_ = new_size;
            set_terminator();
            maybe_shrink();
            return;
        }
        ensure_capacity(new_size);
        // fill with zeros
        std::memset(data_ptr() + length_, 0, new_size - length_);
        length_ = new_size;
        set_terminator();
    }

    char& operator[](size_t index) {
        if (index >= length_) throw std::out_of_range("index out of range");
        return data_ptr()[index];
    }

    MyString operator+(const MyString& rhs) const {
        MyString res;
        size_t total = length_ + rhs.length_;
        res.resize(total);
        std::memcpy(res.data_ptr(), data_ptr(), length_);
        std::memcpy(res.data_ptr() + length_, rhs.data_ptr(), rhs.length_);
        res.set_terminator();
        return res;
    }

    void append(const char* str) {
        if (!str) return;
        size_t add = std::strlen(str);
        if (add == 0) return;
        size_t old = length_;
        resize(length_ + add);
        std::memcpy(data_ptr() + old, str, add);
        set_terminator();
    }

    const char& at(size_t pos) const {
        if (pos >= length_) throw std::out_of_range("at out of range");
        return data_ptr()[pos];
    }

    class const_iterator;

    class iterator {
    private:
        char* ptr_ = nullptr;
    public:
        explicit iterator(char* p=nullptr) : ptr_(p) {}
        iterator& operator++() { ++ptr_; return *this; }
        iterator operator++(int) { iterator tmp=*this; ++(*this); return tmp; }
        iterator& operator--() { --ptr_; return *this; }
        iterator operator--(int) { iterator tmp=*this; --(*this); return tmp; }
        char& operator*() const { return *ptr_; }
        bool operator==(const iterator& other) const { return ptr_==other.ptr_; }
        bool operator!=(const iterator& other) const { return ptr_!=other.ptr_; }
        bool operator==(const const_iterator& other) const { return ptr_==(other.ptr_); }
        bool operator!=(const const_iterator& other) const { return ptr_!=(other.ptr_); }
        friend class MyString;
        friend class const_iterator;
    };

    class const_iterator {
    private:
        const char* ptr_ = nullptr;
    public:
        explicit const_iterator(const char* p=nullptr) : ptr_(p) {}
        const_iterator& operator++() { ++ptr_; return *this; }
        const_iterator operator++(int) { const_iterator tmp=*this; ++(*this); return tmp; }
        const_iterator& operator--() { --ptr_; return *this; }
        const_iterator operator--(int) { const_iterator tmp=*this; --(*this); return tmp; }
        const char& operator*() const { return *ptr_; }
        bool operator==(const const_iterator& other) const { return ptr_==other.ptr_; }
        bool operator!=(const const_iterator& other) const { return ptr_!=other.ptr_; }
        friend class iterator;
        friend class MyString;
    };

public:
    iterator begin() { return iterator(const_cast<char*>(data_ptr())); }
    iterator end() { return iterator(const_cast<char*>(data_ptr()) + length_); }
    const_iterator cbegin() const { return const_iterator(data_ptr()); }
    const_iterator cend() const { return const_iterator(data_ptr() + length_); }
};

#endif // SIMPLE_STRING_SIMPLESTRING_HPP
