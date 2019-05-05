// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/loader.h>

#include <gtest/gtest.h>

#include <cstring>

#if _WIN32
static constexpr bool is_windows = true;
#else
static constexpr bool is_windows = false;
#endif

extern "C" {
extern const char* evmc_test_library_path;
extern const char* evmc_test_library_symbol;
extern evmc_create_fn evmc_test_create_fn;
}

class loader : public ::testing::Test
{
protected:
    void setup(const char* path, const char* symbol, evmc_create_fn fn) noexcept
    {
        evmc_test_library_path = path;
        evmc_test_library_symbol = symbol;
        evmc_test_create_fn = fn;
    }
};

static evmc_instance* create_aaa()
{
    return (evmc_instance*)0xaaa;
}

static evmc_instance* create_eee_bbb()
{
    return (evmc_instance*)0xeeebbb;
}

static evmc_instance* create_failure()
{
    return nullptr;
}

static evmc_instance* create_abi42()
{
    static int abi_version = 42;
    return reinterpret_cast<evmc_instance*>(&abi_version);
}

TEST_F(loader, load_nonexistent)
{
    constexpr auto path = "nonexistent";
    evmc_loader_error_code ec;
    EXPECT_TRUE(evmc_load(path, &ec) == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_CANNOT_OPEN);
    EXPECT_TRUE(evmc_load(path, nullptr) == nullptr);
}

TEST_F(loader, load_long_path)
{
    const std::string path(5000, 'a');
    evmc_loader_error_code ec;
    EXPECT_TRUE(evmc_load(path.c_str(), &ec) == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(),
                 "invalid argument: file name is too long (5000, maximum allowed length is 4096)");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_TRUE(evmc_load(path.c_str(), nullptr) == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(),
                 "invalid argument: file name is too long (5000, maximum allowed length is 4096)");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
}

TEST_F(loader, load_null_path)
{
    evmc_loader_error_code ec;
    EXPECT_TRUE(evmc_load(nullptr, &ec) == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_STREQ(evmc_last_error_msg(), "invalid argument: file name cannot be null");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    EXPECT_TRUE(evmc_load(nullptr, nullptr) == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(), "invalid argument: file name cannot be null");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
}

TEST_F(loader, load_empty_path)
{
    evmc_loader_error_code ec;
    EXPECT_TRUE(evmc_load("", &ec) == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(), "invalid argument: file name cannot be empty");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_TRUE(evmc_load("", nullptr) == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(), "invalid argument: file name cannot be empty");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
}

TEST_F(loader, load_prefix_aaa)
{
    auto paths = {
        "./aaa.evm",
        "aaa.evm",
        "unittests/libaaa.so",
        "unittests/double-prefix-aaa.evm",
        "unittests/double_prefix_aaa.evm",
    };

    for (auto& path : paths)
    {
        setup(path, "evmc_create_aaa", create_aaa);
        evmc_loader_error_code ec;
        auto fn = evmc_load(path, &ec);
        EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
        ASSERT_TRUE(fn != nullptr);
        EXPECT_EQ((uintptr_t)fn(), 0xaaa);
        EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    }
}

TEST_F(loader, load_eee_bbb)
{
    setup("unittests/eee-bbb.dll", "evmc_create_eee_bbb", create_eee_bbb);
    evmc_loader_error_code ec;
    auto fn = evmc_load(evmc_test_library_path, &ec);
    ASSERT_TRUE(fn != nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ((uintptr_t)fn(), 0xeeebbb);
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
}


TEST_F(loader, load_windows_path)
{
    auto paths = {
        "./eee-bbb.evm",
        ".\\eee-bbb.evm",
        "./unittests/eee-bbb.dll",
        "./unittests\\eee-bbb.dll",
        ".\\unittests\\eee-bbb.dll",
        ".\\unittests/eee-bbb.dll",
        "unittests\\eee-bbb.dll",
    };

    for (auto& path : paths)
    {
        bool should_open = is_windows || std::strchr(path, '\\') == nullptr;
        setup(should_open ? path : nullptr, "evmc_create_eee_bbb", create_eee_bbb);

        evmc_loader_error_code ec;
        evmc_load(path, &ec);
        if (should_open)
        {
            EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
            EXPECT_TRUE(evmc_last_error_msg() == nullptr);
        }
        else
        {
            EXPECT_EQ(ec, EVMC_LOADER_CANNOT_OPEN);
            EXPECT_STREQ(evmc_last_error_msg(), "cannot load library");
            EXPECT_TRUE(evmc_last_error_msg() == nullptr);
        }
    }
}

TEST_F(loader, load_symbol_not_found)
{
    auto paths = {"libaaa1.so", "eee2.so", "libeee3.x", "eee4", "_", "lib_.so"};

    for (auto& path : paths)
    {
        setup(path, "evmc_create_aaa", create_aaa);

        evmc_loader_error_code ec;
        EXPECT_TRUE(evmc_load(evmc_test_library_path, &ec) == nullptr);
        EXPECT_EQ(ec, EVMC_LOADER_SYMBOL_NOT_FOUND);
        EXPECT_EQ(evmc_last_error_msg(), "EVMC create function not found in " + std::string(path));
        EXPECT_TRUE(evmc_last_error_msg() == nullptr);
        EXPECT_TRUE(evmc_load(evmc_test_library_path, nullptr) == nullptr);
    }
}

TEST_F(loader, load_default_symbol)
{
    setup("default.evmc", "evmc_create", create_aaa);

    evmc_loader_error_code ec;
    auto fn = evmc_load(evmc_test_library_path, &ec);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ(fn, &create_aaa);

    fn = evmc_load(evmc_test_library_path, nullptr);
    EXPECT_EQ(fn, &create_aaa);
}

TEST_F(loader, load_and_create_failure)
{
    setup("failure.vm", "evmc_create", create_failure);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_create(evmc_test_library_path, &ec);
    EXPECT_TRUE(vm == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_INSTANCE_CREATION_FAILURE);
    EXPECT_STREQ(evmc_last_error_msg(), "creating EVMC instance of failure.vm has failed");
}

TEST_F(loader, load_and_create_abi_mismatch)
{
    setup("abi42.vm", "evmc_create", create_abi42);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_create(evmc_test_library_path, &ec);
    EXPECT_TRUE(vm == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_ABI_VERSION_MISMATCH);
    EXPECT_STREQ(evmc_last_error_msg(),
                 "EVMC ABI version 42 of abi42.vm mismatches the expected version 6");
}
