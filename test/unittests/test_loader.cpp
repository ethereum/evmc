// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 The EVMC Authors.
// Licensed under the Apache License, Version 2.0. See the LICENSE file.

#include <evmc/loader.h>

#include <gtest/gtest.h>

#include <cstring>

#if _WIN32
static constexpr bool is_windows = true;
#else
static constexpr bool is_windows = false;
#endif

TEST(loader, nonexistent)
{
    evmc_loader_error_code ec;
    auto x = evmc_load("nonexistent", &ec);
    EXPECT_EQ(ec, EVMC_LOADER_CANNOT_OPEN);
    EXPECT_EQ(x, nullptr);

    x = evmc_load("nonexistent", nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, longpath)
{
    std::vector<char> path(5000, 'a');
    *path.rbegin() = 0;

    evmc_loader_error_code ec;
    auto x = evmc_load(path.data(), &ec);
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path.data(), nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, not_so)
{
    auto path = "unittests/empty.file";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_CANNOT_OPEN);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, null_path)
{
    evmc_loader_error_code ec;
    auto x = evmc_load(nullptr, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(nullptr, nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, empty_path)
{
    evmc_loader_error_code ec;
    auto x = evmc_load("", &ec);
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_EQ(x, nullptr);

    x = evmc_load("", nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, aaa)
{
    auto path = "unittests/libaaa.so";

    evmc_loader_error_code ec;
    auto fn = evmc_load(path, &ec);
    ASSERT_NE(fn, nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ((uintptr_t)fn(), 0xaaa);

    fn = evmc_load(path, nullptr);
    ASSERT_NE(fn, nullptr);
    EXPECT_EQ((uintptr_t)fn(), 0xaaa);
}

TEST(loader, prefix_aaa)
{
    auto paths = {"unittests/double-prefix-aaa.evm", "unittests/double_prefix_aaa.evm"};

    for (auto& path : paths)
    {
        evmc_loader_error_code ec;
        auto fn = evmc_load(path, &ec);
        ASSERT_NE(fn, nullptr);
        EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
        EXPECT_EQ((uintptr_t)fn(), 0xaaa);
    }
}

TEST(loader, eee_bbb)
{
    auto path = "unittests/eee-bbb.dll";

    evmc_loader_error_code ec;
    auto fn = evmc_load(path, &ec);
    ASSERT_NE(fn, nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ((uintptr_t)fn(), 0xeeebbb);
}

#if _WIN32
TEST(loader, nextto)
{
    // On Unix dlopen searches for system libs when the path does not contain "/".

    auto path = "aaa.evm";

    evmc_loader_error_code ec;
    auto fn = evmc_load(path, &ec);
    ASSERT_NE(fn, nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ((uintptr_t)fn(), 0xaaa);
}
#endif

TEST(loader, windows_path)
{
    auto paths = {
        "./aaa.evm",
        ".\\aaa.evm",
        "./unittests/eee-bbb.dll",
        "./unittests\\eee-bbb.dll",
        ".\\unittests\\eee-bbb.dll",
        ".\\unittests/eee-bbb.dll",
        "unittests\\eee-bbb.dll",
    };

    for (auto& path : paths)
    {
        bool is_windows_path = std::strchr(path, '\\') != nullptr;

        if (is_windows_path && !is_windows)
        {
            evmc_loader_error_code ec;
            auto fn = evmc_load(path, &ec);
            EXPECT_EQ(fn, nullptr);
            EXPECT_EQ(ec, EVMC_LOADER_CANNOT_OPEN);
        }
        else
        {
            evmc_loader_error_code ec;
            auto fn = evmc_load(path, &ec);
            EXPECT_NE(fn, nullptr);
            EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
        }
    }
}

TEST(loader, eee1)
{
    auto path = "unittests/libeee1.so";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, eee2)
{
    auto path = "unittests/eee2.so";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, eee3)
{
    auto path = "unittests/libeee3.x";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}

#if !_WIN32
TEST(loader, eee4)
{
    // Windows is not loading DLLs without extensions.
    auto path = "unittests/eee4";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, _)
{
    // Windows is not loading DLLs without extensions.
    auto path = "unittests/_";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}
#endif

TEST(loader, lib_)
{
    // Windows is not loading DLLs without extensions.
    auto path = "unittests/lib_.so";

    evmc_loader_error_code ec;
    auto x = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
    EXPECT_EQ(x, nullptr);

    x = evmc_load(path, nullptr);
    EXPECT_EQ(x, nullptr);
}

TEST(loader, load_default)
{
    auto path = "unittests/default.evmc";

    evmc_loader_error_code ec;
    auto fn = evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ((uintptr_t)fn(), 0xdeaf);

    fn = evmc_load(path, nullptr);
    EXPECT_EQ((uintptr_t)fn(), 0xdeaf);
}

TEST(loader, load_and_create_failure)
{
    evmc_loader_error_code ec;
    auto vm = evmc_load_and_create("unittests/failure.vm", &ec);
    EXPECT_EQ(vm, nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_INSTANCE_CREATION_FAILURE);
}

TEST(loader, load_and_create_abi_mismatch)
{
    evmc_loader_error_code ec;
    auto vm = evmc_load_and_create("unittests/abi42.vm", &ec);
    EXPECT_EQ(vm, nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_ABI_VERSION_MISMATCH);
}
