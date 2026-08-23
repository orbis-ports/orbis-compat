// Copyright © 2026 Mikołaj Mikołajczyk
// SPDX-License-Identifier: MIT
//
// getenv() for an image that is not the one the operator's env file was applied to.
//
// ⚠ USE THIS INSTEAD OF getenv() FOR ANY KNOB A LOADABLE MODULE MIGHT READ. The SDK's libc is a
// static musl archive, so an executable and every .prx it loads carry their own `environ`;
// setenv() in one is invisible to the others. src/orbis_env.cpp has the measurement that
// established it and the reason nobody hit it until a core needed a knob.
//
// Answers from the process's own environment first, so anything genuinely set still wins.
#ifndef ORBIS_ENV_H
#define ORBIS_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

const char* orbis_env_get(const char* name);

#ifdef __cplusplus
}
#endif

#endif  // ORBIS_ENV_H
