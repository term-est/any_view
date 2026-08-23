// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "detail/self_ref_input_view.hpp"

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