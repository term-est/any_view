// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <beman/any_view/any_view.hpp>

struct self_ref_input_iterator {
    using value_type       = int;
    using difference_type  = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;

    mutable int value{};
    int         stop{};

    self_ref_input_iterator() = delete;

    self_ref_input_iterator(int value, int stop) : value(value), stop(stop) {}

    self_ref_input_iterator(const self_ref_input_iterator&)            = delete;
    self_ref_input_iterator& operator=(const self_ref_input_iterator&) = delete;

    self_ref_input_iterator(self_ref_input_iterator&& other) noexcept : value(other.value), stop(other.stop) {
        other.value = -777;
    }

    self_ref_input_iterator& operator=(self_ref_input_iterator&& other) noexcept {
        value = other.value;
        stop  = other.stop;

        other.value = -777;
        return *this;
    }

    int& operator*() const noexcept {
        // The returned reference points inside this iterator.
        return value;
    }

    self_ref_input_iterator& operator++() noexcept {
        ++value;
        return *this;
    }

    void operator++(int) noexcept { ++*this; }

    friend bool operator==(const self_ref_input_iterator& iterator, std::default_sentinel_t) noexcept {
        return iterator.value == iterator.stop;
    }
};

static_assert(std::input_iterator<self_ref_input_iterator>);

struct self_ref_input_view : std::ranges::view_base {
    int first{};
    int stop{};

    self_ref_input_view() = default;

    self_ref_input_view(int first, int stop) : first(first), stop(stop) {}

    self_ref_input_iterator begin() { return {first, stop}; }

    std::default_sentinel_t end() const noexcept { return {}; }
};

static_assert(std::ranges::input_range<self_ref_input_view>);
