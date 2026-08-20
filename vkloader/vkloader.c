// Resolution, and the entry points that cannot be generated: they are called before an instance
// exists, so there is no handle to resolve them through.
//
// SPDX-License-Identifier: MIT

#include "vkloader.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__PS4__) || defined(__ORBIS__)
#include <orbis/libkernel.h>
#endif

// The instance every non-global resolve needs. One driver, one instance: the engine creates exactly one, and a
// second would want its own table - which is a loader's job and this is not one, so it says so rather than
// pretending.
static VkInstance g_instance;
static unsigned   g_instances;

/* ⚠ A FILE TOO, BECAUSE klog IS NOT WHERE THIS PROJECT READS. vkl_missing aborts, and until now it said so only
 * through sceKernelDebugOutText - so a run that died on a missing entry point looked, in every log this project
 * actually collects, exactly like a run that died anywhere else. That cost a round of suspicion aimed at Mesa's
 * WSI while the loader was never ruled out.
 *
 * Appended rather than truncated: the driver and the title own their own files, and this one has to survive
 * whatever order they are opened in. */
static FILE *
vkl_file(void)
{
  static FILE *f;
  static bool  tried;
  if(!tried) {
    tried = true;
    f = fopen("/data/tempest-vkloader.log", "w");
    }
  return f;
}

static void
vkl_log(const char *fmt, const char *a)
{
  char line[256];
  snprintf(line, sizeof(line), fmt, a);
  FILE *const lf = vkl_file();
  if(lf != NULL) {
    fprintf(lf, "%s\n", line);
    fflush(lf);
    }
#if defined(__PS4__) || defined(__ORBIS__)
  // ⚠ THE DEBUG LOG, NOT stdout. A statically linked eboot's stdout goes nowhere unless the title redirected
  // it, and a loader runs before a title has decided anything. sceKernelDebugOutText is the one channel that
  // works from the first instruction.
  sceKernelDebugOutText(0, line);
  sceKernelDebugOutText(0, "\n");
#else
  fputs(line, stderr);
  fputc('\n', stderr);
#endif
}

void
vkl_missing(const char *name)
{
  vkl_log("vkloader: FATAL - %s is not implemented by this driver", name);
  // ⚠ TRAP RATHER THAN RETURN. Returning would let the caller read an uninitialised out-parameter and blame
  // the next thing to touch it; abort() names this file in the backtrace. Nothing about the run is salvageable
  // once the engine calls an entry point the driver does not have.
  abort();
}

PFN_vkVoidFunction
vkl_resolve(const char *name)
{
  PFN_vkVoidFunction p = vk_icdGetInstanceProcAddr(g_instance, name);
  if(p == NULL)
    vkl_log("vkloader: %s unresolved - the driver does not export it", name);
  return p;
}

// ---------------------------------------------------------------- the global entry points
//
// These three are hand-written because the generator cannot help with them: they are the ones the ICD answers
// with a NULL instance, they are called BEFORE any instance exists, and vkCreateInstance additionally has to
// record the handle every later resolve depends on. Everything else is in vkthunks.c and is generated.

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                 VkInstance *pInstance)
{
  PFN_vkCreateInstance pfn = (PFN_vkCreateInstance)vk_icdGetInstanceProcAddr(NULL, "vkCreateInstance");
  if(pfn == NULL) {
    vkl_log("vkloader: FATAL - the ICD does not answer %s with a NULL instance", "vkCreateInstance");
    abort();
    }

  const VkResult r = pfn(pCreateInfo, pAllocator, pInstance);
  if(r != VK_SUCCESS)
    return r;

  ++g_instances;
  if(g_instances > 1)
    // Not fatal: the second instance works, and only calls made through the pointers this file caches would go
    // to the first. Said out loud because it is a limit rather than a bug, and a silent one would be a bug.
    vkl_log("vkloader: instance #%s created - later resolves still use the first", "2+");
  else
    g_instance = *pInstance;
  return r;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
                                       VkExtensionProperties *pProperties)
{
  PFN_vkEnumerateInstanceExtensionProperties pfn =
    (PFN_vkEnumerateInstanceExtensionProperties)vk_icdGetInstanceProcAddr(
      NULL, "vkEnumerateInstanceExtensionProperties");
  if(pfn == NULL)
    vkl_missing("vkEnumerateInstanceExtensionProperties");
  return pfn(pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
  // ⚠ ANSWERED HERE, NOT FORWARDED. Layers are the loader's mechanism and there is no loader, so the honest
  // answer is "none" - which is also what the engine wants to hear: vulkanapi.cpp only asks in order to decide
  // whether validation is available. An ICD is not required to implement this at all.
  (void)pProperties;
  if(pPropertyCount != NULL)
    *pPropertyCount = 0;
  return VK_SUCCESS;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
  // Straight through: this IS the ICD's own entry point, and the engine calls it directly for extension
  // functions (vdevice.cpp:285,538-539, vpoolcache.cpp:28). Answering from our own table instead would add a
  // second source of truth for no gain.
  return vk_icdGetInstanceProcAddr(instance, pName);
}
