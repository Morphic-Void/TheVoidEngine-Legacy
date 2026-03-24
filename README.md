# Void Engine (Legacy) - Proof-of-Concept Archive

This repository contains the legacy engine and proof-of-concept demo developed between 2010 and 2016. It represents an early exploration of engine architecture, tooling, rendering systems, and asset pipelines that later informed the design of the modern Void Engine.

The codebase is preserved exactly as it existed during development, including its original directory layout, build scripts, and tooling. This ensures the project remains buildable and historically accurate.

---------------------------------------------------------------------

## Purpose of This Repository

This project is published as:

- a historical artefact
- a reference for developers interested in engine architecture
- a snapshot of early ideas that evolved into the modern Void Engine
- a working demonstration of a complete custom engine ecosystem from the period

It is not intended as a maintained or production-ready engine.

---------------------------------------------------------------------

## Why the Original Structure Is Preserved

The build system relies on:

- absolute-relative paths
- batch-file-driven orchestration
- project files expecting a specific directory hierarchy
- tools and scripts with hard-coded assumptions
- a self-contained shader compiler DLL from the development period

Because of these constraints, any attempt to reorganise the directory structure would break the build pipeline. To maintain authenticity and functionality, the layout is kept exactly as it was.

---------------------------------------------------------------------

## Build Status

Despite its age, the engine and tools still build successfully on modern MSVC. This is possible because:

- the repository includes the original shader compiler DLL
- the shader pipeline is self-contained
- the batch-file build system remains intact
- the solution and project files can be auto-updated by MSVC

Users familiar with Visual Studio should be able to build the solution without modification.

---------------------------------------------------------------------

## High-Level Overview of the System

This engine was developed as a proof of concept, but includes a substantial amount of infrastructure for a solo project. Major components include:

- Custom shader pipeline
  - HLSL compiled to .cso
  - reflection extracted and debug data stripped
  - custom metadata blob generated for automated PSO creation

- DLL hot-reload system
  - runtime code reloading for rapid iteration

- Asset conditioning tools
  - batch-file-driven build steps
  - pipeline tools for preparing runtime assets

- Multi-project Visual Studio solution
  - shared codebase under a "common" directory
  - separate configurations for tools, runtime, and demo

- Proof-of-concept demo
  - demonstrates rendering, systems, and engine behaviour

This description is intentionally high-level. The internal workings are complex and reflect the development practices and constraints of the time.

---------------------------------------------------------------------

## Intended Use

This repository is provided for:

- developers interested in historical engine design
- researchers exploring custom rendering pipelines
- anyone curious about the evolution of the modern Void Engine
- those who may find specific subsystems useful as reference material

It is not maintained, supported, or recommended for production use.

---------------------------------------------------------------------

## Relationship to the Modern Void Engine

Active development continues in the modern Void Engine, which is a complete re-architecture and production-ready evolution of the ideas explored here. The modern engine features:

- updated coding style
- redesigned systems
- new architecture
- modernised tooling
- SuiteUTF integration
- a clean, intentional directory layout

This legacy repository exists to document where the technology began.

---------------------------------------------------------------------

## License

This project is released under the same permissive MIT license used by SuiteUTF.
See the LICENSE file for details.

---------------------------------------------------------------------

## Final Notes

This repository is a preserved snapshot of a working engine ecosystem from a specific period. It is intentionally unmodified to maintain authenticity and buildability. Users are welcome to explore, study, and experiment with the code, but should expect a system that reflects its era.
