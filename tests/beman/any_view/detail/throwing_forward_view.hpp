// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstddef>
#include <iterator>
#include <ranges>
#include <utility>

struct throwing_copy_error {};

struct throwing_forward_iterator {
    using value_type       = int;
    using difference_type  = std::ptrdiff_t;
    using iterator_concept = std::forward_iterator_tag;

    inline static bool throw_on_copy = false;
    inline static int  live_count    = 0;

    int* current{};

    throwing_forward_iterator() noexcept { ++live_count; }

    explicit throwing_forward_iterator(int* current) noexcept : current(current) { ++live_count; }

    throwing_forward_iterator(const throwing_forward_iterator& other) : current(other.current) {
        if (throw_on_copy) {
            throw throwing_copy_error{};
        }

        ++live_count;
    }

    throwing_forward_iterator(throwing_forward_iterator&& other) noexcept
        : current(std::exchange(other.current, nullptr)) {
        ++live_count;
    }

    throwing_forward_iterator& operator=(const throwing_forward_iterator& other) {
        if (throw_on_copy) {
            throw throwing_copy_error{};
        }

        current = other.current;
        return *this;
    }

    throwing_forward_iterator& operator=(throwing_forward_iterator&& other) noexcept {
        current = std::exchange(other.current, nullptr);
        return *this;
    }

    ~throwing_forward_iterator() { --live_count; }

    int& operator*() const noexcept { return *current; }

    throwing_forward_iterator& operator++() noexcept {
        ++current;
        return *this;
    }

    throwing_forward_iterator operator++(int) {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend bool operator==(const throwing_forward_iterator&, const throwing_forward_iterator&) = default;
};

static_assert(std::forward_iterator<throwing_forward_iterator>);

struct throwing_forward_view : std::ranges::view_base {
    int* first{};
    int* last{};

    throwing_forward_view() = default;

    throwing_forward_view(int* first, int* last) : first(first), last(last) {}

    throwing_forward_iterator begin() const noexcept { return throwing_forward_iterator{first}; }

    throwing_forward_iterator end() const noexcept { return throwing_forward_iterator{last}; }
};

static_assert(std::ranges::forward_range<throwing_forward_view>);
