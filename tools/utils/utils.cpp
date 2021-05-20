// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

#include <evmc/helpers.h>
#include <tools/utils/utils.hpp>
#include <ostream>
#include <stdexcept>

namespace evmc
{
std::ostream& operator<<(std::ostream& os, evmc_status_code status_code)
{
    return os << evmc_status_code_to_string(status_code);
}

std::ostream& operator<<(std::ostream& os, evmc_revision rev)
{
    return os << evmc_revision_to_string(rev);
}
}  // namespace evmc
