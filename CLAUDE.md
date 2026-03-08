# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`dcc` is a compiler for the Decaf language (a Java-like teaching language). It compiles `.dcf`/`.decaf` source files to x86-64 AT&T-syntax assembly, which is then linked with `gcc`. Written in C++20 using `bisonc++` and `flexc++` for parser/scanner generation.

## Build

The build directory is `dcc/src/build/`. All test scripts must be run from `dcc/`.

```bash
# Configure and build
cd dcc/src
mkdir -p build && cd build
cmake ..
make

# Install dcc binary to dcc/
make install
```

**Dependencies:** `bisonc++`, `flexc++`, `popt`, `CppUnit`, `gcc` (for linking generated assembly).

The CMake build regenerates `parse.cc`/`Parserbase.h` (from `decaf.y` via `bisonc++`) and `lex.cc`/`Scannerbase.h` (from `decaf.l` via `flexc++`) automatically. These generated files are tracked in git.

## Running Tests

All test scripts are run from `dcc/` and expect the binary at `dcc/src/build/dcc`:

```bash
cd dcc
./runtest.sh          # Semantic/IR tests (testdata/semantic/)
./runcodegen.sh       # Code generation tests (testdata/codegen/)
./rundataflow.sh      # Dataflow/basic-blocks optimization tests (testdata/dataflow/)
./runoptimize.sh      # Optimizer correctness + performance tests (testdata/optimizer/)
./runsamples.sh       # Stanford sample programs (testdata/samples/)
```

Each test compiles a `.dcf` file, links with `gcc`, runs the binary, and diffs output against a `.out` file in the corresponding `output/` subdirectory.

To test a single file manually:
```bash
./src/build/dcc -o out/foo.s testdata/semantic/foo.dcf 2> out/foo.output
diff out/foo.output testdata/semantic/output/foo.out
```

## Compiler CLI

```
dcc [options] <input.dcf>
  -t scan|parse    # scan-only mode or full parse (default: parse)
  -o <file>        # output filename (default: decaf.out)
  -d               # dump AST
  -i               # print TAC IR before codegen
  -b               # print basic blocks
  --opt-all        # enable all optimizations
  --opt-basic-blocks                    # all basic-block optimizations
  --opt-basic-blocks-const-folding
  --opt-basic-blocks-common-subexpr-elim
  --opt-basic-blocks-alg-simp
  --opt-basic-blocks-copy-prop
  --opt-basic-blocks-dead-code
  --opt-common-subexpr-elim            # global CSE
```

## Architecture

### Compilation Pipeline

1. **Lex/Parse** — `decaf.l` (flexc++) and `decaf.y` (bisonc++) build an AST of `IrBase`-derived nodes into an `IrProgram`.
2. **Semantic analysis** — `Parser::semanticChecks()` drives three AST traversals via `IrTraversalContext`:
   - `propagateTypes` — type inference pass
   - `analyze` — semantic checks (type errors, undeclared variables, etc.)
   - `allocate` — stack/memory layout
3. **Codegen** — each AST node's `codegen(IrTraversalContext*)` emits `IrTacStmt` records (Three-Address Code) into the context.
4. **Optimization** — `IrOptimizer` partitions TAC into basic blocks, builds a control flow graph (NxN adjacency matrix), runs per-block and global optimizations, then flattens back to TAC.
5. **Assembly emission** — `IrTacGenCode()` in `IrTAC.cpp` converts each TAC statement to x86-64 AT&T assembly.

### Key Classes

| Class | File | Role |
|---|---|---|
| `IrBase` | `ir/IrBase.h` | Abstract base for all AST nodes; virtual `propagateTypes`, `print`, `analyze`, `allocate`, `codegen` |
| `IrProgram` | `ir/IrProgram.h` | Root AST node (fields, methods, classes, interfaces) |
| `IrTraversalContext` | `ir/IrTravCtx.h` | Passed to all AST traversals; holds symbol table stack, parent stack, accumulated TAC statements, and string/double literals |
| `IrSymbolTable` | `ir/IrSymbolTable.h` | Scoped symbol table for variables and methods |
| `IrTacStmt` / `IrTacArg` | `ir/IrTAC.h` | Three-address code instruction (`IrOpcode`, src0, src1, dst) |
| `IrBasicBlock` | `ir/IrBasicBlock.h` | A sequence of TAC statements for local optimization (constant folding, CSE, copy propagation, dead code, algebraic simplification) |
| `IrOptimizer` | `ir/IrOptimizer.h` | Partitions TAC into basic blocks, builds CFG, runs optimizations, supports global CSE |
| `Parser` | `Parser.h` | bisonc++-generated parser; drives the full pipeline via `parse()`, `semanticChecks()`, `codegen()` |

### IR Library

All IR node implementations are compiled into a static library `IrAst` (from `ir/CMakeLists.txt`). The main `dcc` executable links against `IrAst` and `popt`. The test binary `testDccIr` also links `IrAst` and `CppUnit`.

### TAC Opcodes

Defined in `ir/IrTAC.h` as `IrOpcode`. Key ones: `MOV`, `LOAD`/`STORE`, `ADD`/`SUB`/`MUL`/`DIV`/`MOD`, `CALL`/`FBEGIN`/`RETURN`, comparison ops, `LABEL`/`JUMP`/`IFZ`/`IFNZ`, `PARAM`/`GETPARAM`, `GLOBAL`/`STRING`/`DOUBLE`.

### Namespace

All IR code lives in `namespace Decaf`. The `Parser.h` has `using namespace Decaf;` at the top.
