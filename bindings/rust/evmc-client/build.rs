// Copyright (C) 2020 Second State.
// This file is part of EVMC-Client.

// EVMC-Client is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// EVMC-Client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

use std::env;
use std::path::{Path, PathBuf};
use std::process::Command;
extern crate cmake;
use cmake::Config;

macro_rules! get(($name:expr) => (ok!(env::var($name))));
macro_rules! ok(($expression:expr) => ($expression.unwrap()));

const REPOSITORY: &'static str = "https://github.com/second-state/evmc.git";
const TAG: &'static str = "v7.4.0-rust-evmc-client-rc.2";

fn run<F>(name: &str, mut configure: F)
where
    F: FnMut(&mut Command) -> &mut Command,
{
    let mut command = Command::new(name);
    let configured = configure(&mut command);
    if !ok!(configured.status()).success() {
        panic!("failed to execute {:?}", configured);
    }
}

fn build_from_src() {
    let source = PathBuf::from(&get!("CARGO_MANIFEST_DIR")).join(format!("target/evmc-{}", TAG));
    if !Path::new(&source.join(".git")).exists() {
        run("git", |command| {
            command
                .arg("clone")
                .arg(format!("--branch={}", TAG))
                .arg("--recursive")
                .arg(REPOSITORY)
                .arg(&source)
        });
    }

    let dst = Config::new(source).build();
    let evmc_path = Path::new(&dst).join("build/lib/loader");
    println!("cargo:rustc-link-search=native={}", evmc_path.display());
    println!("cargo:rustc-link-lib=static=evmc-loader");
}

fn main() {
    build_from_src();
}
