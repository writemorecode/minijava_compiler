#pragma once

#include <new>
#include <type_traits>
#include <utility>

namespace util {

template <typename T, typename E>
class Expected {
  public:
    static Expected ok(T value) { return Expected(std::in_place_index<0>, std::move(value)); }
    static Expected err(E error) { return Expected(std::in_place_index<1>, std::move(error)); }

    Expected(const Expected &other) {
        if (other.has_value_) {
            new (&storage_.value) T(other.storage_.value);
        } else {
            new (&storage_.error) E(other.storage_.error);
        }
        has_value_ = other.has_value_;
    }

    Expected(Expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                        std::is_nothrow_move_constructible_v<E>) {
        if (other.has_value_) {
            new (&storage_.value) T(std::move(other.storage_.value));
        } else {
            new (&storage_.error) E(std::move(other.storage_.error));
        }
        has_value_ = other.has_value_;
    }

    Expected &operator=(const Expected &other) {
        if (this == &other) {
            return *this;
        }
        destroy();
        if (other.has_value_) {
            new (&storage_.value) T(other.storage_.value);
        } else {
            new (&storage_.error) E(other.storage_.error);
        }
        has_value_ = other.has_value_;
        return *this;
    }

    Expected &operator=(Expected &&other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                   std::is_nothrow_move_constructible_v<E>) {
        if (this == &other) {
            return *this;
        }
        destroy();
        if (other.has_value_) {
            new (&storage_.value) T(std::move(other.storage_.value));
        } else {
            new (&storage_.error) E(std::move(other.storage_.error));
        }
        has_value_ = other.has_value_;
        return *this;
    }

    ~Expected() { destroy(); }

    bool has_value() const { return has_value_; }

    T &value() & { return storage_.value; }
    const T &value() const & { return storage_.value; }
    T &&value() && { return std::move(storage_.value); }

    E &error() & { return storage_.error; }
    const E &error() const & { return storage_.error; }
    E &&error() && { return std::move(storage_.error); }

  private:
    template <typename... Args>
    explicit Expected(std::in_place_index_t<0>, Args &&...args)
        : has_value_(true) {
        new (&storage_.value) T(std::forward<Args>(args)...);
    }

    template <typename... Args>
    explicit Expected(std::in_place_index_t<1>, Args &&...args)
        : has_value_(false) {
        new (&storage_.error) E(std::forward<Args>(args)...);
    }

    void destroy() {
        if (has_value_) {
            storage_.value.~T();
        } else {
            storage_.error.~E();
        }
    }

    union Storage {
        Storage() {}
        ~Storage() {}
        T value;
        E error;
    } storage_;

    bool has_value_ = false;
};

template <typename T, typename E>
Expected<T, E> ok(T value) {
    return Expected<T, E>::ok(std::move(value));
}

template <typename T, typename E>
Expected<T, E> err(E error) {
    return Expected<T, E>::err(std::move(error));
}

} // namespace util
