// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/evmc.h>
#include <evmc/helpers.h>
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
/// Declaration of internal function defined in loader.c.
int strcpy_sx(char* dest, size_t destsz, const char* src);

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
    static std::unordered_map<std::string, std::vector<std::string>> supported_options;
    static std::vector<std::pair<std::string, std::string>> recorded_options;
    static const std::string option_name_causing_unknown_error;

    loader() noexcept
    {
        create_count = 0;
        destroy_count = 0;
        supported_options.clear();
        recorded_options.clear();
    }

    static void setup(const char* path, const char* symbol, evmc_create_fn fn) noexcept
    {
        evmc_test_library_path = path;
        evmc_test_library_symbol = symbol;
        evmc_test_create_fn = fn;
    }

    static void destroy(evmc_vm*) noexcept { ++destroy_count; }

    static evmc_set_option_result set_option(evmc_vm*, const char* name, const char* value) noexcept
    {
        recorded_options.push_back({name, value});  // NOLINT

        auto it = supported_options.find(name);
        if (it == supported_options.end())
            return EVMC_SET_OPTION_INVALID_NAME;

        if (std::find(std::begin(it->second), std::end(it->second), value) != std::end(it->second))
            return EVMC_SET_OPTION_SUCCESS;

        if (name == option_name_causing_unknown_error)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
            return static_cast<evmc_set_option_result>(-42);
#pragma GCC diagnostic pop
        }

        return EVMC_SET_OPTION_INVALID_VALUE;
    }

    /// Creates a VM mock with only destroy() method.
    static evmc_vm* create_vm_barebone()
    {
        static auto instance =
            evmc_vm{EVMC_ABI_VERSION, "vm_barebone", "", destroy, nullptr, nullptr, nullptr};
        ++create_count;
        return &instance;
    }

    /// Creates a VM mock with ABI version different than in this project.
    static evmc_vm* create_vm_with_wrong_abi()
    {
        constexpr auto wrong_abi_version = 1985;
        static_assert(wrong_abi_version != EVMC_ABI_VERSION, "");
        static auto instance =
            evmc_vm{wrong_abi_version, "", "", destroy, nullptr, nullptr, nullptr};
        ++create_count;
        return &instance;
    }

    /// Creates a VM mock with optional set_option() method.
    static evmc_vm* create_vm_with_set_option() noexcept
    {
        static auto instance = evmc_vm{
            EVMC_ABI_VERSION, "vm_with_set_option", "", destroy, nullptr, nullptr, set_option};
        ++create_count;
        return &instance;
    }
};

int loader::create_count = 0;
int loader::destroy_count = 0;
std::unordered_map<std::string, std::vector<std::string>> loader::supported_options;
std::vector<std::pair<std::string, std::string>> loader::recorded_options;

/// The option name that will return unexpected error code from the set_option() method.
const std::string loader::option_name_causing_unknown_error{"raise_unknown"};

static evmc_vm* create_aaa()
{
    return reinterpret_cast<evmc_vm*>(0xaaa);
}

static evmc_vm* create_eee_bbb()
{
    return reinterpret_cast<evmc_vm*>(0xeeebbb);
}

static evmc_vm* create_failure()
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

TEST_F(loader, load_aaa)
{
    auto paths = {
        "./aaa.evm",
        "aaa.evm",
        "unittests/libaaa.so",
    };

    const auto expected_vm_ptr = reinterpret_cast<evmc_vm*>(0xaaa);

    for (auto& path : paths)
    {
        setup(path, "evmc_create_aaa", create_aaa);
        evmc_loader_error_code ec;
        const auto fn = evmc_load(path, &ec);
        EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
        ASSERT_TRUE(fn != nullptr);
        EXPECT_EQ(fn(), expected_vm_ptr);
        EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    }
}

TEST_F(loader, load_file_with_multiple_extensions)
{
    auto paths = {
        "./aaa.evm.0.99",
        "aaa.tar.gz.so",
        "unittests/aaa.x.y.z.so",
        "unittests/aaa.1.lib",
        "unittests/aaa.1.0",
        "unittests/aaa.extextextextextextextextextextextextextextextextext",
    };

    const auto expected_vm_ptr = reinterpret_cast<evmc_vm*>(0xaaa);

    for (auto& path : paths)
    {
        setup(path, "evmc_create_aaa", create_aaa);
        evmc_loader_error_code ec;
        const auto fn = evmc_load(path, &ec);
        EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
        ASSERT_TRUE(fn != nullptr);
        EXPECT_EQ(fn(), expected_vm_ptr);
        EXPECT_TRUE(evmc_last_error_msg() == nullptr);
    }
}

TEST_F(loader, load_eee_bbb)
{
    setup("unittests/eee-bbb.dll", "evmc_create_eee_bbb", create_eee_bbb);
    evmc_loader_error_code ec;
    auto fn = evmc_load(evmc_test_library_path, &ec);
    const auto expected_vm_ptr = reinterpret_cast<evmc_vm*>(0xeeebbb);
    ASSERT_TRUE(fn != nullptr);
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ(fn(), expected_vm_ptr);
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
    auto paths = {
        "libaaa1.so",
        "eee2.so",
        "libeee3.x",
        "eee4",
        "_",
        "lib_.so",
        "unittests/double-prefix-aaa.evm",
        "unittests/double_prefix_aaa.evm",
    };

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
    EXPECT_EQ(ec, EVMC_LOADER_VM_CREATION_FAILURE);
    EXPECT_STREQ(evmc_last_error_msg(), "creating EVMC VM of failure.vm has failed");
    EXPECT_TRUE(evmc_last_error_msg() == nullptr);

    vm = evmc_load_and_create(evmc_test_library_path, nullptr);
    EXPECT_TRUE(vm == nullptr);
    EXPECT_STREQ(evmc_last_error_msg(), "creating EVMC VM of failure.vm has failed");
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

TEST_F(loader, load_and_configure_no_options)
{
    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path", &ec);
    EXPECT_TRUE(vm);
    EXPECT_TRUE(recorded_options.empty());
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);

    setup("path", "evmc_create", create_vm_barebone);

    vm = evmc_load_and_configure("path,", &ec);
    EXPECT_TRUE(vm);
    EXPECT_TRUE(recorded_options.empty());
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
}

TEST_F(loader, load_and_configure_single_option)
{
    supported_options["o"] = {"1"};
    supported_options["O"] = {"2"};

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,o=1", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{1});
    EXPECT_EQ(recorded_options[0].first, "o");
    EXPECT_EQ(recorded_options[0].second, "1");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);

    recorded_options.clear();
    vm = evmc_load_and_configure("path,O=2", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{1});
    EXPECT_EQ(recorded_options[0].first, "O");
    EXPECT_EQ(recorded_options[0].second, "2");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
}

TEST_F(loader, load_and_configure_uknown_option)
{
    supported_options["x"] = {"1"};

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,z=1", &ec);
    EXPECT_FALSE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{1});
    EXPECT_EQ(recorded_options[0].first, "z");
    EXPECT_EQ(recorded_options[0].second, "1");
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_NAME);
    EXPECT_STREQ(evmc_last_error_msg(), "vm_with_set_option (path): unknown option 'z'");
    EXPECT_EQ(destroy_count, create_count);

    recorded_options.clear();
    vm = evmc_load_and_configure("path,x=2,", &ec);
    EXPECT_FALSE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{1});
    EXPECT_EQ(recorded_options[0].first, "x");
    EXPECT_EQ(recorded_options[0].second, "2");
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_VALUE);
    EXPECT_STREQ(evmc_last_error_msg(),
                 "vm_with_set_option (path): unsupported value '2' for option 'x'");
    EXPECT_EQ(destroy_count, create_count);
}

TEST_F(loader, load_and_configure_multiple_options)
{
    supported_options["a"] = {"_a", "_c"};
    supported_options["b"] = {"_b1", "_b2"};
    supported_options["c"] = {"_c"};

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,a=_a,b=_b1,c=_c,b=_b2", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{4});
    EXPECT_EQ(recorded_options[0].first, "a");
    EXPECT_EQ(recorded_options[0].second, "_a");
    EXPECT_EQ(recorded_options[1].first, "b");
    EXPECT_EQ(recorded_options[1].second, "_b1");
    EXPECT_EQ(recorded_options[2].first, "c");
    EXPECT_EQ(recorded_options[2].second, "_c");
    EXPECT_EQ(recorded_options[3].first, "b");
    EXPECT_EQ(recorded_options[3].second, "_b2");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);

    recorded_options.clear();
    vm = evmc_load_and_configure("path,a=_a,b=_b2,a=_c,", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{3});
    EXPECT_EQ(recorded_options[0].first, "a");
    EXPECT_EQ(recorded_options[0].second, "_a");
    EXPECT_EQ(recorded_options[1].first, "b");
    EXPECT_EQ(recorded_options[1].second, "_b2");
    EXPECT_EQ(recorded_options[2].first, "a");
    EXPECT_EQ(recorded_options[2].second, "_c");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
}

TEST_F(loader, load_and_configure_uknown_option_in_sequence)
{
    supported_options["a"] = {"_a"};
    supported_options["b"] = {"_b"};
    supported_options["c"] = {"_c"};

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,a=_a,b=_b,c=_b,", &ec);
    EXPECT_FALSE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{3});
    EXPECT_EQ(recorded_options[0].first, "a");
    EXPECT_EQ(recorded_options[0].second, "_a");
    EXPECT_EQ(recorded_options[1].first, "b");
    EXPECT_EQ(recorded_options[1].second, "_b");
    EXPECT_EQ(recorded_options[2].first, "c");
    EXPECT_EQ(recorded_options[2].second, "_b");
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_VALUE);
    EXPECT_STREQ(evmc_last_error_msg(),
                 "vm_with_set_option (path): unsupported value '_b' for option 'c'");
    EXPECT_EQ(destroy_count, create_count);

    recorded_options.clear();
    vm = evmc_load_and_configure("path,a=_a,x=_b,c=_c", &ec);
    EXPECT_FALSE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{2});
    EXPECT_EQ(recorded_options[0].first, "a");
    EXPECT_EQ(recorded_options[0].second, "_a");
    EXPECT_EQ(recorded_options[1].first, "x");
    EXPECT_EQ(recorded_options[1].second, "_b");
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_NAME);
    EXPECT_STREQ(evmc_last_error_msg(), "vm_with_set_option (path): unknown option 'x'");
    EXPECT_EQ(destroy_count, create_count);
}

TEST_F(loader, load_and_configure_empty_values)
{
    supported_options["flag"] = {""};  // Empty value expected.
    supported_options["e"] = {""};     // Empty value expected.

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,flag,e=,flag=,e", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{4});
    EXPECT_EQ(recorded_options[0].first, "flag");
    EXPECT_EQ(recorded_options[0].second, "");
    EXPECT_EQ(recorded_options[1].first, "e");
    EXPECT_EQ(recorded_options[1].second, "");
    EXPECT_EQ(recorded_options[2].first, "flag");
    EXPECT_EQ(recorded_options[2].second, "");
    EXPECT_EQ(recorded_options[3].first, "e");
    EXPECT_EQ(recorded_options[3].second, "");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_EQ(create_count, 1);
    EXPECT_EQ(destroy_count, 0);
}

TEST_F(loader, load_and_configure_degenerated_names)
{
    supported_options[""] = {"", "xxx"};  // An option with empty name.

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,,,=,,=xxx", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{5});
    EXPECT_EQ(recorded_options[0].first, "");
    EXPECT_EQ(recorded_options[0].second, "");
    EXPECT_EQ(recorded_options[1].first, "");
    EXPECT_EQ(recorded_options[1].second, "");
    EXPECT_EQ(recorded_options[2].first, "");
    EXPECT_EQ(recorded_options[2].second, "");
    EXPECT_EQ(recorded_options[3].first, "");
    EXPECT_EQ(recorded_options[3].second, "");
    EXPECT_EQ(recorded_options[4].first, "");
    EXPECT_EQ(recorded_options[4].second, "xxx");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    evmc_destroy(vm);
    EXPECT_EQ(destroy_count, create_count);
}

TEST_F(loader, load_and_configure_comma_at_the_end)
{
    // The additional comma at the end of the configuration string is ignored.

    supported_options["x"] = {"x"};

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,x=x,", &ec);
    EXPECT_TRUE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{1});
    EXPECT_EQ(recorded_options[0].first, "x");
    EXPECT_EQ(recorded_options[0].second, "x");
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    evmc_destroy(vm);
    EXPECT_EQ(destroy_count, create_count);
}

TEST_F(loader, load_and_configure_vm_without_set_option)
{
    // Allow empty option and check the VM supporting no options still fails to accept it.
    supported_options[""] = {""};

    setup("path", "evmc_create", create_vm_barebone);

    evmc_loader_error_code ec;
    auto vm = evmc_load_and_configure("path,a=0,b=1", &ec);
    EXPECT_FALSE(vm);
    EXPECT_TRUE(recorded_options.empty());
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_NAME);
    EXPECT_STREQ(evmc_last_error_msg(), "vm_barebone (path) does not support any options");
    EXPECT_EQ(destroy_count, create_count);

    vm = evmc_load_and_configure("path,", &ec);
    EXPECT_TRUE(vm);
    EXPECT_TRUE(recorded_options.empty());
    EXPECT_EQ(ec, EVMC_LOADER_SUCCESS);
    EXPECT_FALSE(evmc_last_error_msg());
    evmc_destroy(vm);
    EXPECT_EQ(destroy_count, create_count);

    vm = evmc_load_and_configure("path,,", &ec);
    EXPECT_FALSE(vm);
    EXPECT_TRUE(recorded_options.empty());
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_NAME);
    EXPECT_STREQ(evmc_last_error_msg(), "vm_barebone (path) does not support any options");
    EXPECT_EQ(destroy_count, create_count);
}

TEST_F(loader, load_and_configure_config_too_long)
{
    setup("path", "evmc_create", create_vm_barebone);

    evmc_loader_error_code ec;
    auto config = std::string{"path,"};
    config.append(10000, 'x');
    auto vm = evmc_load_and_configure(config.c_str(), &ec);
    EXPECT_FALSE(vm);
    EXPECT_TRUE(recorded_options.empty());
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_ARGUMENT);
    EXPECT_STREQ(evmc_last_error_msg(),
                 "invalid argument: configuration is too long (maximum allowed length is 4096)");
    EXPECT_EQ(destroy_count, create_count);
}

TEST_F(loader, load_and_configure_error_not_wanted)
{
    setup("path", "evmc_create", create_vm_with_set_option);

    auto vm = evmc_load_and_configure("path,f=1", nullptr);
    EXPECT_FALSE(vm);
    ASSERT_EQ(recorded_options.size(), size_t{1});
    EXPECT_EQ(recorded_options[0].first, "f");
    EXPECT_EQ(recorded_options[0].second, "1");
    EXPECT_EQ(destroy_count, create_count);
    EXPECT_STREQ(evmc_last_error_msg(), "vm_with_set_option (path): unknown option 'f'");
    EXPECT_FALSE(evmc_last_error_msg());
}

TEST_F(loader, load_and_configure_unknown_set_option_error_code)
{
    // Enable "option name causing unknown error".
    supported_options[option_name_causing_unknown_error] = {""};

    setup("path", "evmc_create", create_vm_with_set_option);

    evmc_loader_error_code ec;
    const auto config_str = "path," + option_name_causing_unknown_error + "=1";
    auto vm = evmc_load_and_configure(config_str.c_str(), &ec);
    EXPECT_FALSE(vm);
    ASSERT_EQ(recorded_options.size(), 1u);
    EXPECT_EQ(recorded_options[0].first, option_name_causing_unknown_error);
    EXPECT_EQ(recorded_options[0].second, "1");
    EXPECT_EQ(ec, EVMC_LOADER_INVALID_OPTION_VALUE);
    EXPECT_EQ(evmc_last_error_msg(),
              "vm_with_set_option (path): unknown error when setting value '1' for option '" +
                  option_name_causing_unknown_error + "'");
    EXPECT_EQ(destroy_count, create_count);
}