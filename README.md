# Chunky

Chunky is a minimal ECS system. It supports 64 components max, doesn't 
really support the systems part you have to bring that, and you also need to
handle things like multi-threading.

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
