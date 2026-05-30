# cmake-template

A modern C++23 cross-platform static library template using CMake, Conan 2, and GitHub Actions CI.

[![CI](https://github.com/pcorbineau/cmake-template/actions/workflows/ci.yml/badge.svg)](https://github.com/pcorbineau/cmake-template/actions/workflows/ci.yml)
[![Formatting](https://github.com/pcorbineau/cmake-template/actions/workflows/formatting.yml/badge.svg)](https://github.com/pcorbineau/cmake-template/actions/workflows/formatting.yml)
[![Commitlint](https://github.com/pcorbineau/cmake-template/actions/workflows/commitlint.yml/badge.svg)](https://github.com/pcorbineau/cmake-template/actions/workflows/commitlint.yml)
[![Coverage](https://pcorbineau.github.io/cmake-template/coverage-badge.svg)](https://pcorbineau.github.io/cmake-template/)

## Features

- **C++23** with strict compiler warnings (`-Wall -Wextra -Wpedantic -Werror`)
- **Cross-platform**: Linux (GCC), macOS (Clang), Windows (MSVC)
- **Conan 2** for dependency management
- **ccache** for faster incremental builds
- **clang-tidy** static analysis
- **Code coverage** via `llvm-cov` (Clang), `gcovr` (GCC), `OpenCppCoverage` (MSVC)
- **Coverage dashboard** published to GitHub Pages
- **Conventional Commits** enforced via commitlint

## Requirements

| Tool | Minimum version |
|------|----------------|
| CMake | 3.28 |
| Conan | 2.x |
| Ninja | any recent |
| C++ compiler | GCC 16 / Clang 19 / MSVC 2026 |

## Getting started

### Install dependencies

```bash
pip install conan gcovr
conan profile detect --force
```

### Configure and build

```bash
# Using a preset (debug / release / coverage)
conan install . --profile conan/profiles/ci --build=missing
cmake --preset debug
cmake --build --preset debug
```

### Run tests

```bash
ctest --preset debug
```

### Generate coverage report

```bash
conan install . --profile conan/profiles/ci --build=missing
cmake --preset coverage
cmake --build --preset coverage
cmake --build --preset coverage --target coverage
```

The HTML report is generated inside `build/coverage/`.

## Project structure

```
cmake-template/
├── cmake/              # Custom CMake modules (ccache, clang-tidy, coverage, …)
├── conan/profiles/     # Conan CI profile
├── lib/                # Library source and headers (mylib)
│   ├── include/mylib/
│   └── src/
├── tests/              # Catch2 unit tests
└── .github/
    ├── workflows/      # CI, formatting, commitlint
    ├── scripts/        # Coverage summary merge script
    └── pages/          # GitHub Pages coverage dashboard
```

## CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTING` | `ON` | Build the test suite |
| `USE_CONAN` | `ON` | Inject Conan-generated CMake files |
| `USE_CCACHE` | `ON` | Use ccache as compiler launcher |
| `USE_CLANG_TIDY` | `ON` | Enable clang-tidy static analysis |
| `ENABLE_COVERAGE` | `OFF` | Instrument for code coverage (mutually exclusive with `USE_CLANG_TIDY`) |

## Available presets

| Preset | Build type | Coverage | clang-tidy |
|--------|-----------|----------|------------|
| `debug` | Debug | OFF | ON |
| `release` | RelWithDebInfo | OFF | ON |
| `coverage` | Debug | ON | OFF |

## Dependencies

| Library | Version | Usage |
|---------|---------|-------|
| [spdlog](https://github.com/gabime/spdlog) | 1.15.3 | Logging |
| [Catch2](https://github.com/catchorg/Catch2) | 3.15.0 | Unit testing |

## License

This project is provided as a template. Add your license here.
