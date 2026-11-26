# Executor

A utility to quickly launch or execute programs (MS Windows).

That description is the wording from Chris Nystrom's old programs page.
The included `EXECUTOR.DOC` describes it as a Microsoft Windows 3.0
application: a dialog box that lets you browse drives and directories
and start `.EXE`, `.COM`, or `.BAT` files.

Its purpose: Executor pops up a box where you type the path and name of a
program to run. As Chris remembers it, Windows 3.0 had no such box, and
Windows 3.1 later covered this with Program Manager's File > Run.

Executor was Chris Nystrom's first Windows program, written in the Windows 3.0
era with his late best friend J.R. McConnell.

Version 1.0, 25 June 1991. Developed by Chris Nystrom and J.R. McConnell
for Innovation Inc.

The 1991 Win16 source (`EXECUTOR.C` and the other `EXECUTOR.*` files) is
kept here as history. It cannot be built as a Win32 program. The current
program is Executor 2.1, a rewrite for Windows 10 and 11, in `executor2/`.

## Origin

From Chris Nystrom's old programs page (originally on cactus.org).
The download was `executor.zip`.

https://nystrom.family/chris/old/programs.html
https://nystrom.family/chris/old/executor.zip
All of Chris's code: https://nystrom.family/chris/code/

The zip contained `EXECUTOR.EXE`, `EXECUTOR.DOC`, and an inner source zip
(`EXECSRC.ZIP`). The executable, the compiled resource (`.RES`), and the
icon (`.ICO`) were stripped. Source and the original documentation remain.

## License

Executor (1991) by Chris Nystrom and J.R. McConnell. Released into the
public domain by Chris Nystrom in 2026, in memory of J.R. McConnell,
under the Unlicense; see `LICENSE`.

The original 1991 shareware terms are kept, unchanged, in `EXECUTOR.DOC`
for history, but they no longer apply.

## Build

To build Executor 2.1 (Windows 10/11):

    cd executor2
    make

This produces two statically linked executables that need only Windows
system DLLs:

- `executor2/bin/executor.exe` — 64-bit
- `executor2/bin/executor32.exe` — 32-bit

See `executor2/README.md` for features, other make targets, and how to
build on Windows.

The original 1991 program is a Windows 3.0 project (`windows.h`, a
`.MAK` / resource script layout) and is not compiled here.
