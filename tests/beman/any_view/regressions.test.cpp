// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "detail/self_ref_input_view.hpp"
#include "detail/throwing_forward_view.hpp"
#include "detail/self_ref_forward_proxy_view.hpp"

#include <gtest/gtest.h>

#include <vector>

using beman::any_view::any_view;
using enum beman::any_view::any_view_options;

template <class RefT>
using any_random_access_view = any_view<RefT, random_access, RefT>;

// GH-85
TEST(RegressionTest, moving_input_iterator_rebinds_dereference) {
    any_view<int> view{self_ref_input_view{42, 43}};

    auto source = view.begin();
    auto moved  = std::move(source);

    EXPECT_EQ(*moved, 42);
}

// GH-85
TEST(RegressionTest, move_assigning_input_iterator_rebinds_dereference) {
    any_view<int> view{self_ref_input_view{42, 43}};

    auto destination = view.begin();
    ++destination;

    auto source = view.begin();
    destination = std::move(source);

    EXPECT_EQ(*destination, 42);
}

using proxy_forward_any_view = any_view<int, forward, self_ref_forward_proxy, int>;

// GH-85
TEST(RegressionTest, copying_forward_proxy_iterator_preserves_position) {
    proxy_forward_any_view view{self_ref_forward_proxy_view{42, 44}};

    auto source = view.begin();
    auto copied = source;

    ++source;

    // The copied underlying iterator remains at 42.
    // A copied cache incorrectly points into source and returns 43.
    EXPECT_EQ(static_cast<int>(*copied), 42);
}

// GH-85
TEST(RegressionTest, moving_forward_proxy_iterator_preserves_position) {
    proxy_forward_any_view view{self_ref_forward_proxy_view{42, 44}};

    auto source = view.begin();
    auto moved  = std::move(source);

    // A stale cache points into moved-from source and returns -777.
    EXPECT_EQ(static_cast<int>(*moved), 42);
}

// GH-85
TEST(RegressionTest, copy_assigning_forward_proxy_iterator_preserves_position) {
    proxy_forward_any_view view{self_ref_forward_proxy_view{42, 44}};

    auto source      = view.begin();
    auto destination = view.begin();
    ++destination;

    destination = source;
    ++source;

    EXPECT_EQ(static_cast<int>(*destination), 42);
}

// GH-85
TEST(RegressionTest, move_assigning_forward_proxy_iterator_preserves_position) {
    proxy_forward_any_view view{self_ref_forward_proxy_view{42, 44}};

    auto source      = view.begin();
    auto destination = view.begin();
    ++destination;

    destination = std::move(source);

    EXPECT_EQ(static_cast<int>(*destination), 42);
}

// GH-86
TEST(RegressionTest, copy_assigning_forward_iterator_is_exception_safe) {
    EXPECT_EQ(throwing_forward_iterator::live_count, 0);

    {
        int values[]{1, 2, 3};

        any_view<int, forward> view{throwing_forward_view{values, values + 3}};

        auto source      = view.begin();
        auto destination = view.begin();
        ++destination;

        ASSERT_EQ(*source, 1);
        ASSERT_EQ(*destination, 2);

        const int live_before = throwing_forward_iterator::live_count;

        throwing_forward_iterator::throw_on_copy = true;

        EXPECT_THROW(destination = source, throwing_copy_error);

        throwing_forward_iterator::throw_on_copy = false;

        // The failed replacement must not have destroyed the
        // destination's erased iterator and sentinel.
        EXPECT_EQ(throwing_forward_iterator::live_count, live_before);

        // Strong guarantee: destination retains its original value.
        EXPECT_EQ(*destination, 2);

        // The source was not changed either.
        EXPECT_EQ(*source, 1);
    }

    EXPECT_EQ(throwing_forward_iterator::live_count, 0);
}

TEST(RegressionTest, random_access_proxy_iterator_reaches_end) {
    std::vector<bool> values{true, false, true};
    using proxy = std::ranges::range_reference_t<decltype(values)>;

    any_view<bool, random_access, proxy> view{values};

    auto iterator = view.begin();
    ++iterator;
    ++iterator;
    ++iterator;

    EXPECT_EQ(iterator, view.end());
}
