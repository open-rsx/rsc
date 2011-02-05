/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2011 Jan Moringen
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <string>
#include <vector>

#include <boost/static_assert.hpp>

#include <gtest/gtest.h>

#include <rsc/runtime/TypeStringTools.h>

using namespace std;
using namespace rsc::runtime;

TEST(TypeStringToolsTest, test_type_name)
{

#if defined DEMANGLE_GCC
    string stringName = "std::string"
#elif defined DEMANGLE_MSVC
    string stringName = "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >";
#else
    BOOST_STATIC_ASSERT(false);
#endif

    EXPECT_EQ("bool", type_name(typeid(bool)));
    EXPECT_EQ("int", type_name(typeid(int)));
    EXPECT_EQ(stringName, type_name(typeid(string)));

    EXPECT_EQ("bool", type_name<bool>());
    EXPECT_EQ("int", type_name<int>());
    EXPECT_EQ(stringName, type_name<string>());

    EXPECT_EQ("bool", type_name(true));
    EXPECT_EQ("int", type_name(1));
    EXPECT_EQ(stringName, type_name(string("bla")));
}

TEST(TypeStringToolsTest, test_type_string)
{
    EXPECT_EQ(type_string("known type %1%",
                    "unknown type",
                    true),
            "known type 1");
    EXPECT_EQ(type_string("known type %1%",
                    "unknown type",
                    5),
            "known type 5");
    EXPECT_EQ(type_string("known type %1%",
                    "unknown type",
                    string("test")),
            "known type test");
    EXPECT_EQ(type_string("known type %1%",
                    "unknown type",
                    vector<int>()),
            "known type #()");
    EXPECT_EQ(type_string("known type %1%",
                    "unknown type",
                    vector< vector<int> >()),
            "known type #()");
}
