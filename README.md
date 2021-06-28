# Chunky

Chunky is a minimal ECS system. It supports 64 components max, doesn't 
really support the systems part you have to bring that, and you also need to
handle things like multi-threading.

The main motivation for this was to see what an interface would turn out like
using bitmasks to identify the components for an entity or system.

## Roadmap/Feedback

This is for my own needs/fun, but if you have feedback on how what could be
changed to meet your needs, feel free to raise an issue. I do this in my free
time so its ofcourse dependent on how much time I have.

**Some Unordered Pirority List**

- Stablise the API / Basic design.
- Make API safer to use more thread friendly.
- Virutal Memory backend.

## Status

Currently porting a really bad prototype I made for this accross. There are alot
of hard edges that I want to smooth out, but atm its 'get it working' kinda
thing, so very unstable atm.

## Development Environment

There isn't anything special going on here, you just need to compile the source
files, with a path set to the include folder.

We use cmake its pretty vanilla but likely not setup in a way you can 'embed'
into your own project, but its all pretty vanilla, just compile the files.

```bash
git clone https://github.com/PhilCK/Chunky.git
git submodule update --init --recursive
cmake -B ./build
cd build
make # or what ever generator
```

There is a test app `./bin/imgui` its pretty basic but does the job.
There are also unit tests ... likely should be more than there is.

## Thirdparty

The thirdparty is used in development and testing. It does not ship with Chunky.

Library | URL                                  
--------|--------------------------------------
UTest   | https://github.com/sheredom/utest.h  
UBench  | https://github.com/sheredom/ubench.h 
Imgui   | https://github.com/ocornut/imgui     
CImgui  | https://github.com/cimgui/cimgui     
CImgui-sokol-starter | https://github.com/floooh/cimgui-sokol-starterkit
