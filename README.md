# Loxe: an evolution of the lox programming language

## Getting Started

### Prerequisites

Before you begin, ensure you have the following tools installed on your system:

* C++20 compiler (recommended: MSVC, GCC, or Clang)
* CMake >= 3.8

### Building and Running

1. Clone the repository
2. Navigate to the project directory:
    ```bash
    cd loxe_language/
    ```

3. Build using CMake:
    ```bash
    cmake -B build -S .
    cmake --build build
    ```

4. Or build using the provided Makefile:
    ```bash
    make
    ```

Alternatively, you can compile and run the using an IDE that supports cmake
projects such as [`Visual Studio 2022`](https://visualstudio.microsoft.com/vs/)
or [`CLion`](https://www.jetbrains.com/clion/).

### Usage

Once the project is successfully built, you can run the interpreter with the
generated executable. Simply provide a source file containing a loxe program
as an argument to the executable to run it:

```bash
./build/loxe <script.loxe>
```
