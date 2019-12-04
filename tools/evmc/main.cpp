// EVMC: Ethereum Client-VM Connector API.
// Copyright 2019 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <CLI/CLI.hpp>

int main(int argc, const char** argv)
{
    CLI::App app{"EVMC tool"};
    const auto& version_flag = *app.add_flag("--version", "Print version information");

    try
    {
        app.parse(argc, argv);

        // Handle the --version flag first and exit when present.
        if (version_flag)
        {
            std::cout << "EVMC tool " PROJECT_VERSION "\n";
            return 0;
        }

        return 0;
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
    }
    catch (...)
    {
        return -1;
    }
}
