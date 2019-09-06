# EVM Storage Change Status {#storagestatus}

The description of ::evmc_storage_status enum design and its relation
to the specification in [EIP-2200] and others.

## Specification

> This is the [EIP-2200] specification with modifications:
> - the clause tree has been converted to ordered lists to be referenceable,
> - the cost constant names has been replaced with matching Yellow Paper names.

1. If *gasleft* is less than or equal to gas stipend, fail the current
   call frame with 'out of gas' exception.
2. If *current value* equals *new value* (this is a no-op), G<sub>warmaccess</sub>
   is deducted.
3. If *current value* does not equal *new value*
    1. If *original value* equals *current value* (this storage slot has
       not been changed by the current execution context)
        1. If *original value* is 0, G<sub>sset</sub> is deducted.
        2. Otherwise, G<sub>sreset</sub> gas is deducted.
            1. If *new value* is 0,
               add R<sub>sclear</sub> gas to refund counter.
    2. If *original value* does not equal *current value* (this storage
       slot is dirty), G<sub>warmaccess</sub> gas is deducted. Apply both of the
       following clauses.
        1. If *original value* is not 0
            1. If *current value* is 0 (also means that *new value* is not
               0), remove R<sub>sclear</sub> gas from refund
               counter.
            2. If *new value* is 0 (also means that *current value* is not
               0), add R<sub>sclear</sub> gas to refund counter.
        2. If *original value* equals *new value* (this storage slot is
           reset)
            1. If *original value* is 0, add G<sub>sset</sub> - G<sub>warmaccess</sub> to
               refund counter.
            2. Otherwise, add G<sub>sreset</sub> - G<sub>warmaccess</sub> gas to refund
               counter.

### Cost constants

| Yellow Paper           | EIP-2200                  | EIP-2200 Value | EIP-2929 Value |
|------------------------|---------------------------|----------------|----------------|
| G<sub>warmaccess</sub> | `SLOAD_GAS`               | 800            | 100            |
| G<sub>sset</sub>       | `SSTORE_SET_GAS`          | 20000          | 20000          |
| G<sub>sreset</sub>     | `SSTORE_RESET_GAS`        | 5000           | 2900           |
| R<sub>sclear</sub>     | `SSTORE_CLEARS_SCHEDULE`  | 15000          | 15000          |

## Storage change statuses

- `0` - zero value
- `X` - non-zero value
- `Y` - non-zero value different from `X`
- `Z` - non-zero value different form `X` and `Y`
- `o` - original value
- `c` - current value
- `v` - new value

<table>
    <tr>
        <th>name</th>
        <th>o</th>
        <th>c</th>
        <th>v</th>
        <th>dirty</th>
        <th>restored</th>
        <th>clause</th>
        <th>gas cost</th>
        <th>gas refund</th>
    </tr>
    <tr>
        <td rowspan="7">[ASSIGNED]</td>
        <td>0</td><td>0</td><td>0</td>
        <td>no</td>
        <td>yes</td>
        <td>2</td>
        <td rowspan="7">G<sub>warmaccess</sub></td>
        <td rowspan="7">0</td>
    </tr>
    <tr>
        <td>X</td><td>0</td><td>0</td>
        <td>yes</td>
        <td>no</td>
        <td>2</td>
    </tr>
    <tr>
        <td>0</td><td>Y</td><td>Y</td>
        <td>yes</td>
        <td>no</td>
        <td>2</td>
    </tr>
    <tr>
        <td>X</td><td>Y</td><td>Y</td>
        <td>yes</td>
        <td>no</td>
        <td>2</td>
    </tr>
    <tr>
        <td>Y</td><td>Y</td><td>Y</td>
        <td>no</td>
        <td>yes</td>
        <td>2</td>
    </tr>
    <tr>
        <td>0</td><td>Y</td><td>Z</td>
        <td>yes</td>
        <td>no</td>
        <td>3.2</td>
    </tr>
    <tr>
        <td>X</td><td>Y</td><td>Z</td>
        <td>yes</td>
        <td>no</td>
        <td>3.2</td>
    </tr>
    <tr>
        <td>[ADDED]</td>
        <td>0</td><td>0</td><td>Z</td>
        <td>no</td>
        <td>no</td>
        <td>3.1.1</td>
        <td>G<sub>sset</sub></td>
        <td>0</td>
    </tr>
    <tr>
        <td>[DELETED]</td>
        <td>X</td><td>X</td><td>0</td>
        <td>no</td>
        <td>no</td>
        <td>3.1.2.1</td>
        <td>G<sub>sreset</sub></td>
        <td>R<sub>sclear</sub></td>
    </tr>
    <tr>
        <td>[MODIFIED]</td>
        <td>X</td><td>X</td><td>Z</td>
        <td>no</td>
        <td>no</td>
        <td>3.1.2</td>
        <td>G<sub>sreset</sub></td>
        <td>0</td>
    </tr>
    <tr>
        <td>[DELETED_ADDED]</td>
        <td>X</td><td>0</td><td>Z</td>
        <td>yes</td>
        <td>no</td>
        <td>3.2.1.1</td>
        <td>G<sub>warmaccess</sub></td>
        <td>-R<sub>sclear</sub></td>
    </tr>
    <tr>
        <td>[MODIFIED_DELETED]</td>
        <td>X</td><td>Y</td><td>0</td>
        <td>yes</td>
        <td>no</td>
        <td>3.2.1.2</td>
        <td>G<sub>warmaccess</sub></td>
        <td>R<sub>sclear</sub></td>
    </tr>
    <tr>
        <td>[DELETED_RESTORED]</td>
        <td>X</td><td>0</td><td>X</td>
        <td>yes</td>
        <td>yes</td>
        <td>3.2.1.1 + 3.2.2.2</td>
        <td>G<sub>warmaccess</sub></td>
        <td>-R<sub>sclear</sub> + G<sub>sreset</sub> - G<sub>warmaccess</sub></td>
    </tr>
    <tr>
        <td>[ADDED_DELETED]</td>
        <td>0</td><td>Y</td><td>0</td>
        <td>yes</td>
        <td>yes</td>
        <td>3.2.2.1</td>
        <td>G<sub>warmaccess</sub></td>
        <td>G<sub>sset</sub> - G<sub>warmaccess</sub></td>
    </tr>
    <tr>
        <td>[MODIFIED_RESTORED]</td>
        <td>X</td><td>Y</td><td>X</td>
        <td>yes</td>
        <td>yes</td>
        <td>3.2.2.2</td>
        <td>G<sub>warmaccess</sub></td>
        <td>G<sub>sreset</sub> - G<sub>warmaccess</sub></td>
    </tr>
</table>


## Efficient implementation

All distinctive storage change statuses can be unambiguously selected
by combination of the 4 following checks:
- **o ≠ c**, i.e. `original != current` (dirty),
- **o = v**, i.e `original == new` (restored),
- **c ≠ 0**, i.e `current != 0`,
- **v ≠ 0**, i.e `new != 0`.

<table>
    <tr>
        <th>name</th>
        <th>o</th>
        <th>c</th>
        <th>v</th>
        <th>o ≠ c</th>
        <th>o = v</th>
        <th>c ≠ 0</th>
        <th>v ≠ 0</th>
        <th>checksum</th>
        <th>proof</th>
    </tr>
    <tr>
        <td rowspan="7">[ASSIGNED]</td>
        <td>0</td><td>0</td><td>0</td>
        <td>0</td><td>1</td><td>0</td><td>0</td>
        <td>4 (0b0100)</td>
    </tr>
    <tr>
        <td>X</td><td>0</td><td>0</td>
        <td>1</td><td>0</td><td>0</td><td>0</td>
        <td>8 (0b1000)</td>
    </tr>
    <tr>
        <td>0</td><td>Y</td><td>Y</td>
        <td>1</td><td>0</td><td>1</td><td>1</td>
        <td>11 (0b1011)</td>
    </tr>
    <tr>
        <td>X</td><td>Y</td><td>Y</td>
        <td>1</td><td>0</td><td>1</td><td>1</td>
        <td>11 (0b1011)</td>
    </tr>
    <tr>
        <td>Y</td><td>Y</td><td>Y</td>
        <td>0</td><td>1</td><td>1</td><td>1</td>
        <td>7 (0b0111)</td>
    </tr>
    <tr>
        <td>0</td><td>Y</td><td>Z</td>
        <td>1</td><td>0</td><td>1</td><td>1</td>
        <td>11 (0b1011)</td>
    </tr>
    <tr>
        <td>X</td><td>Y</td><td>Z</td>
        <td>1</td><td>0</td><td>1</td><td>1</td>
        <td>11 (0b1011)</td>
    </tr>
    <tr>
        <td>[ADDED]</td>
        <td>0</td><td>0</td><td>Z</td>
        <td>0</td><td>0</td><td>0</td><td>1</td>
        <td>1 (0b0001)</td>
    </tr>
    <tr>
        <td>[DELETED]</td>
        <td>X</td><td>X</td><td>0</td>
        <td>0</td><td>0</td><td>1</td><td>0</td>
        <td>2 (0b0010)</td>
    </tr>
    <tr>
        <td>[MODIFIED]</td>
        <td>X</td><td>X</td><td>Z</td>
        <td>0</td><td>0</td><td>1</td><td>1</td>
        <td>3 (0b0011)</td>
    </tr>
    <tr>
        <td>[DELETED_ADDED]</td>
        <td>X</td><td>0</td><td>Z</td>
        <td>1</td><td>0</td><td>0</td><td>1</td>
        <td>9 (0b1001)</td>
    </tr>
    <tr>
        <td>[MODIFIED_DELETED]</td>
        <td>X</td><td>Y</td><td>0</td>
        <td>1</td><td>0</td><td>1</td><td>0</td>
        <td>10 (0b1010)</td>
    </tr>
    <tr>
        <td>[DELETED_RESTORED]</td>
        <td>X</td><td>0</td><td>X</td>
        <td>1</td><td>1</td><td>0</td><td>1</td>
        <td>13 (0b1101)</td>
    </tr>
    <tr>
        <td>[ADDED_DELETED]</td>
        <td>0</td><td>Y</td><td>0</td>
        <td>1</td><td>1</td><td>1</td><td>0</td>
        <td>14 (0b1110)</td>
    </tr>
    <tr>
        <td>[MODIFIED_RESTORED]</td>
        <td>X</td><td>Y</td><td>X</td>
        <td>1</td><td>1</td><td>1</td><td>1</td>
        <td>15 (0b1111)</td>
    </tr>
    <tr>
        <td rowspan="4">impossible</td>
        <td></td><td></td><td></td>
        <td>0</td><td>0</td><td>0</td><td>0</td>
        <td>0 (0b0000)</td>
        <td>o=c ∧ o≠v ∧ c=0 ⇒ v≠0</td>
    </tr>
    <tr>
        <td></td><td></td><td></td>
        <td>0</td><td>1</td><td>0</td><td>1</td>
        <td>5 (0b0101)</td>
        <td>o=c ∧ o=v ∧ c=0 ⇒ v=0</td>
    </tr>
    <tr>
        <td></td><td></td><td></td>
        <td>0</td><td>1</td><td>1</td><td>0</td>
        <td>6 (0b0110)</td>
        <td>o=c ∧ o=v ∧ c≠0 ⇒ v≠0</td>
    </tr>
    <tr>
        <td></td><td></td><td></td>
        <td>1</td><td>1</td><td>0</td><td>0</td>
        <td>12 (0b1100)</td>
        <td>o≠c ∧ o=v ∧ c=0 ⇒ v≠0</td>
    </tr>
</table>


[EIP-2200]: https://eips.ethereum.org/EIPS/eip-2200
[ASSIGNED]: @ref EVMC_STORAGE_ASSIGNED
[ADDED]: @ref EVMC_STORAGE_ADDED
[DELETED]: @ref EVMC_STORAGE_DELETED
[MODIFIED]: @ref EVMC_STORAGE_MODIFIED
[DELETED_ADDED]: @ref EVMC_STORAGE_DELETED_ADDED
[MODIFIED_DELETED]: @ref EVMC_STORAGE_MODIFIED_DELETED
[DELETED_RESTORED]: @ref EVMC_STORAGE_DELETED_RESTORED
[ADDED_DELETED]: @ref EVMC_STORAGE_ADDED_DELETED
[MODIFIED_RESTORED]: @ref EVMC_STORAGE_MODIFIED_RESTORED
