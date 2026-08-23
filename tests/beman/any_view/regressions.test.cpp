// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "detail/self_ref_input_view.hpp"
#include "detail/throwing_forward_view.hpp"

#include <gtest/gtest.h>

#include <string>

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
