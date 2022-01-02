# selog
## _Simple Extensible LOGging library written in C_

## Features
- Simple and extensible
- Thread safe
- Portable

## Testing
If You want to see how selog works, simply run `make run`

## Instructions
To add selog to Your project include _selog.h_ and compile _selog.c_

To initialize logging use function `void selog_setup_default(void)`
After the call use macros:
```
log_trace(...)
log_debug(...)
log_info(...)
log_warning(...)
log_error(...)
log_debug(...)
```
to print a message to desired loglevel
You can also use `int selog_logf()` or `int selog_vlogf()` directly

## Supported Languages
- C
- C++

## Supported compilers
- gcc	g++
- clang	clang++

## Supported Operating Systems
- GNU/Linux
- Windows
- BSD
- MacOS

## Information
- Conforms to C99 and up

## See also
- [GTEngine](https://github.com/michaelskyf/GTEngine) - project that uses selog [here](https://github.com/michaelskyf/GTEngine/tree/master/lib)
