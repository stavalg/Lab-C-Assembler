# Lab C Assembler

A two-pass assembler written in ANSI C for the Lab C assembly language. The
program expands macros, validates labels and operands, builds the instruction
and data images, and writes the resulting machine-code files.

## Features

- Macro definition and expansion with `mcro` / `mcroend`
- First-pass symbol and memory analysis
- Second-pass label resolution and machine-code generation
- Optional symbol-table printing
- Support for code, data, string, matrix, entry, and external declarations
- Processing of multiple source files in one invocation

## Requirements

- GCC
- GNU Make

The Makefile uses common Unix shell commands such as `mkdir -p` and `rm -f`.
On Windows, build the project from WSL, MSYS2, or another environment that
provides these commands.

## Building

From the project root:

```sh
make
```

This creates the `assembler` executable. To remove the executable, object
files, and generated test output:

```sh
make clean
```

## Usage

Source files must use the `.as` extension:

```sh
./assembler program.as
```

Multiple files can be assembled in one run:

```sh
./assembler program1.as program2.as
```

To print the symbol table after the first pass:

```sh
./assembler --print-symbols program.as
```

The executable prints progress and validation errors to the terminal. Files
with an extension other than `.as` are skipped.

## Input language

### Instructions

The assembler recognizes the following instructions:

| Operands | Instructions |
| --- | --- |
| Two | `mov`, `cmp`, `add`, `sub`, `lea` |
| One | `not`, `clr`, `inc`, `dec`, `jmp`, `bne`, `red`, `prn`, `jsr` |
| None | `rts`, `stop` |

Supported addressing forms include:

- Immediate: `#5`
- Direct label: `LOOP`
- Register: `r0` through `r7`
- Indexed: `M1[r2][r7]`

### Directives

```asm
.data 10, -5, 20
.string "Hello"
.mat [2][2] 1, 2, 3, 4
.extern EXTERNAL_LABEL
.entry START
```

Labels are written before instructions or data declarations:

```asm
START:  mov  r1, VALUE
VALUE:  .data 42
```

Comments begin with a semicolon:

```asm
; This line is ignored
```

### Macros

Macros are defined with `mcro` and `mcroend`, then invoked by writing the
macro name on its own line:

```asm
mcro increment
    inc r1
    add r1, r2
mcroend

increment
stop
```

## Generated files

For `program.as`, the assembler can create:

| File | Description |
| --- | --- |
| `program.am` | Source after macro expansion |
| `program.ob` | Object file containing instruction and data images |
| `program.ent` | Entry-symbol addresses; created when `.entry` symbols exist |
| `program.ext` | External-symbol usages; created when external symbols are referenced |

Object and symbol addresses are written using the project's special base-4
representation (`a`, `b`, `c`, `d`). Generated files are written beside the
input source file.

## Project layout

```text
include/   Public headers
src/       C source files
obj/       Compiled object files
tests/     Example assembly files and expected output
Makefile   Build and cleanup rules
```

## Example

```sh
make
./assembler --print-symbols tests/test1.as
```

The command first creates `tests/test1.am`, then performs both assembly passes
and writes the applicable `.ob`, `.ent`, and `.ext` files under `tests/`.
