# selog
## _Simple Extensible LOGging library written in C_

![C CI](https://github.com/michaelskyf/selog/actions/workflows/c.yml/badge.svg)
![C++ CI](https://github.com/michaelskyf/selog/actions/workflows/cpp.yml/badge.svg)
![Documentation CI](https://github.com/michaelskyf/selog/actions/workflows/docs.yml/badge.svg)

## Features
- Simple and extensible
- Thread safe
- Portable

## Testing
If You want to see how selog works, simply run `make run`

## Instructions
To initialize logging use function `void selog_setup_default(void)`

After the call use these macros to print a message to desired loglevel:
```
log_trace(...)
log_debug(...)
log_info(...)
log_warning(...)
log_error(...)
log_debug(...)
```

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
- Conforms to C++11 and up

## License
[GPLv3](https://www.gnu.org/licenses/gpl-3.0.html)

## See also
- [GTEngine](https://github.com/michaelskyf/GTEngine) - project that uses selog [here](https://github.com/michaelskyf/GTEngine/tree/master/lib)
