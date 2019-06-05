// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/evmc.h>
#include <evmc/loader.h>
#include <gtest/gtest.h>
#include <cstring>
#include <unordered_map>
#include <vector>

#if _WIN32
static constexpr bool is_windows = true;
#else
static constexpr bool is_windows = false;
#endif

extern "C" {
#if _WIN32
#define strcpy_sx strcpy_s
#else
/// Declaration of internal function defined in loader.c.
int strcpy_sx(char* dest, size_t destsz, const char* src);
#endif

/// The library path expected by mocked evmc_test_load_library().
extern const char* evmc_test_library_path;

/// The symbol name expected by mocked evmc_test_get_symbol_address().
extern const char* evmc_test_library_symbol;

/// The pointer to function returned by evmc_test_get_symbol_address().
extern evmc_create_fn evmc_test_create_fn;
}

class loader : public ::testing::Test
{
protected:
    static int create_count;
    static int destroy_count;
    static std::unordered_map<std::string, const char*> supported_options;
    static std::vector<std::pair<std::string, std::string>> recorded_options;

    loader() noexcept
    {
        create_count = 0;
        destroy_count = 0;
        supported_options.clear();
        recorded_options.clear();
    }

    void setup(const char* path, const char* symbol, evmc_create_fn fn) noexcept
    {
        evmc_test_library_path = path;
        evmc_test_library_symbol = symbol;
        evmc_test_create_fn = fn;
    }

    static void destroy(evmc_instance*) noexcept { ++destroy_count; }

    static evmc_set_option_result set_option(evmc_instance*,
                                             const char* name,
                                             const char* value) noexcept
    {
        recorded_options.push_back({name, value ? value : "<null>"});  // NOLINT

        auto it = supported_options.find(name);
        if (it == supported_options.end())
            return EVMC_SET_OPTION_INVALID_NAME;
        if (it->second == nullptr)
            return value == nullptr ? EVMC_SET_OPTION_SUCCESS : EVMC_SET_OPTION_INVALID_VALUE;
        if (value == nullptr)
            return EVMC_SET_OPTION_INVALID_VALUE;
        return std::string{value} == it->second ? EVMC_SET_OPTION_SUCCESS :
                                                  EVMC_SET_OPTION_INVALID_VALUE;
    }

    /// Creates a VM mock with only destroy() method.
    static evmc_instance* create_vm_barebone()
    {
        static auto instance =
            evmc_instance{EVMC_ABI_VERSION, "", "", destroy, nullptr, nullptr, nullptr, nullptr};
        ++create_count;
        return &instance;
    }

    /// Creates a VM mock with ABI version different than in this project.
    static evmc_instance* create_vm_with_wrong_abi()
    {
        constexpr auto wrong_abi_version = 1985;
        static_assert(wrong_abi_version != EVMC_ABI_VERSION, "");
        static auto instance =
            evmc_instance{wrong_abi_version, "", "", destroy, nullptr, nullptr, nullptr, nullptr};
        ++create_count;
        return &instance;
    }

    /// Creates a VM mock with optional set_option() method.
    static evmc_instance* create_vm_with_set_option() noexcept
    {
        static auto instance =
            evmc_instance{EVMC_ABI_VERSION, "", "", destroy, nullptr, nullptr, nullptr, set_option};
        ++create_count;
        return &instance;
    }
};

int loader::create_count = 0;
int loader::destroy_count = 0;
std::unordered_map<std::string, const char*> loader::supported_options;
std::vector<std::pair<std::string, std::string>> loader::recorded_options;

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

TEST_F(loader, strcpy_sx)
{
    const char input_empty[] = "";
    const char input_that_fits[] = "x";
    const char input_too_big[] = "12";
    char buf[2] = {0x0f, 0x0f};
    static_assert(sizeof(input_empty) <= sizeof(buf), "");
    static_assert(sizeof(input_that_fits) <= sizeof(buf), "");
    static_assert(sizeof(input_too_big) > sizeof(buf), "");

    EXPECT_EQ(strcpy_sx(buf, sizeof(buf), input_empty), 0);
    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[1], 0x0f);

    EXPECT_EQ(strcpy_sx(buf, sizeof(buf), input_that_fits), 0);
    EXPECT_EQ(buf[0], 'x');
    EXPECT_EQ(buf[1], 0);

    EXPECT_NE(strcpy_sx(buf, sizeof(buf), input_too_big), 0);
    EXPECT_EQ(buf[0], 0);
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
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);

    vm = evmc_load_and_create(evmc_test_library_path, nullptr);
    EXPECT_TRUE(vm == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(), "creating EVMC instance of failure.vm has failed");
}

TEST_F(loader, load_and_create_abi_mismatch)
{
    setup("abi1985.vm", "evmc_create", create_vm_with_wrong_abi);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_create(evmc_test_library_path, &ec);
    EXPECT_TRUE(vm == nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_ABI_VERSION_MISMATCH);
    const auto expected_error_msg =
        "EVMC ABI version 1985 of abi1985.vm mismatches the expected version " +
        std::to_string(EVMC_ABI_VERSION);
    EXPECT_EQ(evmc_last_error_msg(), expected_error_msg);
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    EXPECT_EQ(destroy_count, create_count);

    vm = evmc_load_and_create(evmc_test_library_path, nullptr);
    EXPECT_TRUE(vm == nullptr);
    EXPECT_EQ(evmc_last_error_msg(), expected_error_msg);
    EXPECT_EQ(destroy_count, create_count);
}
