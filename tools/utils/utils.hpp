// EVMC: Ethereum Client-VM Connector API.
// Copyright 2018-2020 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <evmc/evmc.h>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <system_error>

namespace evmc
{
/// Output stream operator for evmc_status_code.
std::ostream& operator<<(std::ostream& os, evmc_status_code status_code);

/// Output stream operator for EVM revision.
std::ostream& operator<<(std::ostream& os, evmc_revision revision);

}  // namespace evmc
