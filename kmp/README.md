# Sphere Studio — Kotlin Multiplatform (KMP) Module

This module contains the shared multiplatform codebase for Sphere Studio, supporting:
- **Android** (`androidMain`)
- **iOS** (`iosMain`)
- **Desktop (JVM)** (`desktopMain`)
- **Common code** (`commonMain`)

## Architecture
- Shared UI components using Jetpack / Compose Multiplatform
- Common abstractions and platform dispatchers
- Clean separation from `native_assets/` procedural engine and pipelines
