// EVMC: Ethereum Client-VM Connector API.
// Copyright 2022 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.

extern crate bindgen;
extern crate cmake;

use cmake::Config;
use std::env;
use std::path::PathBuf;

fn main() {
    let dst = Config::new("../../../").build();

    println!("cargo:rustc-link-lib=static=evmc-loader");
    println!(
        "cargo:rustc-link-search=native={}/build/lib/loader",
        dst.display()
    );

    let bindings = bindgen::Builder::default()
        .header("loader.h")
        .generate_comments(false)
        // do not generate an empty enum for EVMC_ABI_VERSION
        .constified_enum("")
        // generate Rust enums for each evmc enum
        .rustified_enum("*")
        .allowlist_type("evmc_.*")
        .allowlist_function("evmc_.*")
        .blocklist_type("evmc_vm")
        // TODO: consider removing this
        .size_t_is_usize(true)
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Could not write bindings");
}
