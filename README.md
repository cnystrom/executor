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

Public domain. In 2026 Chris Nystrom released Executor, written with his late
best friend J.R. McConnell, into the public domain under the Unlicense; see
`LICENSE`. The original 1991 shareware terms are kept, unchanged, in
`EXECUTOR.DOC` for history, but they no longer apply.

## Build

This is a Windows 3.0 program (`windows.h`, a `.MAK` / resource script
layout). It was not compiled here. No source was modified.
