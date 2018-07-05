// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018 Pawel Bylica.
// Licensed under the MIT License. See the LICENSE file.

#include <evmc/loader.h>

#include <gtest/gtest.h>

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
    auto x = (uintptr_t)evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ(x, 0xaaa);

    x = (uintptr_t)evmc_load(path, nullptr);
    EXPECT_EQ(x, 0xaaa);
}

TEST(loader, prefix_aaa)
{
    auto paths = {"unittests/double-prefix-aaa.evm", "unittests/double_prefix_aaa.evm"};

    for (auto& path : paths)
    {
        evmc_loader_error_code ec;
        auto x = (uintptr_t)evmc_load(path, &ec);
        EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
        EXPECT_EQ(x, 0xaaa);

        x = (uintptr_t)evmc_load(path, nullptr);
        EXPECT_EQ(x, 0xaaa);
    }
}

TEST(loader, eee_bbb)
{
    auto path = "unittests/eee-bbb.dll";

    evmc_loader_error_code ec;
    auto x = (uintptr_t)evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ(x, 0xeeebbb);

    x = (uintptr_t)evmc_load(path, nullptr);
    EXPECT_EQ(x, 0xeeebbb);
}

#if _WIN32
TEST(loader, nextto)
{
    // On Unix dlopen searches for system libs when the path does not contain "/".

    auto path = "aaa.evm";

    evmc_loader_error_code ec;
    auto x = (uintptr_t)evmc_load(path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ(x, 0xaaa);

    x = (uintptr_t)evmc_load(path, nullptr);
    EXPECT_EQ(x, 0xaaa);
}
#endif

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
#endif
