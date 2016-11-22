# ICL: Independant C++ Library

ICL provides a set of (nearly) non interdependant utility classes that fill the missing standard C++ library features or wrap OS specific stuff.
The classes are portables and have been tested on Windows 7 and last versions of Ubuntu.

## Utility classes

### Zip / archive

This utility class provides only the decompressing feature.

### Database

Thin wrapper for SQLite database and CouchDB protocol.

### Network

These utility classes implement server/client and Websocket protocols. The server uses a dedicated thread. The Websocket is completely transparent.

### I/O

Byte manipulation and serialization targeted for protocol communication.

### JSEngine

C++ wrapper around the Duktape tiny Javascript Engine

### JSON

Reader/writer classes to encode and decode JSON text format.

### Protocol

Very simple HTTP encoding/decoding.

### Security

Base64 and Sha1 classes.

### Utilities

Various portable classes static functions:

  * Log
  * Unique ID management
  * Generic Value
  * DateTime formating
  * String manipulation
  * Disk, files and directories
  * OS specific wrappers



