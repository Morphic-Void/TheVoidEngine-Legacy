# Void Engine (Legacy) - Proof-of-Concept Archive

This repository contains the legacy engine and proof-of-concept demo developed
between 2010 and 2016. It represents an early exploration of engine
architecture, tooling, rendering systems, asset pipelines, and game-specific
spatial ideas that later informed the design of The Manifold Engine.

This repository should be read as historical exploratory code and design
evidence. It is not the active production engine, not the current codebase, and
not a representation of the engineering standards, documentation style, or
architectural discipline used by the active project.

The source tree, directory layout, build scripts, and tooling are preserved in
their original form so the project remains buildable and historically accurate.

---------------------------------------------------------------------

## Purpose of This Repository

This project is published as:

- a historical artefact
- a record of early validation work
- a reference for developers interested in engine architecture
- a snapshot of early ideas that evolved into The Manifold Engine
- a working demonstration of a complete custom engine ecosystem from the period

The repository records an earlier working prototype that helped identify the
shape of the game, the engine, the editor, and the supporting tooling now being
rebuilt more deliberately in the active codebase.

It is not intended as a maintained or production-ready engine.

---------------------------------------------------------------------

## What This Repository Is Not

This repository is not:

- the active production engine
- the current codebase
- a maintained general-purpose game engine
- a competitor to Unreal, Unity, Godot, or similar engines
- representative of the final code quality, API design, documentation, or
  implementation discipline
- intended as a general-purpose framework for other projects

The code is exploratory and provisional. Some of it is old enough to reflect
earlier constraints, earlier tooling, and earlier engineering preferences.

Its value is that it records working design evidence from an earlier exploratory
phase of the project.

---------------------------------------------------------------------

## Why the Original Structure Is Preserved

The build system relies on:

- absolute-relative paths
- batch-file-driven orchestration
- project files expecting a specific directory hierarchy
- tools and scripts with hard-coded assumptions
- a self-contained shader compiler DLL from the development period

Because of these constraints, reorganising the directory structure would break
the build pipeline. To maintain authenticity and functionality, the layout is
kept in its original form.

---------------------------------------------------------------------

## Build Status

Despite its age, the engine and tools still build successfully on modern MSVC.
This is possible because:

- the repository includes the original shader compiler DLL
- the shader pipeline is self-contained
- the batch-file build system remains intact
- the solution and project files can be auto-updated by MSVC

Users familiar with Visual Studio should be able to build the solution without
modification.

---------------------------------------------------------------------

## High-Level Overview of the System

This engine was developed as a proof of concept, but includes a substantial
amount of infrastructure for a solo project. Major components include:

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

This description is intentionally high-level. The internal workings are complex
and reflect the development practices and constraints of the time.

---------------------------------------------------------------------

## Intended Use

This repository is provided as a resource for:

- developers interested in historical engine design
- researchers exploring custom rendering pipelines
- anyone curious about the origins of The Manifold Engine
- those who may find specific subsystems useful as reference material

It is not maintained, supported, or recommended for production use.

---------------------------------------------------------------------

## Relationship to The Manifold Engine

Active development continues in The Manifold Engine, which is a controlled
reconstruction of the useful lessons from this prototype under a new
architecture, stricter engineering rules, and a cleaner project structure.

The active engine/editor codebase is here:

- [The Manifold Engine](https://github.com/Morphic-Void/morphic-manifold-engine)

For more context on how this prototype influenced the active engine, see:

- [Origins](https://github.com/Morphic-Void/morphic-manifold-engine/blob/main/docs/origins.md)

The Manifold Engine is intentionally narrower than a general-purpose public
engine. It exists to support a specific game, its spatial model, and the
surrounding tooling needed to build it.

This legacy repository exists to document where the technology began.

---------------------------------------------------------------------

## License

Licensed under the MIT License. See the LICENSE file for details.

Copyright (c) 2010-2016 Ritchie Brannan.

Attribution is appreciated where practical.

This project is provided as-is and is not maintained.

---------------------------------------------------------------------

## Final Notes

This repository is a preserved snapshot of a working engine ecosystem from a
specific period. It is intentionally kept in its original structure to maintain
authenticity and buildability.

Users are welcome to explore, study, and experiment with the code, but should
expect a system that reflects its era.
