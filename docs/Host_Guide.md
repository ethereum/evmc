# EVMC Host Implementation Guide {#hostguide}

> How to bring EVMC support to Your Ethereum Client.

## Host interface

First of all, you have to implement the Host interface. The Host interface
allows VMs to query and modify Ethereum state during the execution.

The implementation can be done in object-oriented manner. 
The ::evmc_host_interface lists the methods any Host must implement.

Moreover, each of the methods has a pointer to ::evmc_host_context 
as a parameter. The context is owned entirely by the Host allowing a Host instance 
to behave as an object with data.

## VM usage

When Host implementation is ready it's time to start using EVMC VMs.

1. Firstly, create a VM instance. You need to know what is the name of the "create"
   function in particular VM implementation. The EVMC recommends to name the 
   function by the VM codename, e.g. ::evmc_create_example_vm().
   Invoking the create function will give you the VM instance (::evmc_vm). 
   It is recommended to create the VM instance once.
   
2. If you are interested in loading VMs dynamically (i.e. to use DLLs) 
   check out the [EVMC Loader](@ref loader) library.
   
3. The ::evmc_vm contains information about the VM like 
   name (::evmc_vm::name) or ABI version (::evmc_vm::abi_version)
   and methods.
   
4. To execute code in the VM use the "execute()" method (::evmc_vm::execute).
   You will need:
   - the code to execute,
   - the message (::evmc_message) object that describes the execution context,
   - the Host instance, passed as ::evmc_host_context pointer.
   
5. When execution finishes you will receive ::evmc_result object that describes
   the results of the execution.
   
Have fun!