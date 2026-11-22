# CLOVIS

*A small Redis-like key/value database - named CLOVIS because I'm French.*

CLOVIS is a lightweight key/value database written in C++. The project aims to provide a simple database server with an event-driven architecture and an LSM-Tree-based storage engine.

## Features

- Key/value storage
- Insert, retrieve, update, and delete operations
- Event-driven client/server architecture
- Configurable response formats
- LSM-Tree storage
- Text, JSON, and XML responses
- Interactive client input powered by [replxx](https://github.com/AmokHuginnsson/replxx)
- Up/Down arrow navigation through command history

## Building and Running the Client

The interactive client uses **replxx** to provide line editing and command-history navigation. The library must be installed and available to your compiler and linker on both Windows and Linux.

The client source includes the replxx header:

```cpp
#include <replxx.hxx>
```

Link the client against the replxx library when building. For example, on Linux:

```bash
g++ client_replxx.cpp client_tcp.cpp -o clovis-client -lreplxx
```

Make sure the project headers and any other required source files (including the implementation of `Client_tcp`) are included in your build. On Windows, configure your build system or IDE to include replxx's headers and link its library for the selected architecture and configuration. See the [replxx repository](https://github.com/AmokHuginnsson/replxx) for installation and build instructions for your platform.

Start the CLOVIS server first, then run the client:

```bash
./clovis-client
```

On Windows, run the generated client executable from your terminal or IDE.

### Client command-line quality-of-life features

- Type commands normally and press **Enter** to send them to the server.
- Press **Up** to recall older commands and **Down** to move forward through the history.
- Use replxx's line-editing features while composing a command.
- Type `exit` to leave the client. End-of-input (for example, Ctrl-D on supported terminals) also exits the input loop.

## How to Make a Request

The following commands are supported:

| Command | Description |
|---|---|
| `INS` | Insert a new key/value pair |
| `GET` | Retrieve a value |
| `UPD` | Update an existing value |
| `DEL` | Delete a key/value pair |

### Syntax

```text
COMMAND key "value" [flag]
```

The value must be enclosed in double quotes.

Keys **cannot contain spaces**.

### Response Flags

| Flag | Description |
|---|---|
| `--text` | Plain text response (default) |
| `--json` | JSON response |
| `--xml` | XML response |

The flag is optional. If no flag is provided, `--text` is used.

### Examples

#### Insert

```text
> INS test "lorem ipsum"
>> Insert successful
```

#### Get

```text
> GET test
>> lorem ipsum
```

With JSON:

```text
> GET test --json
```

```json
{
  "status": "success",
  "message": "GET successful",
  "value": "lorem ipsum"
}
```

With XML:

```text
> GET test --xml
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<response>
  <status>success</status>
  <message>GET successful</message>
  <value>lorem ipsum</value>
</response>
```

#### Update

```text
> UPD test "muspi merol"
>> Update successful
```

#### Delete

```text
> DEL test
>> Delete successful
```

### Error Messages

CLOVIS can return the following errors:

```text
Value must be enclosed in quotes
Key already exists
Value cannot be empty
Insert failed
Invalid response format
GET failed
Key not found
Invalid command
```

## Project Goals and Backlog

- [x] Create a client and server using an event-driven architecture
- [x] Insert, retrieve, update, and delete data
- [x] Support configurable response formats
- [x] Implement an LSM-Tree storage system
- [x] Improve and refactor the query parser
- [x] Improve client input with replxx line editing
- [x] Add Up/Down navigation through the command history
- [ ] Add a configuration file to customize CLOVIS behavior
- [ ] Configure the default response format
- [ ] Configure whether `INS`, `UPD`, and `DEL` return a response
- [ ] Add more configuration options
- [ ] Add support for querying multiple keys at once
- [ ] Add a way to list available keys
- [ ] Add key pattern matching (similar to `LIKE`)
- [ ] Add query functions such as `length`, `sum`, etc.

## What's Next?

1. **Configuration**
   - Add a configuration file
   - Allow users to customize CLOVIS behavior
   - Configure response formats and command responses

2. **Advanced queries**
   - Retrieve multiple keys
   - List available keys
   - Add key pattern matching
   - Add basic functions such as `length` and `sum`

3. **Further client improvements**
   - Consider additional client-side quality-of-life features
   - Improve command handling and user interaction as needed

## Project Status

CLOVIS is currently a work in progress.

The basic client/server architecture, CRUD operations, configurable response formats, LSM-Tree storage system, and replxx-powered interactive command history are implemented. The next major focus is configuration and advanced query functionality.
