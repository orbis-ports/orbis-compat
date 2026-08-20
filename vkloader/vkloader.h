/* Copyright © 2026 Mikołaj Mikołajczyk
 * SPDX-License-Identifier: MIT
 *
 * ⚠ THE MECHANISM LIVES HERE; THE GENERATED THUNKS DO NOT, AND THAT IS THE POINT OF THE MOVE.
 *
 * There is no Vulkan loader on this console and no shared object: the ICD is a static archive, so
 * something must define the vk* symbols a program references and forward them through
 * vkGetInstanceProcAddr. vkloader.c is that something, and gen.py writes one thunk per entry point
 * THE PROGRAM ITSELF references - read from its own object files.
 *
 * ⚠ SO vkthunks.c IS PER-CONSUMER AND IS NOT IN THIS REPOSITORY. Until 2026-08-19 the CTS compiled
 * Tempest's generated file, which is a list read out of a game engine's objects: dEQP got thunks it
 * does not call, and would have got a link error for anything it calls that Tempest does not. Each
 * consumer generates and commits its own beside its own build.
 */
#pragma once

// The Vulkan C ABI over the RADV ICD, for a platform with no loader and no shared objects.
//
// Nothing in a consumer's own code includes this: the whole point is that its Vulkan backend calls vkCreateInstance
// and vkCmdDraw exactly as it does on every other platform, and the linker finds them here. This header exists
// for the generated thunks and for a title that wants to check the loader came up.

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

// The one function ps4/radv proved is reachable in a statically linked eboot, and the only door the driver
// offers. Declared rather than included because the ICD entry point has no public header - the loader finds it
// by name, and here we ARE the loader.
PFN_vkVoidFunction vk_icdGetInstanceProcAddr(VkInstance instance, const char *pName);

// Resolution, in the two phases the ICD interface actually has.
//
// BEFORE AN INSTANCE EXISTS the ICD answers only the global entry points, so those resolve with a NULL
// instance. AFTER vkCreateInstance every other name resolves against the handle it returned - and Mesa's
// vk_instance_get_proc_addr searches the instance, physical-device AND device dispatch tables, returning its
// own generated trampoline for device-level entry points. That trampoline dispatches on the first argument
// (VkDevice/VkQueue/VkCommandBuffer), which with one driver, one device and no layers IS the whole dispatch
// chain a loader would have built. Nothing here needs a per-device table.
PFN_vkVoidFunction vkl_resolve(const char *name);

// ⚠ FAILS LOUDLY, and the harness paid for this lesson already: vkCmdBeginRendering against a 1.1 instance
// returned a NULL dispatch entry, the call jumped to address 0, and the log said nothing about which function
// it was. A thunk whose pointer is NULL comes here instead, which names the symbol and then traps.
void vkl_missing(const char *name);

#ifdef __cplusplus
}
#endif
