// EVMC: Ethereum Client-VM Connector API
// Copyright 2021 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/tooling.hpp>

int main(int argc, const char* argv[])
{
    return evmc::tooling::main(argc, argv, "EVMC", PROJECT_VERSION);
}
