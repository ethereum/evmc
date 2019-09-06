# Storage change status

> Analysis of possible storage change statuses in [EIP-2200] / [EIP-1283].

## Cost constants

| Constant | Value |
| -------- | ----- |
| SLOAD    | 200   |
| SET      | 20000 |
| RESET    | 5000  |
| CLEARS   | 15000 |


## Storage change statuses

- `0` - a zero storage value,
- `X` - non-zero storage value,
- `Y` - non-zero storage value different from `X`,
- `Z` - non-zero storage value different form `X` and `Y`,
- `...` - any value.


<table>
    <thead>
        <tr>
            <th>original</th>
            <th>current</th>
            <th>new</th>
            <th>name</th>
            <th>dirty?</th>
            <th>cost</th>
            <th>refund</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>...</td>
            <td>0</td>
            <td>0</td>
            <td rowspan="2">UNCHANGED</td>
            <td rowspan="2">N/A</td>
            <td rowspan="2">200 = SLOAD</td>
            <td rowspan="2">0</td>
        </tr>
        <tr>
            <td>...</td>
            <td>X</td>
            <td>X</td>
        </tr>
        <tr>
            <td>0</td>
            <td>0</td>
            <td>X</td>
            <td>SET / ADDED</td>
            <td>no</td>
            <td>20000 = SET</td>
            <td>0</td>
        </tr>
        <tr>
            <td>X</td>
            <td>X</td>
            <td>Y</td>
            <td>RESET / MODIFIED</td>
            <td>no</td>
            <td>5000 = RESET</td>
            <td>0</td>
        </tr>
        <tr>
            <td>X</td>
            <td>X</td>
            <td>0</td>
            <td>CLEARED / DELETED</td>
            <td>no</td>
            <td>5000 = RESET</td>
            <td>15000 = CLEARS</td>
        </tr>
        <tr>
            <td>0</td>
            <td>X</td>
            <td>0</td>
            <td>DIRTY_SET_TO_CLEARED</td>
            <td>yes</td>
            <td>200 = SLOAD</td>
            <td>19800 = SET - SLOAD</td>
        </tr>
        <tr>
            <td>X</td>
            <td>0</td>
            <td>X</td>
            <td rowspan="2">DIRTY_CLEARED_TO_SET</td>
            <td rowspan="2">yes</td>
            <td rowspan="2">200 = SLOAD</td>
            <td rowspan="2">-15000 = -CLEARS</td>
        </tr>
        <tr>
            <td>X</td>
            <td>0</td>
            <td>Y</td>
        </tr>
        <tr>
            <td>0</td>
            <td>X</td>
            <td>Y</td>
            <td rowspan="2">DIRTY_RESET_AGAIN</td>
            <td rowspan="2">yes</td>
            <td rowspan="2">200 = SLOAD</td>
            <td rowspan="2">0</td>
        </tr>
        <tr>
            <td>X</td>
            <td>Y</td>
            <td>Z</td>
        </tr>
        <tr>
            <td>X</td>
            <td>Y</td>
            <td>0</td>
            <td>DIRTY_RESET_TO_CLEARED</td>
            <td>yes</td>
            <td>200 = SLOAD</td>
            <td>15000 = CLEARS</td>
        </tr>
        <tr>
            <td>X</td>
            <td>Y</td>
            <td>X</td>
            <td>DIRTY_RESET_REVERTED</td>
            <td>yes</td>
            <td>200 = SLOAD</td>
            <td>4800 = RESET - SLOAD</td>
        </tr>
    </tbody>
</table>

[EIP-1283]: https://github.com/ethereum/EIPs/blob/master/EIPS/eip-1283.md
[EIP-2200]: https://github.com/ethereum/EIPs/blob/master/EIPS/eip-2200.md
