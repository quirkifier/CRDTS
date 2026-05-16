# CRDTS

This project is a CRDT-based key-value store written in C++. Right now the codebase contains the four required CRDTs from the manual and a working single-node TCP server for Phase 2.

## Implemented so far

- `GCOUNTER`
- `PNCOUNTER`
- `LWWREG`
- `ORSET`
- Single-node key-value store
- Concurrent client handling with one thread per client
- Text client protocol
- Basic CLI client
- CRDT unit tests and randomized property tests

## Supported commands

- `CREATE key type`
- `INC key n`
- `SET key value`
- `ADD key elem`
- `REMOVE key elem`
- `GET key`
- `\list`
- `\peers`
- `\sync`
- `QUIT`

Notes:

- `INC` works on `GCOUNTER` and `PNCOUNTER`
- negative increments are rejected for `GCOUNTER`
- `SET` works only on `LWWREG`
- `ADD` and `REMOVE` work only on `ORSET`
- `\peers` currently reports `no peers`
- `\sync` is currently a no-op

## Running the server

The server currently supports these command-line options:

```text
crdtkv --id NODE_ID --port PORT
```

Examples:

```text
crdtkv --id A --port 6001
crdtkv --id B --port 6002
```

If no arguments are provided, the defaults are:

- node id: `A`
- port: `6001`

## Running the CLI client

The CLI currently supports:

```text
crdtkv_cli [host] [port]
```

Examples:

```text
crdtkv_cli 127.0.0.1 6001
crdtkv_cli localhost 6002
```

If no arguments are provided, the defaults are:

- host: `127.0.0.1`
- port: `6001`

## Example session

```text
CREATE likes GCOUNTER
INC likes 1
INC likes 5
GET likes
CREATE name LWWREG
SET name Alice
GET name
CREATE tags ORSET
ADD tags red
ADD tags blue
GET tags
\list
QUIT
```

## Tests

The `tests/` directory contains:

- basic tests for counters
- basic tests for LWW register
- basic tests for OR-Set
- randomized CRDT property tests for commutativity, associativity, idempotence, and serialization round trips

## Current limitations

- no gossip protocol yet
- no second gossip port yet
- no CRC64 wire format yet
- no cluster convergence tests yet
- no persistence yet
- current networking code is Windows-oriented because it uses Winsock
