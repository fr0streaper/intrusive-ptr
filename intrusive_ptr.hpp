#ifndef INTRUSIVE_PTR_HPP
#define INTRUSIVE_PTR_HPP

#include <algorithm>

namespace smart_ptr {

    template<typename T>
    class intrusive_ptr {

        T *value = nullptr;

        void add_ref() {
            if (value != nullptr) {
                value->add_ref();
            }
        }

        void release() {
            if (value != nullptr) {
                value->release();
            }
        }

    public:

        intrusive_ptr() = default;

        explicit intrusive_ptr(T *value) : value(value) {
            add_ref();
        }

        intrusive_ptr(const intrusive_ptr &other) : value(other.value) {
            add_ref();
        }

        intrusive_ptr(intrusive_ptr &&other) noexcept {
            std::swap(value, other.value);
        }

        template<typename U>
        intrusive_ptr(const intrusive_ptr<U> &other) : value(other.value) {
            add_ref();
        }

        ~intrusive_ptr() {
            release();
        }

        intrusive_ptr &operator=(const intrusive_ptr &other) {
            release();
            value = other.value;
            add_ref();
            return *this;
        }

        intrusive_ptr &operator=(intrusive_ptr &&other) noexcept {
            release();
            std::swap(value, other.value);
            return *this;
        }

        template<typename U>
        intrusive_ptr &operator=(intrusive_ptr<U> &other) {
            release();
            value = other.value;
            add_ref();
            return *this;
        }

        void swap(intrusive_ptr &other) {
            if (value == other.value) {
                return;
            }

            std::swap(value, other.value);
        }

        int get_refcnt() const {
            if (value != nullptr) {
                return value->get_refcnt();
            } else {
                return 0;
            }
        }

        T &operator*() const {
            return *value;
        }

        T *operator->() const {
            return value;
        }

        T *get() const {
            return value;
        }

        explicit operator bool() const {
            return value != nullptr;
        }

        bool operator!() const {
            return value == nullptr;
        }

    };

    template<typename T, typename U>
    intrusive_ptr<T> dynamic_pointer_cast(const intrusive_ptr<U> &other) noexcept {
        auto ptr = dynamic_cast<T *>(other.get());
        return intrusive_ptr<T>(ptr);
    }

}

#endif // INTRUSIVE_PTR_HPP