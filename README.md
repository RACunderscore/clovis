# CLOVIS

*A small Redis-like key/value database — named CLOVIS because I'm French.*

CLOVIS is a lightweight key/value database written in C++. The project aims to provide a simple database server with an event-driven architecture and an LSM-Tree-based storage engine.

## Features

- Key/value storage
- Insert, retrieve, update, and delete operations
- Event-driven client/server architecture
- Configurable response formats
- LSM-Tree storage
- Text, JSON, and XML responses

## How to Make a Request

The following commands are supported:

| Command | Description |
|---|---|
| `INS` | Insert a new key/value pair |
| `GET` | Retrieve a value |
| `UPD` | Update an existing value |
| `DEL` | Delete a key/value pair |

### Syntax

| COMMAND key "value" [flag]

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
> INS test "lorem ipsum"
>> Insert successful

#### Get
> GET test
>> lorem ipsum

With JSON:
> GET test --json
```json 
{
   "status": "success",
   "message": "GET successful",
   "value": "lorem ipsum"
}
```

With XML:
> GET test --xml
```xml 
<?xml version="1.0" encoding="UTF-8"?>
<response>
  <status>success</status>
  <message>GET successful</message>
  <value>lorem ipsum</value>
</response>
```

#### Update
> UPD test "muspi merol"
>> Update successful

#### Delete
> DEL test
>> Delete successful

### Error Messages

CLOVIS can return the following errors:

    Value must be enclosed in quotes
    Key already exists
    Value cannot be empty
    Insert failed
    Invalid response format
    GET failed
    Key not found
    Invalid command

## Project Goals

- [x] Create a client and server using an event-driven architecture
- [x] Insert, retrieve, update, and delete data
- [x] Support configurable response formats
- [x] Implement an LSM-Tree storage system
- [ ] Add a configuration file to customize CLOVIS behavior
- [ ] Configure the default response format
- [ ] Configure whether `INS`, `UPD`, and `DEL` return a response
- [ ] Add more configuration options
- [ ] Add support for querying multiple keys at once
- [ ] Add a way to list available keys
- [ ] Add key pattern matching (similar to `LIKE`)
- [ ] Add query functions such as `length`, `sum`, etc.
- [ ] Improve and refactor the query parser

## What's Next?

The next steps for the project are:

1. **Improve the client**
   - Add more client-side functionality
   - Improve command handling and user interaction

2. **Refactor the query parser**
   - Simplify command parsing
   - Improve validation
   - Separate parsing from command execution
   - Make response-format handling cleaner

3. **Configuration**
   - Add a configuration file
   - Allow users to customize CLOVIS behavior
   - Configure response formats and command responses

4. **Advanced queries**
   - Retrieve multiple keys
   - List available keys
   - Add key pattern matching
   - Add basic functions such as `length` and `sum`

## Project Status

CLOVIS is currently a work in progress.

The basic client/server architecture, CRUD operations, configurable response formats, and LSM-Tree storage system are implemented. The next major focus is improving the query system, configuration, and client functionality.