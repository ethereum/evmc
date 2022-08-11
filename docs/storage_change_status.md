# Storage change status

> Analysis of possible storage change statuses in [EIP-2200].

## Specification

1. If *gasleft* is less than or equal to gas stipend, fail the current
   call frame with 'out of gas' exception.
2. If *current value* equals *new value* (this is a no-op), `SLOAD_GAS`
   is deducted.
3. If *current value* does not equal *new value*
    1. If *original value* equals *current value* (this storage slot has
       not been changed by the current execution context)
        1. If *original value* is 0, `SSTORE_SET_GAS` is deducted.
        2. Otherwise, `SSTORE_RESET_GAS` gas is deducted.
            1. If *new value* is 0,
               add `SSTORE_CLEARS_SCHEDULE` gas to refund counter.
    2. If *original value* does not equal *current value* (this storage
       slot is dirty), `SLOAD_GAS` gas is deducted. Apply both of the
       following clauses.
        1. If *original value* is not 0
            1. If *current value* is 0 (also means that *new value* is not
               0), remove `SSTORE_CLEARS_SCHEDULE` gas from refund
               counter.
            2. If *new value* is 0 (also means that *current value* is not
               0), add `SSTORE_CLEARS_SCHEDULE` gas to refund counter.
        2. If *original value* equals *new value* (this storage slot is
           reset)
            1. If *original value* is 0, add `SSTORE_SET_GAS - SLOAD_GAS` to
               refund counter.
            2. Otherwise, add `SSTORE_RESET_GAS - SLOAD_GAS` gas to refund
               counter.

## Cost constants

| Constant | Value |
| -------- | ----- |
| SLOAD    | 800   |
| SET      | 20000 |
| RESET    | 5000  |
| CLEARS   | 15000 |

## Storage change statuses

- `0` - a zero storage value,
- `X` - non-zero storage value,
- `Y` - non-zero storage value different from `X`,
- `Z` - non-zero storage value different form `X` and `Y`,

<table>
    <thead>
        <tr>
            <th>name</th>
            <th>o</th>
            <th>c</th>
            <th>v</th>
            <th>dirty?</th>
            <th>clause</th>
            <th>gas cost</th>
            <th>gas refund</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan="7">NOOP</td>
            <td>0</td><td>0</td><td>0</td>
            <td>no</td>
            <td>2</td>
            <td rowspan="7">800 = SLOAD</td>
            <td rowspan="7">0</td>
        </tr>
        <tr>
            <td>X</td><td>0</td><td>0</td>
            <td>yes</td>
            <td>2</td>
        </tr>
        <tr>
            <td>0</td><td>Y</td><td>Y</td>
            <td>yes</td>
            <td>2</td>
        </tr>
        <tr>
            <td>X</td><td>Y</td><td>Y</td>
            <td>yes</td>
            <td>2</td>
        </tr>
        <tr>
            <td>Y</td><td>Y</td><td>Y</td>
            <td>no</td>
            <td>2</td>
        </tr>
        <tr>
            <td>0</td><td>Y</td><td>Z</td>
            <td>yes</td>
            <td>3.2</td>
        </tr>
        <tr>
            <td>X</td><td>Y</td><td>Z</td>
            <td>yes</td>
            <td>3.2</td>
        </tr>
        <tr>
            <td>ADDED</td>
            <td>0</td><td>0</td><td>Z</td>
            <td>no</td>
            <td>3.1.1</td>
            <td>20000 = SET</td>
            <td>0</td>
        </tr>
        <tr>
            <td>DELETED</td>
            <td>X</td><td>X</td><td>0</td>
            <td>no</td>
            <td>3.1.2.1</td>
            <td>5000 = RESET</td>
            <td>15000 = CLEARS</td>
        </tr>
        <tr>
            <td>MODIFIED</td>
            <td>X</td><td>X</td><td>Z</td>
            <td>no</td>
            <td>3.1.2</td>
            <td>5000 = RESET</td>
            <td>0</td>
        </tr>
        <tr>
            <td>deleted added</td>
            <td>X</td><td>0</td><td>Z</td>
            <td>yes</td>
            <td>3.2.1.1</td>
            <td>800 = SLOAD</td>
            <td>-15000 = -CLEARS</td>
        </tr>
        <tr>
            <td>MODIFIED_DELETED</td>
            <td>X</td><td>Y</td><td>0</td>
            <td>yes</td>
            <td>3.2.1.2</td>
            <td>800 = SLOAD</td>
            <td>15000 = CLEARS</td>
        </tr>
        <tr>
            <td>DELETED_RESTORED</td>
            <td>X</td><td>0</td><td>X</td>
            <td>yes</td>
            <td>3.2.1.1 + 3.2.2.2</td>
            <td>800 = SLOAD</td>
            <td>-15000 = -CLEARS</td>
        </tr>
        <tr>
            <td>ADDED_DELETED</td>
            <td>0</td><td>Y</td><td>0</td>
            <td>yes</td>
            <td>3.2.2.1</td>
            <td>800 = SLOAD</td>
            <td>19200 = SET - SLOAD</td>
        </tr>
        <tr>
            <td>MODIFIED_RESTORED</td>
            <td>X</td><td>Y</td><td>X</td>
            <td>yes</td>
            <td>3.2.2.2</td>
            <td>800 = SLOAD</td>
            <td>4200 = RESET - SLOAD</td>
        </tr>
    </tbody>
</table>

[EIP-2200]: https://eips.ethereum.org/EIPS/eip-2200
