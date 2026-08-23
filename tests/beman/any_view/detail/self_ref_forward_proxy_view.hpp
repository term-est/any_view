// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <cstddef>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

struct self_ref_forward_proxy {
    const int* pointer{};

    operator int() const noexcept { return *pointer; }
};

static_assert(std::is_trivially_copyable_v<self_ref_forward_proxy>);

struct self_ref_forward_proxy_iterator {
    using value_type       = int;
    using difference_type  = std::ptrdiff_t;
    using iterator_concept = std::forward_iterator_tag;

    mutable int value{};
    int         stop{};

    self_ref_forward_proxy_iterator() = default;

    self_ref_forward_proxy_iterator(int value, int stop)
        : value(value), stop(stop) {}

    self_ref_forward_proxy_iterator(
        const self_ref_forward_proxy_iterator&) = default;

    self_ref_forward_proxy_iterator&
    operator=(const self_ref_forward_proxy_iterator&) = default;

    self_ref_forward_proxy_iterator(
        self_ref_forward_proxy_iterator&& other) noexcept
        : value(other.value), stop(other.stop) {
        other.value = -777;
    }

    self_ref_forward_proxy_iterator&
    operator=(self_ref_forward_proxy_iterator&& other) noexcept {
        value = other.value;
        stop  = other.stop;

        other.value = -777;
        return *this;
    }

    self_ref_forward_proxy operator*() const noexcept {
        // The proxy points into this iterator.
        return {&value};
    }

    self_ref_forward_proxy_iterator& operator++() noexcept {
        ++value;
        return *this;
    }

    self_ref_forward_proxy_iterator operator++(int) noexcept {
        auto previous = *this;
        ++*this;
        return previous;
    }

    friend bool operator==(
        const self_ref_forward_proxy_iterator&,
        const self_ref_forward_proxy_iterator&) = default;
};

static_assert(
    std::forward_iterator<self_ref_forward_proxy_iterator>
);

struct self_ref_forward_proxy_view : std::ranges::view_base {
    int first{};
    int stop{};

    self_ref_forward_proxy_view() = default;

    self_ref_forward_proxy_view(int first, int stop)
        : first(first), stop(stop) {}

    self_ref_forward_proxy_iterator begin() const noexcept {
        return {first, stop};
    }

    self_ref_forward_proxy_iterator end() const noexcept {
        return {stop, stop};
    }
};

static_assert(
    std::ranges::forward_range<self_ref_forward_proxy_view>
);