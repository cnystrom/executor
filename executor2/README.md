# Executor 2.0

A small program launcher for Windows 10 and 11. Type or browse to a
program, document, or URL and run it.

Executor 2.0 continues the idea of Executor, the 1991 Windows 3.0
program written by Chris Nystrom and J.R. McConnell. The original
popped up a dialog where you typed a path or browsed drives and
directories to start a `.EXE`, `.COM`, or `.BAT` file — a File > Run
box, which Windows 3.0 did not have.

The application icon is the original 1991 Executor icon, kept as
crisp pixel art at 16, 32, 48, and 256 pixels.

## Features

- File field, with an editable list of the last 10 programs you ran
  (saved in `%APPDATA%\Executor\recent.txt`)
- Browse button, using the Windows common file dialog
- Optional arguments and working directory (the working directory
  defaults to the program's folder when left blank)
- Run as administrator (requests elevation with the `runas` verb)
- Opens programs (`.exe`, `.com`, `.bat`, `.cmd`) as well as
  documents and URLs
- Enter runs the file; Esc closes the window
- A clear error message if the file cannot be started
- About box crediting the 1991 program and this 2026 version

## Build

On Linux, with mingw-w64 installed:

    make

This produces two statically linked executables that need only
Windows system DLLs:

- `bin/executor.exe` — 64-bit
- `bin/executor32.exe` — 32-bit

Useful targets: `make 64`, `make 32`, `make clean`.

The compiler flags are `-Wall -Wextra`. On Windows, the same
`src/executor.c` and `src/executor.rc` can be built with MinGW-w64
or a recent Windows SDK.

## License

Public domain (Unlicense). See `LICENSE`.

Original Executor (1991) by Chris Nystrom and J.R. McConnell.
Executor 2.0 (2026).
