// Copyright (C) 2020 Second State.
// This file is part of Rust-SSVM.

// Rust-SSVM is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Rust-SSVM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

pub use evmc_sys as ffi;

pub const ADDRESS_LENGTH: usize = 20;
pub const BYTES32_LENGTH: usize = 32;
pub type Address = [u8; ADDRESS_LENGTH];
pub type Bytes32 = [u8; BYTES32_LENGTH];
pub type Bytes = [u8];

pub type StorageStatus = ffi::evmc_storage_status;
pub type CallKind = ffi::evmc_call_kind;
pub type Revision = ffi::evmc_revision;
pub type StatusCode = ffi::evmc_status_code;
pub type Capabilities = ffi::evmc_capabilities;
