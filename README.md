# Junk Engine

> **Warning: This is literally junk.**
> A Vulkan rendering engine written in C99 because why not.

## What is this garbage?

Junk Engine is a personal experiment in building a Vulkan-based rendering engine from scratch using C99. It's probably full of memory leaks, questionable design & problematic code.

## Target Platform

Designed to run on anything that vaguely pretends to understand Vulkan, which is basically every GPU made in the last decade. Specifically:

- **Vulkan 1.0–1.2**: No fancy cutting-edge stuff here. It's not trying to impress anyone.
- **VRAM**: Capped at ~1.5GB on purpose. This junk is designed to be lightweight and chaotic.
- **OS Support**:
  - Linux (Arch & Ubuntu tested)
  - Windows (good luck, seriously)
  - Mac? You Wish....
- **GPU**: Any Vulkan-capable GPU should technically work. Older GPUs might crash spectacularly or just quietly refuse to render anything. Either way, entertainment guaranteed.

**Pro tip:** If your system doesn’t explode immediately, consider it a minor victory.

## Requirements (Because Nothing Works Without This Stuff)

Before you dive into this garbage fire, make sure you have **all the junk** installed:

1. **Bear** – Not the best but it simple enough. It makes your life slightly less painful by generating compilation databases.
   - Arch Linux: `sudo pacman -S bear`. 
   - Ubuntu: `sudo apt install bear`.
   - Windows: [Grab it here, if you dare](https://github.com/rizsotto/Bear).

2. **Make / Makefiles** – Because apparently chaos needs rules.
   - Arch Linux: `sudo pacman -S make`.
   - Ubuntu: `sudo apt install build-essential`. 
   - Windows: Get [GNU Make](http://gnuwin32.sourceforge.net/packages/make.htm) or use MSYS2 if you like suffering.

3. **Vulkan SDK & Vulkan loader library** – So we can make the GPU sweat.
   - Linux:
     - Arch Linux: `sudo pacman -S vulkan-devel`.
     - Ubuntu: `sudo apt install vulkan-tools vulkan-utils` - i don't know the exact packages for Ubuntu. 
   - Windows: [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home) – download and pray.

4. **C Compiler** – Any modern C99 compiler should do. GCC, Clang, or something.
   - Arch Linux: `sudo pacman -S gcc` `sudo pacman -S clang` 
   - Ubuntu: `sudo apt install gcc` `sudo apt install clang` 
   - Windows: Install [MSVC](https://visualstudio.microsoft.com/) or [MinGW](http://www.mingw.org/).

5. **Optional:** 
   - Coffee, a lot of coffee. 

**Pro tip:** If you’re running Windows, good luck. You’ll need to figure out yourself just to get the paths right. On Linux, you’re mostly fine… until it breaks.

## Building (Good Luck)

```bash
# for building all junk 
./scripts/build-all.sh

```bash
# for cleaning all junk 
./scripts/clean-all.sh

```bash
# for running junk 
./scripts/run.sh

# When that fails (it will)
make -f Makefile.engine
make -f Makefile.testbed

