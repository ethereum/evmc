# EVMC – Ethereum Client-VM Connector API {#mainpage}

The EVMC is the low-level ABI between Ethereum Virtual Machines (EVMs) and 
Ethereum Clients. On the EVM-side it supports classic EVM1 and [eWASM].
On the Client-side it defines the interface for EVM implementations 
to access Ethereum environment and state.


## Modules

- [EVMC](@ref EVMC) 
   – the main component that defines API for VMs and Clients (Hosts).
- [EVMC Loader](@ref loader)
   – the library for loading VMs implemented as Dynamically Loaded Libraries (DLLs, shared objects). 
- [EVMC Helpers](@ref helpers)
   – a collection of utility functions for easier integration with EVMC.
- [EVM Instructions](@ref instructions) 
   – the library with collection of metrics for EVM1 instruction set.
- [EMVC VM Tester](@ref vmtester)
   – the EVMC-compatibility testing tool for VM implementations.


[eWASM]: https://github.com/ewasm/design


@addtogroup EVMC

## Terms

1. **VM** – An Ethereum Virtual Machine instance/implementation.
2. **Host** – An entity controlling the VM. 
   The Host requests code execution and responses to VM queries by callback 
   functions. This usually represents an Ethereum Client.
   

## Responsibilities

### VM

- Executes the code (obviously).
- Calculates the running gas cost and manages the gas counter except the refund
  counter.
- Controls the call depth, including the exceptional termination of execution
  in case the maximum depth is reached.
  
  
### Host

- Provides access to State.
- Creates new accounts (with code being a result of VM execution).
- Handles refunds entirely.
- Manages the set of precompiled contracts and handles execution of messages
  coming to them.
