# Chunky

Chunky is a minimal ECS system. It supports 64 components max, doesn't 
really support the systems part you have to bring that, and you also need to
handle things like multi-threading.

The main motivation for this was to see what an interface would turn out like
using bitmasks to identify the components for an entity or system.

## Status

Currently porting a really bad prototype I made for this accross. There are alot
of hard edges that I want to smooth out, but atm its 'get it working' kinda
thing, so very unstable atm.

## Development Environment

There isn't anything special going on here, you just need to compile the source
files, with a path set to the include folder.

You can use the scripts in the scripts folder like so.

```bash
git clone https://github.com/PhilCK/Chunky.git
git submodule update --init --recursive
cd scripts
./win_build_and_go.bat
```

## Thirdparty

The thirdparty is used in development and testing. It does not ship with Chunky.

Library | URL                                  | Author
--------|--------------------------------------|-----------------------------
UTest   | https://github.com/sheredom/utest.h  | https://twitter.com/sheredom
UBench  | https://github.com/sheredom/ubench.h | https://twitter.com/sheredom
