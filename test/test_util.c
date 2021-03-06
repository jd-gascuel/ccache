// Copyright (C) 2010-2016 Joel Rosdahl
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// This file contains tests for functions in util.c.

#include "../ccache.h"
#include "framework.h"

TEST_SUITE(util)

TEST(basename)
{
	CHECK_STR_EQ_FREE2("foo.c", basename("foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("dir1/dir2/foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("/dir/foo.c"));
	CHECK_STR_EQ_FREE2("", basename("dir1/dir2/"));

#ifdef _WIN32
	CHECK_STR_EQ_FREE2("foo.c", basename("C:/dir1/dir2/foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("D:/dir1/dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("E:/dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("F:\\dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("\\dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("/dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("foo.c", basename("/dir1/dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("", basename("G:/dir1/dir2/"));
	CHECK_STR_EQ_FREE2("", basename("H:/dir1\\dir2\\"));
	CHECK_STR_EQ_FREE2("", basename("I:\\dir1\\dir2\\"));
#endif
}

TEST(dirname)
{
	CHECK_STR_EQ_FREE2(".", dirname("foo.c"));
	CHECK_STR_EQ_FREE2(".", dirname(""));
	CHECK_STR_EQ_FREE2("/", dirname("/"));
	CHECK_STR_EQ_FREE2("/", dirname("/foo.c"));
	CHECK_STR_EQ_FREE2("dir1/dir2", dirname("dir1/dir2/foo.c"));
	CHECK_STR_EQ_FREE2("/dir", dirname("/dir/foo.c"));
	CHECK_STR_EQ_FREE2("dir1/dir2", dirname("dir1/dir2/"));

#ifdef _WIN32
	CHECK_STR_EQ_FREE2("C:/dir1/dir2", dirname("C:/dir1/dir2/foo.c"));
	CHECK_STR_EQ_FREE2("D:/dir1/dir2", dirname("D:/dir1/dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("E:/dir1/dir2", dirname("E:/dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("F:/dir1/dir2", dirname("F:\\dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("/dir1/dir2", dirname("\\dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("/dir1/dir2", dirname("/dir1\\dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("/dir1/dir2", dirname("/dir1/dir2\\foo.c"));
	CHECK_STR_EQ_FREE2("G:/dir1/dir2", dirname("G:/dir1/dir2/"));
	CHECK_STR_EQ_FREE2("H:/dir1/dir2", dirname("H:/dir1\\dir2\\"));
	CHECK_STR_EQ_FREE2("I:/dir1/dir2", dirname("I:\\dir1\\dir2\\"));
#endif
}

TEST(str_quote_balance)
{
	CHECK(str_quote_balance("no quote"));
	CHECK(str_quote_balance("bask-slashed \\\" quote"));
	CHECK(str_quote_balance("balanced ending \"quote\""));
	CHECK(str_quote_balance("\"several\" balanced \"quoted\" words"));
	CHECK(!str_quote_balance("unbalanced \" quote"));
	CHECK(!str_quote_balance("\"unbalanced \\\\\" quotes\""));
#ifndef _WIN32
	CHECK(str_quote_balance("'balanced' squote"));
	CHECK(str_quote_balance("'balanced' mixed \"quote\""));
	CHECK(str_quote_balance("'also \"balanced' \"mixed' quotes\""));
	CHECK(str_quote_balance("'single \"balanced' \"mixed quotes\""));
	CHECK(str_quote_balance("'double balanced' \"mixed 'quotes\""));
	CHECK(!str_quote_balance("'un 'balanced' squote"));
#endif
}

TEST(path_eq)
{
	CHECK(path_eq("/abc", "/abc"));
	CHECK(!path_eq("/abc", "/ab"));
	CHECK(!path_eq("/ab", "/abc"));
	CHECK(!path_eq("a", "b"));

#ifdef _WIN32
	CHECK(path_eq("c:/a/b", "C:\\A\\B"));
	CHECK(path_eq("C:\\A\\B", "c:/a/b"));
#endif
}

TEST(path_startswith)
{
	CHECK(path_startswith("/", "/"));
	CHECK(path_startswith("/a", "/"));
	CHECK(path_startswith("/a", "/a"));
	CHECK(path_startswith("/a/b", "/a"));
	CHECK(path_startswith("/a/b", "/a/b"));

	CHECK(!path_startswith("/a", "/b"));
	CHECK(!path_startswith("/a", "/a/b"));

#ifdef _WIN32
	CHECK(path_startswith("C:/a/b", "C:/a"));
	CHECK(path_startswith("D:/a/b", "d:/A"));
	CHECK(path_startswith("e:\\A/B", "E:/a"));
	CHECK(path_startswith("F:/a/b", "F:\\a"));
	CHECK(!path_startswith("C:/a/b", "D:/a"));
	CHECK(!path_startswith("G:/b", "G:/a"));
	CHECK(path_startswith("H:\\\\a\\\\b\\\\c", "H:/a/b/c"));
	CHECK(path_startswith("I:\\\\a/b\\\\c", "I:\\a\\\\b\\c"));
	CHECK(!path_startswith("\\\\server\\file.c", "/server/file.c"));
#endif
}

TEST(common_dir_prefix_length)
{
	CHECK_INT_EQ(0, common_dir_prefix_length("", ""));
	CHECK_INT_EQ(0, common_dir_prefix_length("/", "/"));
	CHECK_INT_EQ(0, common_dir_prefix_length("/", "/b"));
	CHECK_INT_EQ(0, common_dir_prefix_length("/a", "/b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("/a", "/a"));
	CHECK_INT_EQ(2, common_dir_prefix_length("/a", "/a/b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("/a/b", "/a/c"));
	CHECK_INT_EQ(4, common_dir_prefix_length("/a/b", "/a/b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("/a/bc", "/a/b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("/a/b", "/a/bc"));
	CHECK_INT_EQ(0, common_dir_prefix_length("foo", "bar"));
	CHECK_INT_EQ(2, common_dir_prefix_length("/a/bbbb", "/a/b/c"));
	CHECK_INT_EQ(5, common_dir_prefix_length("/a/b/", "/a/b/"));
#ifdef _WIN32
	CHECK_INT_EQ(0, common_dir_prefix_length("\\", "\\b"));
	CHECK_INT_EQ(0, common_dir_prefix_length("\\", "/b"));
	CHECK_INT_EQ(0, common_dir_prefix_length("/", "\\b"));
	CHECK_INT_EQ(5, common_dir_prefix_length("/a/bc", "/a/bc"));
	CHECK_INT_EQ(5, common_dir_prefix_length("\\a\\bc", "\\a\\bc"));
	CHECK_INT_EQ(2, common_dir_prefix_length("\\a\\bc", "\\a\\b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("\\a\\b", "\\a\\bc"));
	CHECK_INT_EQ(2, common_dir_prefix_length("\\a\\bc", "\\a/b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("\\a/bc", "\\a/b"));
	CHECK_INT_EQ(2, common_dir_prefix_length("\\a/b", "\\a\\bc"));
	CHECK_INT_EQ(2, common_dir_prefix_length("\\a/b", "\\a/bc"));

	CHECK_INT_EQ(7, common_dir_prefix_length("\\A/B\\C/", "/a/b/c/"));
#endif
}

TEST(get_relative_path)
{
#ifdef _WIN32
	CHECK_STR_EQ_FREE2("a", get_relative_path("C:/doesn't matter", "a"));
	CHECK_STR_EQ_FREE2("a/b", get_relative_path("C:/doesn't matter", "a/b"));
	CHECK_STR_EQ_FREE2(".", get_relative_path("C:/a", "C:/a"));
	CHECK_STR_EQ_FREE2("..", get_relative_path("C:/a/b", "C:/a"));
	CHECK_STR_EQ_FREE2("b", get_relative_path("C:/a", "C:/a/b"));
	CHECK_STR_EQ_FREE2("b/c", get_relative_path("C:/a", "C:/a/b/c"));
	CHECK_STR_EQ_FREE2("../c", get_relative_path("C:/a/b", "C:/a/c"));
	CHECK_STR_EQ_FREE2("../c/d", get_relative_path("C:/a/b", "C:/a/c/d"));
	CHECK_STR_EQ_FREE2("../../c/d", get_relative_path("C:/a/b/c", "C:/a/c/d"));
	CHECK_STR_EQ_FREE2("../..", get_relative_path("C:/a/b", "C:/"));
	CHECK_STR_EQ_FREE2("../../c", get_relative_path("C:/a/b", "C:/c"));
	CHECK_STR_EQ_FREE2("a/b", get_relative_path("C:/", "C:/a/b"));

	CHECK_STR_EQ_FREE2("a/b/c", get_relative_path("C:\\", "C:\\a\\b\\c"));
	CHECK_STR_EQ_FREE2("b/c", get_relative_path("C:\\a", "C:\\a\\b\\c"));
	CHECK_STR_EQ_FREE2("c", get_relative_path("C:\\a\\b", "C:\\a\\b\\c"));
	CHECK_STR_EQ_FREE2(".", get_relative_path("C:\\a\\b\\c", "C:\\a\\b\\c"));
	CHECK_STR_EQ_FREE2("..", get_relative_path("C:\\a\\b\\c", "C:\\a\\b"));
	CHECK_STR_EQ_FREE2("../..", get_relative_path("C:\\a\\b\\c", "C:\\a"));
	CHECK_STR_EQ_FREE2("../../..", get_relative_path("C:\\a\\b\\c", "C:\\"));

	CHECK_STR_EQ_FREE2("c", get_relative_path("C:/a/b", "C:\\a/b\\c"));
	CHECK_STR_EQ_FREE2(".", get_relative_path("C:\\a\\b\\c", "C:/a/b/c"));
	CHECK_STR_EQ_FREE2("..", get_relative_path("C:\\a\\b\\c", "C:/a/b"));

	CHECK_STR_EQ_FREE2(".", get_relative_path("c:/a/b", "C:/A/B"));
	CHECK_STR_EQ_FREE2("D:/a/b", get_relative_path("C:/a/b", "D:/a/b"));
#else
	CHECK_STR_EQ_FREE2("a", get_relative_path("/doesn't matter", "a"));
	CHECK_STR_EQ_FREE2("a/b", get_relative_path("/doesn't matter", "a/b"));
	CHECK_STR_EQ_FREE2(".", get_relative_path("/a", "/a"));
	CHECK_STR_EQ_FREE2("..", get_relative_path("/a/b", "/a"));
	CHECK_STR_EQ_FREE2("b", get_relative_path("/a", "/a/b"));
	CHECK_STR_EQ_FREE2("b/c", get_relative_path("/a", "/a/b/c"));
	CHECK_STR_EQ_FREE2("../c", get_relative_path("/a/b", "/a/c"));
	CHECK_STR_EQ_FREE2("../c/d", get_relative_path("/a/b", "/a/c/d"));
	CHECK_STR_EQ_FREE2("../../c/d", get_relative_path("/a/b/c", "/a/c/d"));
	CHECK_STR_EQ_FREE2("../..", get_relative_path("/a/b", "/"));
	CHECK_STR_EQ_FREE2("../../c", get_relative_path("/a/b", "/c"));
	CHECK_STR_EQ_FREE2("a/b", get_relative_path("/", "/a/b"));
#endif
}

TEST(format_hash_as_string)
{
	unsigned char hash[16] = {
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	};

	CHECK_STR_EQ_FREE2("00000000000000000000000000000000",
	                   format_hash_as_string(hash, -1));
	CHECK_STR_EQ_FREE2("00000000000000000000000000000000-0",
	                   format_hash_as_string(hash, 0));
	hash[0] = 17;
	hash[15] = 42;
	CHECK_STR_EQ_FREE2("1100000000000000000000000000002a-12345",
	                   format_hash_as_string(hash, 12345));
}

TEST(subst_env_in_string)
{
	char *errmsg;

	putenv("FOO=bar");

	CHECK_STR_EQ_FREE2("bar",
	                   subst_env_in_string("$FOO", &errmsg));
	CHECK(!errmsg);

	errmsg = "";
	CHECK_STR_EQ_FREE2("$",
	                   subst_env_in_string("$", &errmsg));
	CHECK(!errmsg);

	errmsg = "";
	CHECK_STR_EQ_FREE2("bar bar:bar",
	                   subst_env_in_string("$FOO $FOO:$FOO", &errmsg));
	CHECK(!errmsg);

	errmsg = "";
	CHECK_STR_EQ_FREE2("xbar",
	                   subst_env_in_string("x$FOO", &errmsg));
	CHECK(!errmsg);

	errmsg = "";
	CHECK_STR_EQ_FREE2("barx",
	                   subst_env_in_string("${FOO}x", &errmsg));
	CHECK(!errmsg);

	CHECK(!subst_env_in_string("$surelydoesntexist", &errmsg));
	CHECK_STR_EQ_FREE2("environment variable \"surelydoesntexist\" not set",
	                   errmsg);

	CHECK(!subst_env_in_string("${FOO", &errmsg));
	CHECK_STR_EQ_FREE2("syntax error: missing '}' after \"FOO\"", errmsg);

	x_unsetenv("FOO");
	CHECK(!subst_env_in_string("$FOO", &errmsg));
	CHECK_STR_EQ_FREE2("environment variable \"FOO\" not set",
	                   errmsg);
}

TEST(format_human_readable_size)
{
	CHECK_STR_EQ_FREE2("0.0 kB", format_human_readable_size(0));
	CHECK_STR_EQ_FREE2("0.0 kB", format_human_readable_size(49));
	CHECK_STR_EQ_FREE2("0.1 kB", format_human_readable_size(50));
	CHECK_STR_EQ_FREE2("42.0 kB", format_human_readable_size(42 * 1000));
	CHECK_STR_EQ_FREE2("1.0 MB", format_human_readable_size(1000 * 1000));
	CHECK_STR_EQ_FREE2("1.2 MB", format_human_readable_size(1234 * 1000));
	CHECK_STR_EQ_FREE2("438.5 MB",
	                   format_human_readable_size(438.5 * 1000 * 1000));
	CHECK_STR_EQ_FREE2("1.0 GB",
	                   format_human_readable_size(1000 * 1000 * 1000));
	CHECK_STR_EQ_FREE2("17.1 GB",
	                   format_human_readable_size(17.11 * 1000 * 1000 * 1000));
}

TEST(format_parsable_size_with_suffix)
{
	CHECK_STR_EQ_FREE2("0", format_parsable_size_with_suffix(0));
	CHECK_STR_EQ_FREE2("42.0k", format_parsable_size_with_suffix(42 * 1000));
	CHECK_STR_EQ_FREE2("1.0M", format_parsable_size_with_suffix(1000 * 1000));
	CHECK_STR_EQ_FREE2("1.2M", format_parsable_size_with_suffix(1234 * 1000));
	CHECK_STR_EQ_FREE2("438.5M",
	                   format_parsable_size_with_suffix(438.5 * 1000 * 1000));
	CHECK_STR_EQ_FREE2("1.0G",
	                   format_parsable_size_with_suffix(1000 * 1000 * 1000));
	CHECK_STR_EQ_FREE2(
		"17.1G",
		format_parsable_size_with_suffix(17.11 * 1000 * 1000 * 1000));
}

TEST(parse_size_with_suffix)
{
	uint64_t size;
	size_t i;
	struct { const char *size; int64_t expected; } sizes[] = {
		{"0", 0},
		{"42", (int64_t)42 * 1000 * 1000 * 1000},     // Default suffix: G

		{"78k",       78 * 1000},
		{"78K",       78 * 1000},
		{"1.1 M",     1.1 * 1000 * 1000},
		{"438.55M",   438.55 * 1000 * 1000},
		{"1 G",       1 * 1000 * 1000 * 1000},
		{"2T",        (int64_t)2 * 1000 * 1000 * 1000 * 1000},

		{"78 Ki",     78 * 1024},
		{"1.1Mi",     1.1 * 1024 * 1024},
		{"438.55 Mi", 438.55 * 1024 * 1024},
		{"1Gi",       1 * 1024 * 1024 * 1024},
		{"2 Ti",      (int64_t)2 * 1024 * 1024 * 1024 * 1024},

	};

	for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
		CHECKM(parse_size_with_suffix(sizes[i].size, &size), sizes[i].size);
		CHECK_INT_EQ(sizes[i].expected, size);
	}
}

TEST_SUITE_END
