#ifndef INTRUSIVE_PTR_HPP
#define INTRUSIVE_PTR_HPP

#include <algorithm>

namespace smart_ptr {


//    template<typename T>
//    class intrusive_ptr;
//
//    namespace {

        struct intrusive_ptr_base {

        template<typename T>
        friend class intrusive_ptr;

        protected:
            virtual void add_ref() = 0;

            virtual void release() = 0;

            virtual int get_refcnt() = 0;

        };

//    }

    template<typename T>
    class intrusive_ptr : public intrusive_ptr_base {
    private:

        intrusive_ptr_base *handler = nullptr;
        T *value = nullptr;


    private:
        void add_ref() override {
            if (handler != nullptr) {
                handler->add_ref();
            } else if (value != nullptr) {
                value->add_ref();
            }
        }

        void release() override {
            if (handler != nullptr) {
                return handler->release();
            } else if (value != nullptr) {
                value->release();
            }
        }

    public:
        intrusive_ptr() = default;

        explicit intrusive_ptr(T *value) : value(value), handler(nullptr) {
            add_ref();
        }

        intrusive_ptr(const intrusive_ptr &other) : value(other.value) {
            handler = const_cast<intrusive_ptr_base *>(static_cast<const intrusive_ptr_base *>(&other));
            add_ref();
        }

        intrusive_ptr(intrusive_ptr &&other) noexcept : handler(nullptr) {
            std::swap(value, other.value);
        }

        template<typename U>
        intrusive_ptr(const intrusive_ptr<U> &other) : value(other.value) {
            handler = const_cast<intrusive_ptr_base *>(static_cast<const intrusive_ptr_base *>(&other));
            add_ref();
        }

        template<typename U>
        intrusive_ptr(const intrusive_ptr<U> &other, T *value) : value(value) {
            handler = const_cast<intrusive_ptr_base *>(static_cast<const intrusive_ptr_base *>(&other));
            add_ref();
        }

        ~intrusive_ptr() {
            release();
        }

        intrusive_ptr &operator=(const intrusive_ptr &other) {
            release();
            value = other.value;
            handler = &other;
            add_ref();
            return *this;
        }

        intrusive_ptr &operator=(intrusive_ptr &&other) noexcept {
            release();
            std::swap(value, other.value);
            std::swap(handler, &other);
            return *this;
        }

        template<typename U>
        intrusive_ptr &operator=(intrusive_ptr<U> &other) {
            release();
            value = other.value;
            handler = &other;
            add_ref();
            return *this;
        }

        void swap(intrusive_ptr &other) {
            if (value == other.value) {
                return;
            }

            std::swap(value, other.value);
            std::swap(handler, other.handler);
        }

        int get_refcnt() override {
            if (handler != nullptr) {
                return handler->get_refcnt();
            } else if (value != nullptr) {
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
        if (ptr == nullptr) {
            return intrusive_ptr<T>();
        } else {
            return intrusive_ptr<T>(other, ptr);
        }
    }

}

#endif // INTRUSIVE_PTR_HPP