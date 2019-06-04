/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

//! evmc-declare is an attribute-style procedural macro to be used for automatic generation of FFI
//! code for the EVMC API with minimal boilerplate.
//!
//! evmc-declare can be used by applying its attribute to any struct which implements the `EvmcVm`
//! trait, from the evmc-vm crate.
//!
//! The macro takes two or three arguments: a valid UTF-8 stylized VM name, a comma-separated list of
//! capabilities, and an optional custom version string. If only the name and capabilities are
//! passed, the version string will default to the crate version.
//!
//! # Example
//! ```
//! #[evmc_declare::evmc_declare_vm("This is an example VM name", "ewasm, evm", "1.2.3-custom")]
//! pub struct ExampleVM;
//!
//! impl evmc_vm::EvmcVm for ExampleVM {
//!     fn init() -> Self {
//!             ExampleVM {}
//!     }
//!
//!     fn execute(&self, code: &[u8], context: &evmc_vm::ExecutionContext) -> evmc_vm::ExecutionResult {
//!             evmc_vm::ExecutionResult::success(1337, None)
//!     }
//! }
//! ```

// Set a higher recursion limit because parsing certain token trees might fail with the default of 64.
#![recursion_limit = "128"]

extern crate proc_macro;

use heck::ShoutySnakeCase;
use heck::SnakeCase;
use proc_macro::TokenStream;
use quote::quote;
use syn::parse_macro_input;
use syn::spanned::Spanned;
use syn::AttributeArgs;
use syn::Ident;
use syn::IntSuffix;
use syn::ItemStruct;
use syn::Lit;
use syn::LitInt;
use syn::LitStr;
use syn::NestedMeta;

struct VMNameSet {
    type_name: String,
    name_allcaps: String,
    name_lowercase: String,
}

struct VMMetaData {
    capabilities: u32,
    // Not included in VMNameSet because it is parsed from the meta-item arguments.
    name_stylized: String,
    custom_version: Option<String>,
}

#[allow(dead_code)]
impl VMNameSet {
    fn new(ident: String) -> Self {
        let caps = ident.to_shouty_snake_case();
        let lowercase = ident
            .to_snake_case()
            .chars()
            .filter(|c| *c != '_')
            .collect();
        VMNameSet {
            type_name: ident,
            name_allcaps: caps,
            name_lowercase: lowercase,
        }
    }

    /// Return a reference to the struct name, as a string.
    fn get_type_name(&self) -> &String {
        &self.type_name
    }

    /// Return a reference to the name in shouty snake case.
    fn get_name_caps(&self) -> &String {
        &self.name_allcaps
    }

    /// Return a reference to the name in lowercase, with all underscores removed. (Used for
    /// symbols like evmc_create_vmname)
    fn get_name_lowercase(&self) -> &String {
        &self.name_lowercase
    }

    /// Get the struct's name as an explicit identifier to be interpolated with quote.
    fn get_type_as_ident(&self) -> Ident {
        Ident::new(&self.type_name, self.type_name.span())
    }

    /// Get the lowercase name appended with arbitrary text as an explicit ident.
    fn get_lowercase_as_ident_append(&self, suffix: &str) -> Ident {
        let concat = format!("{}{}", &self.name_lowercase, suffix);
        Ident::new(&concat, self.name_lowercase.span())
    }

    /// Get the lowercase name prepended with arbitrary text as an explicit ident.
    fn get_lowercase_as_ident_prepend(&self, prefix: &str) -> Ident {
        let concat = format!("{}{}", prefix, &self.name_lowercase);
        Ident::new(&concat, self.name_lowercase.span())
    }

    /// Get the lowercase name appended with arbitrary text as an explicit ident.
    fn get_caps_as_ident_append(&self, suffix: &str) -> Ident {
        let concat = format!("{}{}", &self.name_allcaps, suffix);
        Ident::new(&concat, self.name_allcaps.span())
    }
}

impl VMMetaData {
    fn new(args: AttributeArgs) -> Self {
        assert!(
            args.len() == 2 || args.len() == 3,
            "Incorrect number of arguments supplied"
        );

        let vm_name_meta = &args[0];
        let vm_capabilities_meta = &args[1];

        let vm_version_meta = if args.len() == 3 {
            Some(&args[2])
        } else {
            None
        };

        let vm_name_string = match vm_name_meta {
            NestedMeta::Literal(lit) => {
                if let Lit::Str(s) = lit {
                    s.value()
                } else {
                    panic!("Literal argument type mismatch")
                }
            }
            _ => panic!("Argument 1 must be a string literal"),
        };

        let vm_capabilities_string = match vm_capabilities_meta {
            NestedMeta::Literal(lit) => {
                if let Lit::Str(s) = lit {
                    s.value().to_string()
                } else {
                    panic!("Literal argument type mismatch")
                }
            }
            _ => panic!("Argument 2 must be a string literal"),
        };

        // Parse the individual capabilities out of the list and prepare a capabilities flagset.
        // Prune spaces and underscores here to make a clean comma-separated list.
        let capabilities_list_pruned: String = vm_capabilities_string
            .chars()
            .filter(|c| *c != '_' && *c != ' ')
            .collect();
        let capabilities_flags = {
            let mut ret: u32 = 0;
            for capability in capabilities_list_pruned.split(",") {
                match capability {
                    "ewasm" => ret |= 0x1 << 1,
                    "evm" => ret |= 0x1,
                    _ => panic!("Invalid capability specified."),
                }
            }
            ret
        };

        // If a custom version string was supplied, then include it in the metadata.
        let vm_version_string_optional: Option<String> = match vm_version_meta {
            Some(meta) => {
                if let NestedMeta::Literal(lit) = meta {
                    match lit {
                        Lit::Str(s) => Some(s.value().to_string()),
                        _ => panic!("Literal argument type mismatch"),
                    }
                } else {
                    panic!("Argument 3 must be a string literal")
                }
            }
            None => None,
        };

        VMMetaData {
            capabilities: capabilities_flags,
            name_stylized: vm_name_string,
            custom_version: vm_version_string_optional,
        }
    }

    fn get_capabilities(&self) -> u32 {
        self.capabilities
    }

    fn get_name_stylized(&self) -> &String {
        &self.name_stylized
    }

    fn get_custom_version(&self) -> &Option<String> {
        &self.custom_version
    }
}

#[proc_macro_attribute]
pub fn evmc_declare_vm(args: TokenStream, item: TokenStream) -> TokenStream {
    // First, try to parse the input token stream into an AST node representing a struct
    // declaration.
    let input: ItemStruct = parse_macro_input!(item as ItemStruct);

    // Extract the identifier of the struct from the AST node.
    let vm_type_name: String = input.ident.to_string();

    // Build the VM name set.
    let names = VMNameSet::new(vm_type_name);

    // Parse the arguments for the macro.
    let meta_args = parse_macro_input!(args as AttributeArgs);
    let vm_data = VMMetaData::new(meta_args);

    // Get all the tokens from the respective helpers.
    let static_data_tokens = build_static_data(&names, &vm_data);
    let capabilities_tokens = build_capabilities_fn(vm_data.get_capabilities());
    let create_tokens = build_create_fn(&names);
    let destroy_tokens = build_destroy_fn(&names);
    let execute_tokens = build_execute_fn(&names);

    let quoted = quote! {
        #input
        #static_data_tokens
        #capabilities_tokens
        #create_tokens
        #destroy_tokens
        #execute_tokens
    };

    quoted.into()
}

/// Generate tokens for the static data associated with an EVMC VM.
fn build_static_data(names: &VMNameSet, metadata: &VMMetaData) -> proc_macro2::TokenStream {
    // Stitch together the VM name and the suffix _NAME
    let static_name_ident = names.get_caps_as_ident_append("_NAME");
    let static_version_ident = names.get_caps_as_ident_append("_VERSION");

    // Turn the stylized VM name and version into string literals.
    let stylized_name_literal = LitStr::new(
        metadata.get_name_stylized().as_str(),
        metadata.get_name_stylized().as_str().span(),
    );

    // If the user supplied a custom version, use it here. Otherwise, default to crate version.
    let version_tokens = match metadata.get_custom_version() {
        Some(s) => {
            let lit = LitStr::new(s.as_str(), s.as_str().span());
            quote! {
                #lit
            }
        }
        None => quote! {
            env!("CARGO_PKG_VERSION")
        },
    };

    quote! {
        static #static_name_ident: &'static str = #stylized_name_literal;
        static #static_version_ident: &'static str = #version_tokens;
    }
}

/// Takes a capabilities flag and builds the evmc_get_capabilities callback.
fn build_capabilities_fn(capabilities: u32) -> proc_macro2::TokenStream {
    let capabilities_literal =
        LitInt::new(capabilities as u64, IntSuffix::U32, capabilities.span());

    quote! {
        extern "C" fn __evmc_get_capabilities(instance: *mut ::evmc_vm::ffi::evmc_instance) -> ::evmc_vm::ffi::evmc_capabilities_flagset {
            #capabilities_literal
        }
    }
}

/// Takes an identifier and struct definition, builds an evmc_create_* function for FFI.
fn build_create_fn(names: &VMNameSet) -> proc_macro2::TokenStream {
    let type_ident = names.get_type_as_ident();
    let fn_ident = names.get_lowercase_as_ident_prepend("evmc_create_");

    let static_version_ident = names.get_caps_as_ident_append("_VERSION");
    let static_name_ident = names.get_caps_as_ident_append("_NAME");

    quote! {
        #[no_mangle]
        extern "C" fn #fn_ident() -> *const ::evmc_vm::ffi::evmc_instance {
            let new_instance = ::evmc_vm::ffi::evmc_instance {
                abi_version: ::evmc_vm::ffi::EVMC_ABI_VERSION as i32,
                destroy: Some(__evmc_destroy),
                execute: Some(__evmc_execute),
                get_capabilities: Some(__evmc_get_capabilities),
                set_option: None,
                set_tracer: None,
                name: ::std::ffi::CString::new(#static_name_ident).expect("Failed to build VM name string").into_raw() as *const i8,
                version: ::std::ffi::CString::new(#static_version_ident).expect("Failed to build VM version string").into_raw() as *const i8,
            };

            unsafe {
                ::evmc_vm::EvmcContainer::into_ffi_pointer(Box::new(::evmc_vm::EvmcContainer::<#type_ident>::new(new_instance)))
            }
        }
    }
}

/// Builds a callback to dispose of the VM instance
fn build_destroy_fn(names: &VMNameSet) -> proc_macro2::TokenStream {
    let type_ident = names.get_type_as_ident();

    quote! {
        extern "C" fn __evmc_destroy(instance: *mut ::evmc_vm::ffi::evmc_instance) {
            unsafe {
                ::evmc_vm::EvmcContainer::<#type_ident>::from_ffi_pointer(instance);
            }
        }
    }
}

fn build_execute_fn(names: &VMNameSet) -> proc_macro2::TokenStream {
    let type_name_ident = names.get_type_as_ident();

    quote! {
        extern "C" fn __evmc_execute(
            instance: *mut ::evmc_vm::ffi::evmc_instance,
            context: *mut ::evmc_vm::ffi::evmc_context,
            rev: ::evmc_vm::ffi::evmc_revision,
            msg: *const ::evmc_vm::ffi::evmc_message,
            code: *const u8,
            code_size: usize
        ) -> ::evmc_vm::ffi::evmc_result
        {
            assert!(!msg.is_null());
            assert!(!context.is_null());
            assert!(!instance.is_null());
            assert!(!code.is_null());

            let execution_context = unsafe {
                ::evmc_vm::ExecutionContext::new(
                    msg.as_ref().expect("EVMC message is null"),
                    context.as_mut().expect("EVMC context is null")
                )
            };

            let code_ref: &[u8] = unsafe {
                ::std::slice::from_raw_parts(code, code_size)
            };

            let container = unsafe {
                ::evmc_vm::EvmcContainer::<#type_name_ident>::from_ffi_pointer(instance)
            };

            let result = container.execute(code_ref, &execution_context);

            unsafe {
                ::evmc_vm::EvmcContainer::into_ffi_pointer(container);
            }

            result.into()
        }
    }
}
