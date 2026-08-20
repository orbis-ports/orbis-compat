/* GENERATED - do not edit. Regenerate with:
 *
 *     python3 <orbis-compat>/vkloader/gen.py --objs <this project's object dirs> --out <this file>
 *
 * One forwarding thunk per Vulkan entry point THIS project references, over the RADV ICD. There is no Vulkan
 * loader on this console and no shared object, so something has to define the vk* symbols a program calls and
 * forward them through vkGetInstanceProcAddr; vkloader.c is that something and this is the generated half.
 *
 * The list is read from this project's own objects, so it cannot rot into a link error in a file nobody edited.
 * Re-running the command above must leave `git diff` empty.
 *
 * THE POINTERS ARE RESOLVED ON FIRST CALL, per thunk. A race between two threads calling the same entry point
 * for the first time resolves it twice and stores the same value twice, which is benign - the alternative is a
 * mutex on every Vulkan call in the program, to protect a write that cannot tear.
 *
 * vkloader.h is orbis-compat's, reached through the include directory the build adds for it -
 * ORBIS_VKLOADER_DIR in a CMake consumer. It is not a file of this repository.
 *
 * SPDX-License-Identifier: MIT
 */

#include "vkloader.h"

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAcquireDrmDisplayEXT(VkPhysicalDevice physicalDevice, int32_t drmFd, VkDisplayKHR display)
{
  static PFN_vkAcquireDrmDisplayEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAcquireDrmDisplayEXT)vkl_resolve("vkAcquireDrmDisplayEXT");
    if(pfn == NULL)
      vkl_missing("vkAcquireDrmDisplayEXT");
    }
  return pfn(physicalDevice, drmFd, display);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
{
  static PFN_vkAcquireNextImage2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAcquireNextImage2KHR)vkl_resolve("vkAcquireNextImage2KHR");
    if(pfn == NULL)
      vkl_missing("vkAcquireNextImage2KHR");
    }
  return pfn(device, pAcquireInfo, pImageIndex);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
{
  static PFN_vkAcquireNextImageKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAcquireNextImageKHR)vkl_resolve("vkAcquireNextImageKHR");
    if(pfn == NULL)
      vkl_missing("vkAcquireNextImageKHR");
    }
  return pfn(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAcquirePerformanceConfigurationINTEL(VkDevice device, const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo, VkPerformanceConfigurationINTEL* pConfiguration)
{
  static PFN_vkAcquirePerformanceConfigurationINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAcquirePerformanceConfigurationINTEL)vkl_resolve("vkAcquirePerformanceConfigurationINTEL");
    if(pfn == NULL)
      vkl_missing("vkAcquirePerformanceConfigurationINTEL");
    }
  return pfn(device, pAcquireInfo, pConfiguration);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAcquireProfilingLockKHR(VkDevice device, const VkAcquireProfilingLockInfoKHR* pInfo)
{
  static PFN_vkAcquireProfilingLockKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAcquireProfilingLockKHR)vkl_resolve("vkAcquireProfilingLockKHR");
    if(pfn == NULL)
      vkl_missing("vkAcquireProfilingLockKHR");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers)
{
  static PFN_vkAllocateCommandBuffers pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAllocateCommandBuffers)vkl_resolve("vkAllocateCommandBuffers");
    if(pfn == NULL)
      vkl_missing("vkAllocateCommandBuffers");
    }
  return pfn(device, pAllocateInfo, pCommandBuffers);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets)
{
  static PFN_vkAllocateDescriptorSets pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAllocateDescriptorSets)vkl_resolve("vkAllocateDescriptorSets");
    if(pfn == NULL)
      vkl_missing("vkAllocateDescriptorSets");
    }
  return pfn(device, pAllocateInfo, pDescriptorSets);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory)
{
  static PFN_vkAllocateMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAllocateMemory)vkl_resolve("vkAllocateMemory");
    if(pfn == NULL)
      vkl_missing("vkAllocateMemory");
    }
  return pfn(device, pAllocateInfo, pAllocator, pMemory);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkAntiLagUpdateAMD(VkDevice device, const VkAntiLagDataAMD* pData)
{
  static PFN_vkAntiLagUpdateAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkAntiLagUpdateAMD)vkl_resolve("vkAntiLagUpdateAMD");
    if(pfn == NULL)
      vkl_missing("vkAntiLagUpdateAMD");
    }
  pfn(device, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo)
{
  static PFN_vkBeginCommandBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBeginCommandBuffer)vkl_resolve("vkBeginCommandBuffer");
    if(pfn == NULL)
      vkl_missing("vkBeginCommandBuffer");
    }
  return pfn(commandBuffer, pBeginInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindAccelerationStructureMemoryNV(VkDevice device, uint32_t bindInfoCount, const VkBindAccelerationStructureMemoryInfoNV* pBindInfos)
{
  static PFN_vkBindAccelerationStructureMemoryNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindAccelerationStructureMemoryNV)vkl_resolve("vkBindAccelerationStructureMemoryNV");
    if(pfn == NULL)
      vkl_missing("vkBindAccelerationStructureMemoryNV");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
  static PFN_vkBindBufferMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindBufferMemory)vkl_resolve("vkBindBufferMemory");
    if(pfn == NULL)
      vkl_missing("vkBindBufferMemory");
    }
  return pfn(device, buffer, memory, memoryOffset);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos)
{
  static PFN_vkBindBufferMemory2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindBufferMemory2)vkl_resolve("vkBindBufferMemory2");
    if(pfn == NULL)
      vkl_missing("vkBindBufferMemory2");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindBufferMemory2KHR(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos)
{
  static PFN_vkBindBufferMemory2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindBufferMemory2KHR)vkl_resolve("vkBindBufferMemory2KHR");
    if(pfn == NULL)
      vkl_missing("vkBindBufferMemory2KHR");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindDataGraphPipelineSessionMemoryARM(VkDevice device, uint32_t bindInfoCount, const VkBindDataGraphPipelineSessionMemoryInfoARM* pBindInfos)
{
  static PFN_vkBindDataGraphPipelineSessionMemoryARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindDataGraphPipelineSessionMemoryARM)vkl_resolve("vkBindDataGraphPipelineSessionMemoryARM");
    if(pfn == NULL)
      vkl_missing("vkBindDataGraphPipelineSessionMemoryARM");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
  static PFN_vkBindImageMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindImageMemory)vkl_resolve("vkBindImageMemory");
    if(pfn == NULL)
      vkl_missing("vkBindImageMemory");
    }
  return pfn(device, image, memory, memoryOffset);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos)
{
  static PFN_vkBindImageMemory2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindImageMemory2)vkl_resolve("vkBindImageMemory2");
    if(pfn == NULL)
      vkl_missing("vkBindImageMemory2");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindImageMemory2KHR(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos)
{
  static PFN_vkBindImageMemory2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindImageMemory2KHR)vkl_resolve("vkBindImageMemory2KHR");
    if(pfn == NULL)
      vkl_missing("vkBindImageMemory2KHR");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindOpticalFlowSessionImageNV(VkDevice device, VkOpticalFlowSessionNV session, VkOpticalFlowSessionBindingPointNV bindingPoint, VkImageView view, VkImageLayout layout)
{
  static PFN_vkBindOpticalFlowSessionImageNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindOpticalFlowSessionImageNV)vkl_resolve("vkBindOpticalFlowSessionImageNV");
    if(pfn == NULL)
      vkl_missing("vkBindOpticalFlowSessionImageNV");
    }
  return pfn(device, session, bindingPoint, view, layout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindTensorMemoryARM(VkDevice device, uint32_t bindInfoCount, const VkBindTensorMemoryInfoARM* pBindInfos)
{
  static PFN_vkBindTensorMemoryARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindTensorMemoryARM)vkl_resolve("vkBindTensorMemoryARM");
    if(pfn == NULL)
      vkl_missing("vkBindTensorMemoryARM");
    }
  return pfn(device, bindInfoCount, pBindInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBindVideoSessionMemoryKHR(VkDevice device, VkVideoSessionKHR videoSession, uint32_t bindSessionMemoryInfoCount, const VkBindVideoSessionMemoryInfoKHR* pBindSessionMemoryInfos)
{
  static PFN_vkBindVideoSessionMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBindVideoSessionMemoryKHR)vkl_resolve("vkBindVideoSessionMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkBindVideoSessionMemoryKHR");
    }
  return pfn(device, videoSession, bindSessionMemoryInfoCount, pBindSessionMemoryInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBuildAccelerationStructuresKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
  static PFN_vkBuildAccelerationStructuresKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBuildAccelerationStructuresKHR)vkl_resolve("vkBuildAccelerationStructuresKHR");
    if(pfn == NULL)
      vkl_missing("vkBuildAccelerationStructuresKHR");
    }
  return pfn(device, deferredOperation, infoCount, pInfos, ppBuildRangeInfos);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkBuildMicromapsEXT(VkDevice device, VkDeferredOperationKHR deferredOperation, uint32_t infoCount, const VkMicromapBuildInfoEXT* pInfos)
{
  static PFN_vkBuildMicromapsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkBuildMicromapsEXT)vkl_resolve("vkBuildMicromapsEXT");
    if(pfn == NULL)
      vkl_missing("vkBuildMicromapsEXT");
    }
  return pfn(device, deferredOperation, infoCount, pInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkClearShaderInstrumentationMetricsARM(VkDevice device, VkShaderInstrumentationARM instrumentation)
{
  static PFN_vkClearShaderInstrumentationMetricsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkClearShaderInstrumentationMetricsARM)vkl_resolve("vkClearShaderInstrumentationMetricsARM");
    if(pfn == NULL)
      vkl_missing("vkClearShaderInstrumentationMetricsARM");
    }
  pfn(device, instrumentation);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginConditionalRendering2EXT(VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfo2EXT* pConditionalRenderingBegin)
{
  static PFN_vkCmdBeginConditionalRendering2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginConditionalRendering2EXT)vkl_resolve("vkCmdBeginConditionalRendering2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginConditionalRendering2EXT");
    }
  pfn(commandBuffer, pConditionalRenderingBegin);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginConditionalRenderingEXT(VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin)
{
  static PFN_vkCmdBeginConditionalRenderingEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginConditionalRenderingEXT)vkl_resolve("vkCmdBeginConditionalRenderingEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginConditionalRenderingEXT");
    }
  pfn(commandBuffer, pConditionalRenderingBegin);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginCustomResolveEXT(VkCommandBuffer commandBuffer, const VkBeginCustomResolveInfoEXT* pBeginCustomResolveInfo)
{
  static PFN_vkCmdBeginCustomResolveEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginCustomResolveEXT)vkl_resolve("vkCmdBeginCustomResolveEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginCustomResolveEXT");
    }
  pfn(commandBuffer, pBeginCustomResolveInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
  static PFN_vkCmdBeginDebugUtilsLabelEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkl_resolve("vkCmdBeginDebugUtilsLabelEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginDebugUtilsLabelEXT");
    }
  pfn(commandBuffer, pLabelInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCmdBeginGpaSampleAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD gpaSession, const VkGpaSampleBeginInfoAMD* pGpaSampleBeginInfo, uint32_t* pSampleID)
{
  static PFN_vkCmdBeginGpaSampleAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginGpaSampleAMD)vkl_resolve("vkCmdBeginGpaSampleAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginGpaSampleAMD");
    }
  return pfn(commandBuffer, gpaSession, pGpaSampleBeginInfo, pSampleID);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCmdBeginGpaSessionAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD gpaSession)
{
  static PFN_vkCmdBeginGpaSessionAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginGpaSessionAMD)vkl_resolve("vkCmdBeginGpaSessionAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginGpaSessionAMD");
    }
  return pfn(commandBuffer, gpaSession);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginPerTileExecutionQCOM(VkCommandBuffer commandBuffer, const VkPerTileBeginInfoQCOM* pPerTileBeginInfo)
{
  static PFN_vkCmdBeginPerTileExecutionQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginPerTileExecutionQCOM)vkl_resolve("vkCmdBeginPerTileExecutionQCOM");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginPerTileExecutionQCOM");
    }
  pfn(commandBuffer, pPerTileBeginInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
  static PFN_vkCmdBeginQuery pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginQuery)vkl_resolve("vkCmdBeginQuery");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginQuery");
    }
  pfn(commandBuffer, queryPool, query, flags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags, uint32_t index)
{
  static PFN_vkCmdBeginQueryIndexedEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginQueryIndexedEXT)vkl_resolve("vkCmdBeginQueryIndexedEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginQueryIndexedEXT");
    }
  pfn(commandBuffer, queryPool, query, flags, index);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
{
  static PFN_vkCmdBeginRenderPass pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginRenderPass)vkl_resolve("vkCmdBeginRenderPass");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginRenderPass");
    }
  pfn(commandBuffer, pRenderPassBegin, contents);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRenderPass2(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo)
{
  static PFN_vkCmdBeginRenderPass2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginRenderPass2)vkl_resolve("vkCmdBeginRenderPass2");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginRenderPass2");
    }
  pfn(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRenderPass2KHR(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo)
{
  static PFN_vkCmdBeginRenderPass2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginRenderPass2KHR)vkl_resolve("vkCmdBeginRenderPass2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginRenderPass2KHR");
    }
  pfn(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRendering(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo)
{
  static PFN_vkCmdBeginRendering pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginRendering)vkl_resolve("vkCmdBeginRendering");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginRendering");
    }
  pfn(commandBuffer, pRenderingInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRenderingKHR(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo)
{
  static PFN_vkCmdBeginRenderingKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginRenderingKHR)vkl_resolve("vkCmdBeginRenderingKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginRenderingKHR");
    }
  pfn(commandBuffer, pRenderingInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginShaderInstrumentationARM(VkCommandBuffer commandBuffer, VkShaderInstrumentationARM instrumentation)
{
  static PFN_vkCmdBeginShaderInstrumentationARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginShaderInstrumentationARM)vkl_resolve("vkCmdBeginShaderInstrumentationARM");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginShaderInstrumentationARM");
    }
  pfn(commandBuffer, instrumentation);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginTransformFeedback2EXT(VkCommandBuffer commandBuffer, uint32_t firstCounterRange, uint32_t counterRangeCount, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos)
{
  static PFN_vkCmdBeginTransformFeedback2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginTransformFeedback2EXT)vkl_resolve("vkCmdBeginTransformFeedback2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginTransformFeedback2EXT");
    }
  pfn(commandBuffer, firstCounterRange, counterRangeCount, pCounterInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer, uint32_t counterBufferCount, const VkBuffer* pCounterBuffers, const VkDeviceSize* pCounterBufferOffsets)
{
  static PFN_vkCmdBeginTransformFeedbackEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginTransformFeedbackEXT)vkl_resolve("vkCmdBeginTransformFeedbackEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginTransformFeedbackEXT");
    }
  pfn(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBeginVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoBeginCodingInfoKHR* pBeginInfo)
{
  static PFN_vkCmdBeginVideoCodingKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBeginVideoCodingKHR)vkl_resolve("vkCmdBeginVideoCodingKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBeginVideoCodingKHR");
    }
  pfn(commandBuffer, pBeginInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorBufferEmbeddedSamplers2EXT(VkCommandBuffer commandBuffer, const VkBindDescriptorBufferEmbeddedSamplersInfoEXT* pBindDescriptorBufferEmbeddedSamplersInfo)
{
  static PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT)vkl_resolve("vkCmdBindDescriptorBufferEmbeddedSamplers2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindDescriptorBufferEmbeddedSamplers2EXT");
    }
  pfn(commandBuffer, pBindDescriptorBufferEmbeddedSamplersInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorBufferEmbeddedSamplersEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set)
{
  static PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT)vkl_resolve("vkCmdBindDescriptorBufferEmbeddedSamplersEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindDescriptorBufferEmbeddedSamplersEXT");
    }
  pfn(commandBuffer, pipelineBindPoint, layout, set);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorBuffersEXT(VkCommandBuffer commandBuffer, uint32_t bufferCount, const VkDescriptorBufferBindingInfoEXT* pBindingInfos)
{
  static PFN_vkCmdBindDescriptorBuffersEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindDescriptorBuffersEXT)vkl_resolve("vkCmdBindDescriptorBuffersEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindDescriptorBuffersEXT");
    }
  pfn(commandBuffer, bufferCount, pBindingInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
  static PFN_vkCmdBindDescriptorSets pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindDescriptorSets)vkl_resolve("vkCmdBindDescriptorSets");
    if(pfn == NULL)
      vkl_missing("vkCmdBindDescriptorSets");
    }
  pfn(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorSets2(VkCommandBuffer commandBuffer, const VkBindDescriptorSetsInfo* pBindDescriptorSetsInfo)
{
  static PFN_vkCmdBindDescriptorSets2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindDescriptorSets2)vkl_resolve("vkCmdBindDescriptorSets2");
    if(pfn == NULL)
      vkl_missing("vkCmdBindDescriptorSets2");
    }
  pfn(commandBuffer, pBindDescriptorSetsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorSets2KHR(VkCommandBuffer commandBuffer, const VkBindDescriptorSetsInfo* pBindDescriptorSetsInfo)
{
  static PFN_vkCmdBindDescriptorSets2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindDescriptorSets2KHR)vkl_resolve("vkCmdBindDescriptorSets2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBindDescriptorSets2KHR");
    }
  pfn(commandBuffer, pBindDescriptorSetsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
  static PFN_vkCmdBindIndexBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindIndexBuffer)vkl_resolve("vkCmdBindIndexBuffer");
    if(pfn == NULL)
      vkl_missing("vkCmdBindIndexBuffer");
    }
  pfn(commandBuffer, buffer, offset, indexType);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindIndexBuffer2(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, VkIndexType indexType)
{
  static PFN_vkCmdBindIndexBuffer2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindIndexBuffer2)vkl_resolve("vkCmdBindIndexBuffer2");
    if(pfn == NULL)
      vkl_missing("vkCmdBindIndexBuffer2");
    }
  pfn(commandBuffer, buffer, offset, size, indexType);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindIndexBuffer2KHR(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, VkIndexType indexType)
{
  static PFN_vkCmdBindIndexBuffer2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindIndexBuffer2KHR)vkl_resolve("vkCmdBindIndexBuffer2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBindIndexBuffer2KHR");
    }
  pfn(commandBuffer, buffer, offset, size, indexType);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindIndexBuffer3KHR(VkCommandBuffer commandBuffer, const VkBindIndexBuffer3InfoKHR* pInfo)
{
  static PFN_vkCmdBindIndexBuffer3KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindIndexBuffer3KHR)vkl_resolve("vkCmdBindIndexBuffer3KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBindIndexBuffer3KHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindInvocationMaskHUAWEI(VkCommandBuffer commandBuffer, VkImageView imageView, VkImageLayout imageLayout)
{
  static PFN_vkCmdBindInvocationMaskHUAWEI pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindInvocationMaskHUAWEI)vkl_resolve("vkCmdBindInvocationMaskHUAWEI");
    if(pfn == NULL)
      vkl_missing("vkCmdBindInvocationMaskHUAWEI");
    }
  pfn(commandBuffer, imageView, imageLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
  static PFN_vkCmdBindPipeline pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindPipeline)vkl_resolve("vkCmdBindPipeline");
    if(pfn == NULL)
      vkl_missing("vkCmdBindPipeline");
    }
  pfn(commandBuffer, pipelineBindPoint, pipeline);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindPipelineShaderGroupNV(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline, uint32_t groupIndex)
{
  static PFN_vkCmdBindPipelineShaderGroupNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindPipelineShaderGroupNV)vkl_resolve("vkCmdBindPipelineShaderGroupNV");
    if(pfn == NULL)
      vkl_missing("vkCmdBindPipelineShaderGroupNV");
    }
  pfn(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindResourceHeapEXT(VkCommandBuffer commandBuffer, const VkBindHeapInfoEXT* pBindInfo)
{
  static PFN_vkCmdBindResourceHeapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindResourceHeapEXT)vkl_resolve("vkCmdBindResourceHeapEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindResourceHeapEXT");
    }
  pfn(commandBuffer, pBindInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindSamplerHeapEXT(VkCommandBuffer commandBuffer, const VkBindHeapInfoEXT* pBindInfo)
{
  static PFN_vkCmdBindSamplerHeapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindSamplerHeapEXT)vkl_resolve("vkCmdBindSamplerHeapEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindSamplerHeapEXT");
    }
  pfn(commandBuffer, pBindInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindShadersEXT(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders)
{
  static PFN_vkCmdBindShadersEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindShadersEXT)vkl_resolve("vkCmdBindShadersEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindShadersEXT");
    }
  pfn(commandBuffer, stageCount, pStages, pShaders);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindShadingRateImageNV(VkCommandBuffer commandBuffer, VkImageView imageView, VkImageLayout imageLayout)
{
  static PFN_vkCmdBindShadingRateImageNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindShadingRateImageNV)vkl_resolve("vkCmdBindShadingRateImageNV");
    if(pfn == NULL)
      vkl_missing("vkCmdBindShadingRateImageNV");
    }
  pfn(commandBuffer, imageView, imageLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindTileMemoryQCOM(VkCommandBuffer commandBuffer, const VkTileMemoryBindInfoQCOM* pTileMemoryBindInfo)
{
  static PFN_vkCmdBindTileMemoryQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindTileMemoryQCOM)vkl_resolve("vkCmdBindTileMemoryQCOM");
    if(pfn == NULL)
      vkl_missing("vkCmdBindTileMemoryQCOM");
    }
  pfn(commandBuffer, pTileMemoryBindInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindTransformFeedbackBuffers2EXT(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBindTransformFeedbackBuffer2InfoEXT* pBindingInfos)
{
  static PFN_vkCmdBindTransformFeedbackBuffers2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindTransformFeedbackBuffers2EXT)vkl_resolve("vkCmdBindTransformFeedbackBuffers2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindTransformFeedbackBuffers2EXT");
    }
  pfn(commandBuffer, firstBinding, bindingCount, pBindingInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindTransformFeedbackBuffersEXT(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes)
{
  static PFN_vkCmdBindTransformFeedbackBuffersEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindTransformFeedbackBuffersEXT)vkl_resolve("vkCmdBindTransformFeedbackBuffersEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindTransformFeedbackBuffersEXT");
    }
  pfn(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
  static PFN_vkCmdBindVertexBuffers pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindVertexBuffers)vkl_resolve("vkCmdBindVertexBuffers");
    if(pfn == NULL)
      vkl_missing("vkCmdBindVertexBuffers");
    }
  pfn(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes, const VkDeviceSize* pStrides)
{
  static PFN_vkCmdBindVertexBuffers2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindVertexBuffers2)vkl_resolve("vkCmdBindVertexBuffers2");
    if(pfn == NULL)
      vkl_missing("vkCmdBindVertexBuffers2");
    }
  pfn(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindVertexBuffers2EXT(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes, const VkDeviceSize* pStrides)
{
  static PFN_vkCmdBindVertexBuffers2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindVertexBuffers2EXT)vkl_resolve("vkCmdBindVertexBuffers2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBindVertexBuffers2EXT");
    }
  pfn(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBindVertexBuffers3KHR(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBindVertexBuffer3InfoKHR* pBindingInfos)
{
  static PFN_vkCmdBindVertexBuffers3KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBindVertexBuffers3KHR)vkl_resolve("vkCmdBindVertexBuffers3KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBindVertexBuffers3KHR");
    }
  pfn(commandBuffer, firstBinding, bindingCount, pBindingInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
  static PFN_vkCmdBlitImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBlitImage)vkl_resolve("vkCmdBlitImage");
    if(pfn == NULL)
      vkl_missing("vkCmdBlitImage");
    }
  pfn(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo)
{
  static PFN_vkCmdBlitImage2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBlitImage2)vkl_resolve("vkCmdBlitImage2");
    if(pfn == NULL)
      vkl_missing("vkCmdBlitImage2");
    }
  pfn(commandBuffer, pBlitImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBlitImage2KHR(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo)
{
  static PFN_vkCmdBlitImage2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBlitImage2KHR)vkl_resolve("vkCmdBlitImage2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBlitImage2KHR");
    }
  pfn(commandBuffer, pBlitImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBuildAccelerationStructureNV(VkCommandBuffer commandBuffer, const VkAccelerationStructureInfoNV* pInfo, VkBuffer instanceData, VkDeviceSize instanceOffset, VkBool32 update, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkBuffer scratch, VkDeviceSize scratchOffset)
{
  static PFN_vkCmdBuildAccelerationStructureNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBuildAccelerationStructureNV)vkl_resolve("vkCmdBuildAccelerationStructureNV");
    if(pfn == NULL)
      vkl_missing("vkCmdBuildAccelerationStructureNV");
    }
  pfn(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBuildAccelerationStructuresIndirectKHR(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkDeviceAddress* pIndirectDeviceAddresses, const uint32_t* pIndirectStrides, const uint32_t* const* ppMaxPrimitiveCounts)
{
  static PFN_vkCmdBuildAccelerationStructuresIndirectKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)vkl_resolve("vkCmdBuildAccelerationStructuresIndirectKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBuildAccelerationStructuresIndirectKHR");
    }
  pfn(commandBuffer, infoCount, pInfos, pIndirectDeviceAddresses, pIndirectStrides, ppMaxPrimitiveCounts);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBuildAccelerationStructuresKHR(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos, const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
  static PFN_vkCmdBuildAccelerationStructuresKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBuildAccelerationStructuresKHR)vkl_resolve("vkCmdBuildAccelerationStructuresKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdBuildAccelerationStructuresKHR");
    }
  pfn(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBuildClusterAccelerationStructureIndirectNV(VkCommandBuffer commandBuffer, const VkClusterAccelerationStructureCommandsInfoNV* pCommandInfos)
{
  static PFN_vkCmdBuildClusterAccelerationStructureIndirectNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBuildClusterAccelerationStructureIndirectNV)vkl_resolve("vkCmdBuildClusterAccelerationStructureIndirectNV");
    if(pfn == NULL)
      vkl_missing("vkCmdBuildClusterAccelerationStructureIndirectNV");
    }
  pfn(commandBuffer, pCommandInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBuildMicromapsEXT(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkMicromapBuildInfoEXT* pInfos)
{
  static PFN_vkCmdBuildMicromapsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBuildMicromapsEXT)vkl_resolve("vkCmdBuildMicromapsEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdBuildMicromapsEXT");
    }
  pfn(commandBuffer, infoCount, pInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdBuildPartitionedAccelerationStructuresNV(VkCommandBuffer commandBuffer, const VkBuildPartitionedAccelerationStructureInfoNV* pBuildInfo)
{
  static PFN_vkCmdBuildPartitionedAccelerationStructuresNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdBuildPartitionedAccelerationStructuresNV)vkl_resolve("vkCmdBuildPartitionedAccelerationStructuresNV");
    if(pfn == NULL)
      vkl_missing("vkCmdBuildPartitionedAccelerationStructuresNV");
    }
  pfn(commandBuffer, pBuildInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects)
{
  static PFN_vkCmdClearAttachments pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdClearAttachments)vkl_resolve("vkCmdClearAttachments");
    if(pfn == NULL)
      vkl_missing("vkCmdClearAttachments");
    }
  pfn(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
  static PFN_vkCmdClearColorImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdClearColorImage)vkl_resolve("vkCmdClearColorImage");
    if(pfn == NULL)
      vkl_missing("vkCmdClearColorImage");
    }
  pfn(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
  static PFN_vkCmdClearDepthStencilImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdClearDepthStencilImage)vkl_resolve("vkCmdClearDepthStencilImage");
    if(pfn == NULL)
      vkl_missing("vkCmdClearDepthStencilImage");
    }
  pfn(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdControlVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoCodingControlInfoKHR* pCodingControlInfo)
{
  static PFN_vkCmdControlVideoCodingKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdControlVideoCodingKHR)vkl_resolve("vkCmdControlVideoCodingKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdControlVideoCodingKHR");
    }
  pfn(commandBuffer, pCodingControlInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdConvertCooperativeVectorMatrixNV(VkCommandBuffer commandBuffer, uint32_t infoCount, const VkConvertCooperativeVectorMatrixInfoNV* pInfos)
{
  static PFN_vkCmdConvertCooperativeVectorMatrixNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdConvertCooperativeVectorMatrixNV)vkl_resolve("vkCmdConvertCooperativeVectorMatrixNV");
    if(pfn == NULL)
      vkl_missing("vkCmdConvertCooperativeVectorMatrixNV");
    }
  pfn(commandBuffer, infoCount, pInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR* pInfo)
{
  static PFN_vkCmdCopyAccelerationStructureKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyAccelerationStructureKHR)vkl_resolve("vkCmdCopyAccelerationStructureKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyAccelerationStructureKHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyAccelerationStructureNV(VkCommandBuffer commandBuffer, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkCopyAccelerationStructureModeKHR mode)
{
  static PFN_vkCmdCopyAccelerationStructureNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyAccelerationStructureNV)vkl_resolve("vkCmdCopyAccelerationStructureNV");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyAccelerationStructureNV");
    }
  pfn(commandBuffer, dst, src, mode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyAccelerationStructureToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
  static PFN_vkCmdCopyAccelerationStructureToMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)vkl_resolve("vkCmdCopyAccelerationStructureToMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyAccelerationStructureToMemoryKHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
  static PFN_vkCmdCopyBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyBuffer)vkl_resolve("vkCmdCopyBuffer");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyBuffer");
    }
  pfn(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo)
{
  static PFN_vkCmdCopyBuffer2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyBuffer2)vkl_resolve("vkCmdCopyBuffer2");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyBuffer2");
    }
  pfn(commandBuffer, pCopyBufferInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBuffer2KHR(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo)
{
  static PFN_vkCmdCopyBuffer2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyBuffer2KHR)vkl_resolve("vkCmdCopyBuffer2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyBuffer2KHR");
    }
  pfn(commandBuffer, pCopyBufferInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
  static PFN_vkCmdCopyBufferToImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyBufferToImage)vkl_resolve("vkCmdCopyBufferToImage");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyBufferToImage");
    }
  pfn(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBufferToImage2(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo)
{
  static PFN_vkCmdCopyBufferToImage2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyBufferToImage2)vkl_resolve("vkCmdCopyBufferToImage2");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyBufferToImage2");
    }
  pfn(commandBuffer, pCopyBufferToImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyBufferToImage2KHR(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo)
{
  static PFN_vkCmdCopyBufferToImage2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyBufferToImage2KHR)vkl_resolve("vkCmdCopyBufferToImage2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyBufferToImage2KHR");
    }
  pfn(commandBuffer, pCopyBufferToImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyGpaSessionResultsAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD gpaSession)
{
  static PFN_vkCmdCopyGpaSessionResultsAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyGpaSessionResultsAMD)vkl_resolve("vkCmdCopyGpaSessionResultsAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyGpaSessionResultsAMD");
    }
  pfn(commandBuffer, gpaSession);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions)
{
  static PFN_vkCmdCopyImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImage)vkl_resolve("vkCmdCopyImage");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImage");
    }
  pfn(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo)
{
  static PFN_vkCmdCopyImage2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImage2)vkl_resolve("vkCmdCopyImage2");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImage2");
    }
  pfn(commandBuffer, pCopyImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImage2KHR(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo)
{
  static PFN_vkCmdCopyImage2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImage2KHR)vkl_resolve("vkCmdCopyImage2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImage2KHR");
    }
  pfn(commandBuffer, pCopyImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
  static PFN_vkCmdCopyImageToBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImageToBuffer)vkl_resolve("vkCmdCopyImageToBuffer");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImageToBuffer");
    }
  pfn(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImageToBuffer2(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo)
{
  static PFN_vkCmdCopyImageToBuffer2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImageToBuffer2)vkl_resolve("vkCmdCopyImageToBuffer2");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImageToBuffer2");
    }
  pfn(commandBuffer, pCopyImageToBufferInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImageToBuffer2KHR(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo)
{
  static PFN_vkCmdCopyImageToBuffer2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImageToBuffer2KHR)vkl_resolve("vkCmdCopyImageToBuffer2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImageToBuffer2KHR");
    }
  pfn(commandBuffer, pCopyImageToBufferInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImageToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo)
{
  static PFN_vkCmdCopyImageToMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyImageToMemoryKHR)vkl_resolve("vkCmdCopyImageToMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyImageToMemoryKHR");
    }
  pfn(commandBuffer, pCopyMemoryInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryIndirectKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryIndirectInfoKHR* pCopyMemoryIndirectInfo)
{
  static PFN_vkCmdCopyMemoryIndirectKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryIndirectKHR)vkl_resolve("vkCmdCopyMemoryIndirectKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryIndirectKHR");
    }
  pfn(commandBuffer, pCopyMemoryIndirectInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryIndirectNV(VkCommandBuffer commandBuffer, VkDeviceAddress copyBufferAddress, uint32_t copyCount, uint32_t stride)
{
  static PFN_vkCmdCopyMemoryIndirectNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryIndirectNV)vkl_resolve("vkCmdCopyMemoryIndirectNV");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryIndirectNV");
    }
  pfn(commandBuffer, copyBufferAddress, copyCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryInfoKHR* pCopyMemoryInfo)
{
  static PFN_vkCmdCopyMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryKHR)vkl_resolve("vkCmdCopyMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryKHR");
    }
  pfn(commandBuffer, pCopyMemoryInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryToAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
  static PFN_vkCmdCopyMemoryToAccelerationStructureKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)vkl_resolve("vkCmdCopyMemoryToAccelerationStructureKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryToAccelerationStructureKHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryToImageIndirectKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToImageIndirectInfoKHR* pCopyMemoryToImageIndirectInfo)
{
  static PFN_vkCmdCopyMemoryToImageIndirectKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryToImageIndirectKHR)vkl_resolve("vkCmdCopyMemoryToImageIndirectKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryToImageIndirectKHR");
    }
  pfn(commandBuffer, pCopyMemoryToImageIndirectInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryToImageIndirectNV(VkCommandBuffer commandBuffer, VkDeviceAddress copyBufferAddress, uint32_t copyCount, uint32_t stride, VkImage dstImage, VkImageLayout dstImageLayout, const VkImageSubresourceLayers* pImageSubresources)
{
  static PFN_vkCmdCopyMemoryToImageIndirectNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryToImageIndirectNV)vkl_resolve("vkCmdCopyMemoryToImageIndirectNV");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryToImageIndirectNV");
    }
  pfn(commandBuffer, copyBufferAddress, copyCount, stride, dstImage, dstImageLayout, pImageSubresources);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryToImageKHR(VkCommandBuffer commandBuffer, const VkCopyDeviceMemoryImageInfoKHR* pCopyMemoryInfo)
{
  static PFN_vkCmdCopyMemoryToImageKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryToImageKHR)vkl_resolve("vkCmdCopyMemoryToImageKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryToImageKHR");
    }
  pfn(commandBuffer, pCopyMemoryInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryToMicromapEXT(VkCommandBuffer commandBuffer, const VkCopyMemoryToMicromapInfoEXT* pInfo)
{
  static PFN_vkCmdCopyMemoryToMicromapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMemoryToMicromapEXT)vkl_resolve("vkCmdCopyMemoryToMicromapEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMemoryToMicromapEXT");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMicromapEXT(VkCommandBuffer commandBuffer, const VkCopyMicromapInfoEXT* pInfo)
{
  static PFN_vkCmdCopyMicromapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMicromapEXT)vkl_resolve("vkCmdCopyMicromapEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMicromapEXT");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMicromapToMemoryEXT(VkCommandBuffer commandBuffer, const VkCopyMicromapToMemoryInfoEXT* pInfo)
{
  static PFN_vkCmdCopyMicromapToMemoryEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyMicromapToMemoryEXT)vkl_resolve("vkCmdCopyMicromapToMemoryEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyMicromapToMemoryEXT");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
  static PFN_vkCmdCopyQueryPoolResults pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyQueryPoolResults)vkl_resolve("vkCmdCopyQueryPoolResults");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyQueryPoolResults");
    }
  pfn(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyQueryPoolResultsToMemoryKHR(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, const VkStridedDeviceAddressRangeKHR* pDstRange, VkAddressCommandFlagsKHR dstFlags, VkQueryResultFlags queryResultFlags)
{
  static PFN_vkCmdCopyQueryPoolResultsToMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyQueryPoolResultsToMemoryKHR)vkl_resolve("vkCmdCopyQueryPoolResultsToMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyQueryPoolResultsToMemoryKHR");
    }
  pfn(commandBuffer, queryPool, firstQuery, queryCount, pDstRange, dstFlags, queryResultFlags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCopyTensorARM(VkCommandBuffer commandBuffer, const VkCopyTensorInfoARM* pCopyTensorInfo)
{
  static PFN_vkCmdCopyTensorARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCopyTensorARM)vkl_resolve("vkCmdCopyTensorARM");
    if(pfn == NULL)
      vkl_missing("vkCmdCopyTensorARM");
    }
  pfn(commandBuffer, pCopyTensorInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdCuLaunchKernelNVX(VkCommandBuffer commandBuffer, const VkCuLaunchInfoNVX* pLaunchInfo)
{
  static PFN_vkCmdCuLaunchKernelNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdCuLaunchKernelNVX)vkl_resolve("vkCmdCuLaunchKernelNVX");
    if(pfn == NULL)
      vkl_missing("vkCmdCuLaunchKernelNVX");
    }
  pfn(commandBuffer, pLaunchInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDebugMarkerBeginEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
  static PFN_vkCmdDebugMarkerBeginEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDebugMarkerBeginEXT)vkl_resolve("vkCmdDebugMarkerBeginEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDebugMarkerBeginEXT");
    }
  pfn(commandBuffer, pMarkerInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDebugMarkerEndEXT(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdDebugMarkerEndEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDebugMarkerEndEXT)vkl_resolve("vkCmdDebugMarkerEndEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDebugMarkerEndEXT");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDebugMarkerInsertEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
  static PFN_vkCmdDebugMarkerInsertEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDebugMarkerInsertEXT)vkl_resolve("vkCmdDebugMarkerInsertEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDebugMarkerInsertEXT");
    }
  pfn(commandBuffer, pMarkerInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDecodeVideoKHR(VkCommandBuffer commandBuffer, const VkVideoDecodeInfoKHR* pDecodeInfo)
{
  static PFN_vkCmdDecodeVideoKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDecodeVideoKHR)vkl_resolve("vkCmdDecodeVideoKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDecodeVideoKHR");
    }
  pfn(commandBuffer, pDecodeInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDecompressMemoryEXT(VkCommandBuffer commandBuffer, const VkDecompressMemoryInfoEXT* pDecompressMemoryInfoEXT)
{
  static PFN_vkCmdDecompressMemoryEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDecompressMemoryEXT)vkl_resolve("vkCmdDecompressMemoryEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDecompressMemoryEXT");
    }
  pfn(commandBuffer, pDecompressMemoryInfoEXT);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDecompressMemoryIndirectCountEXT(VkCommandBuffer commandBuffer, VkMemoryDecompressionMethodFlagsEXT decompressionMethod, VkDeviceAddress indirectCommandsAddress, VkDeviceAddress indirectCommandsCountAddress, uint32_t maxDecompressionCount, uint32_t stride)
{
  static PFN_vkCmdDecompressMemoryIndirectCountEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDecompressMemoryIndirectCountEXT)vkl_resolve("vkCmdDecompressMemoryIndirectCountEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDecompressMemoryIndirectCountEXT");
    }
  pfn(commandBuffer, decompressionMethod, indirectCommandsAddress, indirectCommandsCountAddress, maxDecompressionCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDecompressMemoryIndirectCountNV(VkCommandBuffer commandBuffer, VkDeviceAddress indirectCommandsAddress, VkDeviceAddress indirectCommandsCountAddress, uint32_t stride)
{
  static PFN_vkCmdDecompressMemoryIndirectCountNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDecompressMemoryIndirectCountNV)vkl_resolve("vkCmdDecompressMemoryIndirectCountNV");
    if(pfn == NULL)
      vkl_missing("vkCmdDecompressMemoryIndirectCountNV");
    }
  pfn(commandBuffer, indirectCommandsAddress, indirectCommandsCountAddress, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDecompressMemoryNV(VkCommandBuffer commandBuffer, uint32_t decompressRegionCount, const VkDecompressMemoryRegionNV* pDecompressMemoryRegions)
{
  static PFN_vkCmdDecompressMemoryNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDecompressMemoryNV)vkl_resolve("vkCmdDecompressMemoryNV");
    if(pfn == NULL)
      vkl_missing("vkCmdDecompressMemoryNV");
    }
  pfn(commandBuffer, decompressRegionCount, pDecompressMemoryRegions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
  static PFN_vkCmdDispatch pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatch)vkl_resolve("vkCmdDispatch");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatch");
    }
  pfn(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
  static PFN_vkCmdDispatchBase pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatchBase)vkl_resolve("vkCmdDispatchBase");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatchBase");
    }
  pfn(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchBaseKHR(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
  static PFN_vkCmdDispatchBaseKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatchBaseKHR)vkl_resolve("vkCmdDispatchBaseKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatchBaseKHR");
    }
  pfn(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchDataGraphARM(VkCommandBuffer commandBuffer, VkDataGraphPipelineSessionARM session, const VkDataGraphPipelineDispatchInfoARM* pInfo)
{
  static PFN_vkCmdDispatchDataGraphARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatchDataGraphARM)vkl_resolve("vkCmdDispatchDataGraphARM");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatchDataGraphARM");
    }
  pfn(commandBuffer, session, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
  static PFN_vkCmdDispatchIndirect pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatchIndirect)vkl_resolve("vkCmdDispatchIndirect");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatchIndirect");
    }
  pfn(commandBuffer, buffer, offset);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchIndirect2KHR(VkCommandBuffer commandBuffer, const VkDispatchIndirect2InfoKHR* pInfo)
{
  static PFN_vkCmdDispatchIndirect2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatchIndirect2KHR)vkl_resolve("vkCmdDispatchIndirect2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatchIndirect2KHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchTileQCOM(VkCommandBuffer commandBuffer, const VkDispatchTileInfoQCOM* pDispatchTileInfo)
{
  static PFN_vkCmdDispatchTileQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDispatchTileQCOM)vkl_resolve("vkCmdDispatchTileQCOM");
    if(pfn == NULL)
      vkl_missing("vkCmdDispatchTileQCOM");
    }
  pfn(commandBuffer, pDispatchTileInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
  static PFN_vkCmdDraw pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDraw)vkl_resolve("vkCmdDraw");
    if(pfn == NULL)
      vkl_missing("vkCmdDraw");
    }
  pfn(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawClusterHUAWEI(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
  static PFN_vkCmdDrawClusterHUAWEI pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawClusterHUAWEI)vkl_resolve("vkCmdDrawClusterHUAWEI");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawClusterHUAWEI");
    }
  pfn(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawClusterIndirectHUAWEI(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
  static PFN_vkCmdDrawClusterIndirectHUAWEI pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawClusterIndirectHUAWEI)vkl_resolve("vkCmdDrawClusterIndirectHUAWEI");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawClusterIndirectHUAWEI");
    }
  pfn(commandBuffer, buffer, offset);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
  static PFN_vkCmdDrawIndexed pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexed)vkl_resolve("vkCmdDrawIndexed");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexed");
    }
  pfn(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndexedIndirect pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexedIndirect)vkl_resolve("vkCmdDrawIndexedIndirect");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexedIndirect");
    }
  pfn(commandBuffer, buffer, offset, drawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirect2KHR(VkCommandBuffer commandBuffer, const VkDrawIndirect2InfoKHR* pInfo)
{
  static PFN_vkCmdDrawIndexedIndirect2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexedIndirect2KHR)vkl_resolve("vkCmdDrawIndexedIndirect2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexedIndirect2KHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndexedIndirectCount pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexedIndirectCount)vkl_resolve("vkCmdDrawIndexedIndirectCount");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexedIndirectCount");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirectCount2KHR(VkCommandBuffer commandBuffer, const VkDrawIndirectCount2InfoKHR* pInfo)
{
  static PFN_vkCmdDrawIndexedIndirectCount2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexedIndirectCount2KHR)vkl_resolve("vkCmdDrawIndexedIndirectCount2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexedIndirectCount2KHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndexedIndirectCountAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexedIndirectCountAMD)vkl_resolve("vkCmdDrawIndexedIndirectCountAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexedIndirectCountAMD");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirectCountKHR(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndexedIndirectCountKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndexedIndirectCountKHR)vkl_resolve("vkCmdDrawIndexedIndirectCountKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndexedIndirectCountKHR");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndirect pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirect)vkl_resolve("vkCmdDrawIndirect");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirect");
    }
  pfn(commandBuffer, buffer, offset, drawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirect2KHR(VkCommandBuffer commandBuffer, const VkDrawIndirect2InfoKHR* pInfo)
{
  static PFN_vkCmdDrawIndirect2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirect2KHR)vkl_resolve("vkCmdDrawIndirect2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirect2KHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirectByteCount2EXT(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfo, uint32_t counterOffset, uint32_t vertexStride)
{
  static PFN_vkCmdDrawIndirectByteCount2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirectByteCount2EXT)vkl_resolve("vkCmdDrawIndirectByteCount2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirectByteCount2EXT");
    }
  pfn(commandBuffer, instanceCount, firstInstance, pCounterInfo, counterOffset, vertexStride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirectByteCountEXT(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance, VkBuffer counterBuffer, VkDeviceSize counterBufferOffset, uint32_t counterOffset, uint32_t vertexStride)
{
  static PFN_vkCmdDrawIndirectByteCountEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirectByteCountEXT)vkl_resolve("vkCmdDrawIndirectByteCountEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirectByteCountEXT");
    }
  pfn(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndirectCount pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirectCount)vkl_resolve("vkCmdDrawIndirectCount");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirectCount");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirectCount2KHR(VkCommandBuffer commandBuffer, const VkDrawIndirectCount2InfoKHR* pInfo)
{
  static PFN_vkCmdDrawIndirectCount2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirectCount2KHR)vkl_resolve("vkCmdDrawIndirectCount2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirectCount2KHR");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndirectCountAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirectCountAMD)vkl_resolve("vkCmdDrawIndirectCountAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirectCountAMD");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirectCountKHR(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawIndirectCountKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawIndirectCountKHR)vkl_resolve("vkCmdDrawIndirectCountKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawIndirectCountKHR");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksEXT(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
  static PFN_vkCmdDrawMeshTasksEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksEXT)vkl_resolve("vkCmdDrawMeshTasksEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksEXT");
    }
  pfn(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksIndirect2EXT(VkCommandBuffer commandBuffer, const VkDrawIndirect2InfoKHR* pInfo)
{
  static PFN_vkCmdDrawMeshTasksIndirect2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksIndirect2EXT)vkl_resolve("vkCmdDrawMeshTasksIndirect2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksIndirect2EXT");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksIndirectCount2EXT(VkCommandBuffer commandBuffer, const VkDrawIndirectCount2InfoKHR* pInfo)
{
  static PFN_vkCmdDrawMeshTasksIndirectCount2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksIndirectCount2EXT)vkl_resolve("vkCmdDrawMeshTasksIndirectCount2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksIndirectCount2EXT");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksIndirectCountEXT(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawMeshTasksIndirectCountEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksIndirectCountEXT)vkl_resolve("vkCmdDrawMeshTasksIndirectCountEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksIndirectCountEXT");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksIndirectCountNV(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
  static PFN_vkCmdDrawMeshTasksIndirectCountNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksIndirectCountNV)vkl_resolve("vkCmdDrawMeshTasksIndirectCountNV");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksIndirectCountNV");
    }
  pfn(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksIndirectEXT(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
  static PFN_vkCmdDrawMeshTasksIndirectEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksIndirectEXT)vkl_resolve("vkCmdDrawMeshTasksIndirectEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksIndirectEXT");
    }
  pfn(commandBuffer, buffer, offset, drawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksIndirectNV(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
  static PFN_vkCmdDrawMeshTasksIndirectNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksIndirectNV)vkl_resolve("vkCmdDrawMeshTasksIndirectNV");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksIndirectNV");
    }
  pfn(commandBuffer, buffer, offset, drawCount, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMeshTasksNV(VkCommandBuffer commandBuffer, uint32_t taskCount, uint32_t firstTask)
{
  static PFN_vkCmdDrawMeshTasksNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMeshTasksNV)vkl_resolve("vkCmdDrawMeshTasksNV");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMeshTasksNV");
    }
  pfn(commandBuffer, taskCount, firstTask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMultiEXT(VkCommandBuffer commandBuffer, uint32_t drawCount, const VkMultiDrawInfoEXT* pVertexInfo, uint32_t instanceCount, uint32_t firstInstance, uint32_t stride)
{
  static PFN_vkCmdDrawMultiEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMultiEXT)vkl_resolve("vkCmdDrawMultiEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMultiEXT");
    }
  pfn(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdDrawMultiIndexedEXT(VkCommandBuffer commandBuffer, uint32_t drawCount, const VkMultiDrawIndexedInfoEXT* pIndexInfo, uint32_t instanceCount, uint32_t firstInstance, uint32_t stride, const int32_t* pVertexOffset)
{
  static PFN_vkCmdDrawMultiIndexedEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdDrawMultiIndexedEXT)vkl_resolve("vkCmdDrawMultiIndexedEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdDrawMultiIndexedEXT");
    }
  pfn(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEncodeVideoKHR(VkCommandBuffer commandBuffer, const VkVideoEncodeInfoKHR* pEncodeInfo)
{
  static PFN_vkCmdEncodeVideoKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEncodeVideoKHR)vkl_resolve("vkCmdEncodeVideoKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdEncodeVideoKHR");
    }
  pfn(commandBuffer, pEncodeInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndConditionalRenderingEXT(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdEndConditionalRenderingEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndConditionalRenderingEXT)vkl_resolve("vkCmdEndConditionalRenderingEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdEndConditionalRenderingEXT");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdEndDebugUtilsLabelEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndDebugUtilsLabelEXT)vkl_resolve("vkCmdEndDebugUtilsLabelEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdEndDebugUtilsLabelEXT");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndGpaSampleAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD gpaSession, uint32_t sampleID)
{
  static PFN_vkCmdEndGpaSampleAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndGpaSampleAMD)vkl_resolve("vkCmdEndGpaSampleAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdEndGpaSampleAMD");
    }
  pfn(commandBuffer, gpaSession, sampleID);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCmdEndGpaSessionAMD(VkCommandBuffer commandBuffer, VkGpaSessionAMD gpaSession)
{
  static PFN_vkCmdEndGpaSessionAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndGpaSessionAMD)vkl_resolve("vkCmdEndGpaSessionAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdEndGpaSessionAMD");
    }
  return pfn(commandBuffer, gpaSession);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndPerTileExecutionQCOM(VkCommandBuffer commandBuffer, const VkPerTileEndInfoQCOM* pPerTileEndInfo)
{
  static PFN_vkCmdEndPerTileExecutionQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndPerTileExecutionQCOM)vkl_resolve("vkCmdEndPerTileExecutionQCOM");
    if(pfn == NULL)
      vkl_missing("vkCmdEndPerTileExecutionQCOM");
    }
  pfn(commandBuffer, pPerTileEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
  static PFN_vkCmdEndQuery pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndQuery)vkl_resolve("vkCmdEndQuery");
    if(pfn == NULL)
      vkl_missing("vkCmdEndQuery");
    }
  pfn(commandBuffer, queryPool, query);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, uint32_t index)
{
  static PFN_vkCmdEndQueryIndexedEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndQueryIndexedEXT)vkl_resolve("vkCmdEndQueryIndexedEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdEndQueryIndexedEXT");
    }
  pfn(commandBuffer, queryPool, query, index);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRenderPass(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdEndRenderPass pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRenderPass)vkl_resolve("vkCmdEndRenderPass");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRenderPass");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRenderPass2(VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo)
{
  static PFN_vkCmdEndRenderPass2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRenderPass2)vkl_resolve("vkCmdEndRenderPass2");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRenderPass2");
    }
  pfn(commandBuffer, pSubpassEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRenderPass2KHR(VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo)
{
  static PFN_vkCmdEndRenderPass2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRenderPass2KHR)vkl_resolve("vkCmdEndRenderPass2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRenderPass2KHR");
    }
  pfn(commandBuffer, pSubpassEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRendering(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdEndRendering pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRendering)vkl_resolve("vkCmdEndRendering");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRendering");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRendering2EXT(VkCommandBuffer commandBuffer, const VkRenderingEndInfoKHR* pRenderingEndInfo)
{
  static PFN_vkCmdEndRendering2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRendering2EXT)vkl_resolve("vkCmdEndRendering2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRendering2EXT");
    }
  pfn(commandBuffer, pRenderingEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRendering2KHR(VkCommandBuffer commandBuffer, const VkRenderingEndInfoKHR* pRenderingEndInfo)
{
  static PFN_vkCmdEndRendering2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRendering2KHR)vkl_resolve("vkCmdEndRendering2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRendering2KHR");
    }
  pfn(commandBuffer, pRenderingEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndRenderingKHR(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdEndRenderingKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndRenderingKHR)vkl_resolve("vkCmdEndRenderingKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdEndRenderingKHR");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndShaderInstrumentationARM(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdEndShaderInstrumentationARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndShaderInstrumentationARM)vkl_resolve("vkCmdEndShaderInstrumentationARM");
    if(pfn == NULL)
      vkl_missing("vkCmdEndShaderInstrumentationARM");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndTransformFeedback2EXT(VkCommandBuffer commandBuffer, uint32_t firstCounterRange, uint32_t counterRangeCount, const VkBindTransformFeedbackBuffer2InfoEXT* pCounterInfos)
{
  static PFN_vkCmdEndTransformFeedback2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndTransformFeedback2EXT)vkl_resolve("vkCmdEndTransformFeedback2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdEndTransformFeedback2EXT");
    }
  pfn(commandBuffer, firstCounterRange, counterRangeCount, pCounterInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer, uint32_t counterBufferCount, const VkBuffer* pCounterBuffers, const VkDeviceSize* pCounterBufferOffsets)
{
  static PFN_vkCmdEndTransformFeedbackEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndTransformFeedbackEXT)vkl_resolve("vkCmdEndTransformFeedbackEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdEndTransformFeedbackEXT");
    }
  pfn(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdEndVideoCodingKHR(VkCommandBuffer commandBuffer, const VkVideoEndCodingInfoKHR* pEndCodingInfo)
{
  static PFN_vkCmdEndVideoCodingKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdEndVideoCodingKHR)vkl_resolve("vkCmdEndVideoCodingKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdEndVideoCodingKHR");
    }
  pfn(commandBuffer, pEndCodingInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
  static PFN_vkCmdExecuteCommands pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdExecuteCommands)vkl_resolve("vkCmdExecuteCommands");
    if(pfn == NULL)
      vkl_missing("vkCmdExecuteCommands");
    }
  pfn(commandBuffer, commandBufferCount, pCommandBuffers);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdExecuteGeneratedCommandsEXT(VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo)
{
  static PFN_vkCmdExecuteGeneratedCommandsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdExecuteGeneratedCommandsEXT)vkl_resolve("vkCmdExecuteGeneratedCommandsEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdExecuteGeneratedCommandsEXT");
    }
  pfn(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdExecuteGeneratedCommandsNV(VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
  static PFN_vkCmdExecuteGeneratedCommandsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdExecuteGeneratedCommandsNV)vkl_resolve("vkCmdExecuteGeneratedCommandsNV");
    if(pfn == NULL)
      vkl_missing("vkCmdExecuteGeneratedCommandsNV");
    }
  pfn(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
  static PFN_vkCmdFillBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdFillBuffer)vkl_resolve("vkCmdFillBuffer");
    if(pfn == NULL)
      vkl_missing("vkCmdFillBuffer");
    }
  pfn(commandBuffer, dstBuffer, dstOffset, size, data);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdFillMemoryKHR(VkCommandBuffer commandBuffer, const VkDeviceAddressRangeKHR* pDstRange, VkAddressCommandFlagsKHR dstFlags, uint32_t data)
{
  static PFN_vkCmdFillMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdFillMemoryKHR)vkl_resolve("vkCmdFillMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdFillMemoryKHR");
    }
  pfn(commandBuffer, pDstRange, dstFlags, data);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
  static PFN_vkCmdInsertDebugUtilsLabelEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkl_resolve("vkCmdInsertDebugUtilsLabelEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdInsertDebugUtilsLabelEXT");
    }
  pfn(commandBuffer, pLabelInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
  static PFN_vkCmdNextSubpass pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdNextSubpass)vkl_resolve("vkCmdNextSubpass");
    if(pfn == NULL)
      vkl_missing("vkCmdNextSubpass");
    }
  pfn(commandBuffer, contents);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdNextSubpass2(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo)
{
  static PFN_vkCmdNextSubpass2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdNextSubpass2)vkl_resolve("vkCmdNextSubpass2");
    if(pfn == NULL)
      vkl_missing("vkCmdNextSubpass2");
    }
  pfn(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdNextSubpass2KHR(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo)
{
  static PFN_vkCmdNextSubpass2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdNextSubpass2KHR)vkl_resolve("vkCmdNextSubpass2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdNextSubpass2KHR");
    }
  pfn(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdOpticalFlowExecuteNV(VkCommandBuffer commandBuffer, VkOpticalFlowSessionNV session, const VkOpticalFlowExecuteInfoNV* pExecuteInfo)
{
  static PFN_vkCmdOpticalFlowExecuteNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdOpticalFlowExecuteNV)vkl_resolve("vkCmdOpticalFlowExecuteNV");
    if(pfn == NULL)
      vkl_missing("vkCmdOpticalFlowExecuteNV");
    }
  pfn(commandBuffer, session, pExecuteInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
  static PFN_vkCmdPipelineBarrier pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPipelineBarrier)vkl_resolve("vkCmdPipelineBarrier");
    if(pfn == NULL)
      vkl_missing("vkCmdPipelineBarrier");
    }
  pfn(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo)
{
  static PFN_vkCmdPipelineBarrier2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPipelineBarrier2)vkl_resolve("vkCmdPipelineBarrier2");
    if(pfn == NULL)
      vkl_missing("vkCmdPipelineBarrier2");
    }
  pfn(commandBuffer, pDependencyInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPipelineBarrier2KHR(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo)
{
  static PFN_vkCmdPipelineBarrier2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPipelineBarrier2KHR)vkl_resolve("vkCmdPipelineBarrier2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdPipelineBarrier2KHR");
    }
  pfn(commandBuffer, pDependencyInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPreprocessGeneratedCommandsEXT(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo, VkCommandBuffer stateCommandBuffer)
{
  static PFN_vkCmdPreprocessGeneratedCommandsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPreprocessGeneratedCommandsEXT)vkl_resolve("vkCmdPreprocessGeneratedCommandsEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdPreprocessGeneratedCommandsEXT");
    }
  pfn(commandBuffer, pGeneratedCommandsInfo, stateCommandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPreprocessGeneratedCommandsNV(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
  static PFN_vkCmdPreprocessGeneratedCommandsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPreprocessGeneratedCommandsNV)vkl_resolve("vkCmdPreprocessGeneratedCommandsNV");
    if(pfn == NULL)
      vkl_missing("vkCmdPreprocessGeneratedCommandsNV");
    }
  pfn(commandBuffer, pGeneratedCommandsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues)
{
  static PFN_vkCmdPushConstants pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushConstants)vkl_resolve("vkCmdPushConstants");
    if(pfn == NULL)
      vkl_missing("vkCmdPushConstants");
    }
  pfn(commandBuffer, layout, stageFlags, offset, size, pValues);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushConstants2(VkCommandBuffer commandBuffer, const VkPushConstantsInfo* pPushConstantsInfo)
{
  static PFN_vkCmdPushConstants2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushConstants2)vkl_resolve("vkCmdPushConstants2");
    if(pfn == NULL)
      vkl_missing("vkCmdPushConstants2");
    }
  pfn(commandBuffer, pPushConstantsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushConstants2KHR(VkCommandBuffer commandBuffer, const VkPushConstantsInfo* pPushConstantsInfo)
{
  static PFN_vkCmdPushConstants2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushConstants2KHR)vkl_resolve("vkCmdPushConstants2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdPushConstants2KHR");
    }
  pfn(commandBuffer, pPushConstantsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDataEXT(VkCommandBuffer commandBuffer, const VkPushDataInfoEXT* pPushDataInfo)
{
  static PFN_vkCmdPushDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDataEXT)vkl_resolve("vkCmdPushDataEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDataEXT");
    }
  pfn(commandBuffer, pPushDataInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
  static PFN_vkCmdPushDescriptorSet pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSet)vkl_resolve("vkCmdPushDescriptorSet");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSet");
    }
  pfn(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSet2(VkCommandBuffer commandBuffer, const VkPushDescriptorSetInfo* pPushDescriptorSetInfo)
{
  static PFN_vkCmdPushDescriptorSet2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSet2)vkl_resolve("vkCmdPushDescriptorSet2");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSet2");
    }
  pfn(commandBuffer, pPushDescriptorSetInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSet2KHR(VkCommandBuffer commandBuffer, const VkPushDescriptorSetInfo* pPushDescriptorSetInfo)
{
  static PFN_vkCmdPushDescriptorSet2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSet2KHR)vkl_resolve("vkCmdPushDescriptorSet2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSet2KHR");
    }
  pfn(commandBuffer, pPushDescriptorSetInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSetKHR(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
  static PFN_vkCmdPushDescriptorSetKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSetKHR)vkl_resolve("vkCmdPushDescriptorSetKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSetKHR");
    }
  pfn(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSetWithTemplate(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData)
{
  static PFN_vkCmdPushDescriptorSetWithTemplate pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSetWithTemplate)vkl_resolve("vkCmdPushDescriptorSetWithTemplate");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSetWithTemplate");
    }
  pfn(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSetWithTemplate2(VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo)
{
  static PFN_vkCmdPushDescriptorSetWithTemplate2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSetWithTemplate2)vkl_resolve("vkCmdPushDescriptorSetWithTemplate2");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSetWithTemplate2");
    }
  pfn(commandBuffer, pPushDescriptorSetWithTemplateInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSetWithTemplate2KHR(VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo)
{
  static PFN_vkCmdPushDescriptorSetWithTemplate2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSetWithTemplate2KHR)vkl_resolve("vkCmdPushDescriptorSetWithTemplate2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSetWithTemplate2KHR");
    }
  pfn(commandBuffer, pPushDescriptorSetWithTemplateInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSetWithTemplateKHR(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData)
{
  static PFN_vkCmdPushDescriptorSetWithTemplateKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdPushDescriptorSetWithTemplateKHR)vkl_resolve("vkCmdPushDescriptorSetWithTemplateKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdPushDescriptorSetWithTemplateKHR");
    }
  pfn(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
  static PFN_vkCmdResetEvent pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResetEvent)vkl_resolve("vkCmdResetEvent");
    if(pfn == NULL)
      vkl_missing("vkCmdResetEvent");
    }
  pfn(commandBuffer, event, stageMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResetEvent2(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2 stageMask)
{
  static PFN_vkCmdResetEvent2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResetEvent2)vkl_resolve("vkCmdResetEvent2");
    if(pfn == NULL)
      vkl_missing("vkCmdResetEvent2");
    }
  pfn(commandBuffer, event, stageMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResetEvent2KHR(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2 stageMask)
{
  static PFN_vkCmdResetEvent2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResetEvent2KHR)vkl_resolve("vkCmdResetEvent2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdResetEvent2KHR");
    }
  pfn(commandBuffer, event, stageMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
  static PFN_vkCmdResetQueryPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResetQueryPool)vkl_resolve("vkCmdResetQueryPool");
    if(pfn == NULL)
      vkl_missing("vkCmdResetQueryPool");
    }
  pfn(commandBuffer, queryPool, firstQuery, queryCount);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions)
{
  static PFN_vkCmdResolveImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResolveImage)vkl_resolve("vkCmdResolveImage");
    if(pfn == NULL)
      vkl_missing("vkCmdResolveImage");
    }
  pfn(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResolveImage2(VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo)
{
  static PFN_vkCmdResolveImage2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResolveImage2)vkl_resolve("vkCmdResolveImage2");
    if(pfn == NULL)
      vkl_missing("vkCmdResolveImage2");
    }
  pfn(commandBuffer, pResolveImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdResolveImage2KHR(VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo)
{
  static PFN_vkCmdResolveImage2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdResolveImage2KHR)vkl_resolve("vkCmdResolveImage2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdResolveImage2KHR");
    }
  pfn(commandBuffer, pResolveImageInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToCoverageEnable)
{
  static PFN_vkCmdSetAlphaToCoverageEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetAlphaToCoverageEnableEXT)vkl_resolve("vkCmdSetAlphaToCoverageEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetAlphaToCoverageEnableEXT");
    }
  pfn(commandBuffer, alphaToCoverageEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetAlphaToOneEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToOneEnable)
{
  static PFN_vkCmdSetAlphaToOneEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetAlphaToOneEnableEXT)vkl_resolve("vkCmdSetAlphaToOneEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetAlphaToOneEnableEXT");
    }
  pfn(commandBuffer, alphaToOneEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetAttachmentFeedbackLoopEnableEXT(VkCommandBuffer commandBuffer, VkImageAspectFlags aspectMask)
{
  static PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT)vkl_resolve("vkCmdSetAttachmentFeedbackLoopEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetAttachmentFeedbackLoopEnableEXT");
    }
  pfn(commandBuffer, aspectMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
  static PFN_vkCmdSetBlendConstants pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetBlendConstants)vkl_resolve("vkCmdSetBlendConstants");
    if(pfn == NULL)
      vkl_missing("vkCmdSetBlendConstants");
    }
  pfn(commandBuffer, blendConstants);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCheckpointNV(VkCommandBuffer commandBuffer, const void* pCheckpointMarker)
{
  static PFN_vkCmdSetCheckpointNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCheckpointNV)vkl_resolve("vkCmdSetCheckpointNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCheckpointNV");
    }
  pfn(commandBuffer, pCheckpointMarker);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoarseSampleOrderNV(VkCommandBuffer commandBuffer, VkCoarseSampleOrderTypeNV sampleOrderType, uint32_t customSampleOrderCount, const VkCoarseSampleOrderCustomNV* pCustomSampleOrders)
{
  static PFN_vkCmdSetCoarseSampleOrderNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoarseSampleOrderNV)vkl_resolve("vkCmdSetCoarseSampleOrderNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoarseSampleOrderNV");
    }
  pfn(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetColorBlendAdvancedEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorBlendAdvancedEXT* pColorBlendAdvanced)
{
  static PFN_vkCmdSetColorBlendAdvancedEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetColorBlendAdvancedEXT)vkl_resolve("vkCmdSetColorBlendAdvancedEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetColorBlendAdvancedEXT");
    }
  pfn(commandBuffer, firstAttachment, attachmentCount, pColorBlendAdvanced);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables)
{
  static PFN_vkCmdSetColorBlendEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetColorBlendEnableEXT)vkl_resolve("vkCmdSetColorBlendEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetColorBlendEnableEXT");
    }
  pfn(commandBuffer, firstAttachment, attachmentCount, pColorBlendEnables);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorBlendEquationEXT* pColorBlendEquations)
{
  static PFN_vkCmdSetColorBlendEquationEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetColorBlendEquationEXT)vkl_resolve("vkCmdSetColorBlendEquationEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetColorBlendEquationEXT");
    }
  pfn(commandBuffer, firstAttachment, attachmentCount, pColorBlendEquations);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetColorWriteEnableEXT(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkBool32* pColorWriteEnables)
{
  static PFN_vkCmdSetColorWriteEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetColorWriteEnableEXT)vkl_resolve("vkCmdSetColorWriteEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetColorWriteEnableEXT");
    }
  pfn(commandBuffer, attachmentCount, pColorWriteEnables);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks)
{
  static PFN_vkCmdSetColorWriteMaskEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetColorWriteMaskEXT)vkl_resolve("vkCmdSetColorWriteMaskEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetColorWriteMaskEXT");
    }
  pfn(commandBuffer, firstAttachment, attachmentCount, pColorWriteMasks);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetComputeOccupancyPriorityNV(VkCommandBuffer commandBuffer, const VkComputeOccupancyPriorityParametersNV* pParameters)
{
  static PFN_vkCmdSetComputeOccupancyPriorityNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetComputeOccupancyPriorityNV)vkl_resolve("vkCmdSetComputeOccupancyPriorityNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetComputeOccupancyPriorityNV");
    }
  pfn(commandBuffer, pParameters);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetConservativeRasterizationModeEXT(VkCommandBuffer commandBuffer, VkConservativeRasterizationModeEXT conservativeRasterizationMode)
{
  static PFN_vkCmdSetConservativeRasterizationModeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetConservativeRasterizationModeEXT)vkl_resolve("vkCmdSetConservativeRasterizationModeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetConservativeRasterizationModeEXT");
    }
  pfn(commandBuffer, conservativeRasterizationMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoverageModulationModeNV(VkCommandBuffer commandBuffer, VkCoverageModulationModeNV coverageModulationMode)
{
  static PFN_vkCmdSetCoverageModulationModeNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoverageModulationModeNV)vkl_resolve("vkCmdSetCoverageModulationModeNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoverageModulationModeNV");
    }
  pfn(commandBuffer, coverageModulationMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoverageModulationTableEnableNV(VkCommandBuffer commandBuffer, VkBool32 coverageModulationTableEnable)
{
  static PFN_vkCmdSetCoverageModulationTableEnableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoverageModulationTableEnableNV)vkl_resolve("vkCmdSetCoverageModulationTableEnableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoverageModulationTableEnableNV");
    }
  pfn(commandBuffer, coverageModulationTableEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoverageModulationTableNV(VkCommandBuffer commandBuffer, uint32_t coverageModulationTableCount, const float* pCoverageModulationTable)
{
  static PFN_vkCmdSetCoverageModulationTableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoverageModulationTableNV)vkl_resolve("vkCmdSetCoverageModulationTableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoverageModulationTableNV");
    }
  pfn(commandBuffer, coverageModulationTableCount, pCoverageModulationTable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoverageReductionModeNV(VkCommandBuffer commandBuffer, VkCoverageReductionModeNV coverageReductionMode)
{
  static PFN_vkCmdSetCoverageReductionModeNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoverageReductionModeNV)vkl_resolve("vkCmdSetCoverageReductionModeNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoverageReductionModeNV");
    }
  pfn(commandBuffer, coverageReductionMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoverageToColorEnableNV(VkCommandBuffer commandBuffer, VkBool32 coverageToColorEnable)
{
  static PFN_vkCmdSetCoverageToColorEnableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoverageToColorEnableNV)vkl_resolve("vkCmdSetCoverageToColorEnableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoverageToColorEnableNV");
    }
  pfn(commandBuffer, coverageToColorEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCoverageToColorLocationNV(VkCommandBuffer commandBuffer, uint32_t coverageToColorLocation)
{
  static PFN_vkCmdSetCoverageToColorLocationNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCoverageToColorLocationNV)vkl_resolve("vkCmdSetCoverageToColorLocationNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCoverageToColorLocationNV");
    }
  pfn(commandBuffer, coverageToColorLocation);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode)
{
  static PFN_vkCmdSetCullMode pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCullMode)vkl_resolve("vkCmdSetCullMode");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCullMode");
    }
  pfn(commandBuffer, cullMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetCullModeEXT(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode)
{
  static PFN_vkCmdSetCullModeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetCullModeEXT)vkl_resolve("vkCmdSetCullModeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetCullModeEXT");
    }
  pfn(commandBuffer, cullMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
  static PFN_vkCmdSetDepthBias pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBias)vkl_resolve("vkCmdSetDepthBias");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBias");
    }
  pfn(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBias2EXT(VkCommandBuffer commandBuffer, const VkDepthBiasInfoEXT* pDepthBiasInfo)
{
  static PFN_vkCmdSetDepthBias2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBias2EXT)vkl_resolve("vkCmdSetDepthBias2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBias2EXT");
    }
  pfn(commandBuffer, pDepthBiasInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable)
{
  static PFN_vkCmdSetDepthBiasEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBiasEnable)vkl_resolve("vkCmdSetDepthBiasEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBiasEnable");
    }
  pfn(commandBuffer, depthBiasEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBiasEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable)
{
  static PFN_vkCmdSetDepthBiasEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBiasEnableEXT)vkl_resolve("vkCmdSetDepthBiasEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBiasEnableEXT");
    }
  pfn(commandBuffer, depthBiasEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
  static PFN_vkCmdSetDepthBounds pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBounds)vkl_resolve("vkCmdSetDepthBounds");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBounds");
    }
  pfn(commandBuffer, minDepthBounds, maxDepthBounds);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable)
{
  static PFN_vkCmdSetDepthBoundsTestEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBoundsTestEnable)vkl_resolve("vkCmdSetDepthBoundsTestEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBoundsTestEnable");
    }
  pfn(commandBuffer, depthBoundsTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBoundsTestEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable)
{
  static PFN_vkCmdSetDepthBoundsTestEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthBoundsTestEnableEXT)vkl_resolve("vkCmdSetDepthBoundsTestEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthBoundsTestEnableEXT");
    }
  pfn(commandBuffer, depthBoundsTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthClampEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClampEnable)
{
  static PFN_vkCmdSetDepthClampEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthClampEnableEXT)vkl_resolve("vkCmdSetDepthClampEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthClampEnableEXT");
    }
  pfn(commandBuffer, depthClampEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthClampRangeEXT(VkCommandBuffer commandBuffer, VkDepthClampModeEXT depthClampMode, const VkDepthClampRangeEXT* pDepthClampRange)
{
  static PFN_vkCmdSetDepthClampRangeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthClampRangeEXT)vkl_resolve("vkCmdSetDepthClampRangeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthClampRangeEXT");
    }
  pfn(commandBuffer, depthClampMode, pDepthClampRange);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthClipEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClipEnable)
{
  static PFN_vkCmdSetDepthClipEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthClipEnableEXT)vkl_resolve("vkCmdSetDepthClipEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthClipEnableEXT");
    }
  pfn(commandBuffer, depthClipEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer commandBuffer, VkBool32 negativeOneToOne)
{
  static PFN_vkCmdSetDepthClipNegativeOneToOneEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthClipNegativeOneToOneEXT)vkl_resolve("vkCmdSetDepthClipNegativeOneToOneEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthClipNegativeOneToOneEXT");
    }
  pfn(commandBuffer, negativeOneToOne);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp)
{
  static PFN_vkCmdSetDepthCompareOp pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthCompareOp)vkl_resolve("vkCmdSetDepthCompareOp");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthCompareOp");
    }
  pfn(commandBuffer, depthCompareOp);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthCompareOpEXT(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp)
{
  static PFN_vkCmdSetDepthCompareOpEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthCompareOpEXT)vkl_resolve("vkCmdSetDepthCompareOpEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthCompareOpEXT");
    }
  pfn(commandBuffer, depthCompareOp);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable)
{
  static PFN_vkCmdSetDepthTestEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthTestEnable)vkl_resolve("vkCmdSetDepthTestEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthTestEnable");
    }
  pfn(commandBuffer, depthTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthTestEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable)
{
  static PFN_vkCmdSetDepthTestEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthTestEnableEXT)vkl_resolve("vkCmdSetDepthTestEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthTestEnableEXT");
    }
  pfn(commandBuffer, depthTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable)
{
  static PFN_vkCmdSetDepthWriteEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthWriteEnable)vkl_resolve("vkCmdSetDepthWriteEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthWriteEnable");
    }
  pfn(commandBuffer, depthWriteEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthWriteEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable)
{
  static PFN_vkCmdSetDepthWriteEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDepthWriteEnableEXT)vkl_resolve("vkCmdSetDepthWriteEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDepthWriteEnableEXT");
    }
  pfn(commandBuffer, depthWriteEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDescriptorBufferOffsets2EXT(VkCommandBuffer commandBuffer, const VkSetDescriptorBufferOffsetsInfoEXT* pSetDescriptorBufferOffsetsInfo)
{
  static PFN_vkCmdSetDescriptorBufferOffsets2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDescriptorBufferOffsets2EXT)vkl_resolve("vkCmdSetDescriptorBufferOffsets2EXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDescriptorBufferOffsets2EXT");
    }
  pfn(commandBuffer, pSetDescriptorBufferOffsetsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDescriptorBufferOffsetsEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t setCount, const uint32_t* pBufferIndices, const VkDeviceSize* pOffsets)
{
  static PFN_vkCmdSetDescriptorBufferOffsetsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDescriptorBufferOffsetsEXT)vkl_resolve("vkCmdSetDescriptorBufferOffsetsEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDescriptorBufferOffsetsEXT");
    }
  pfn(commandBuffer, pipelineBindPoint, layout, firstSet, setCount, pBufferIndices, pOffsets);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask)
{
  static PFN_vkCmdSetDeviceMask pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDeviceMask)vkl_resolve("vkCmdSetDeviceMask");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDeviceMask");
    }
  pfn(commandBuffer, deviceMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDeviceMaskKHR(VkCommandBuffer commandBuffer, uint32_t deviceMask)
{
  static PFN_vkCmdSetDeviceMaskKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDeviceMaskKHR)vkl_resolve("vkCmdSetDeviceMaskKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDeviceMaskKHR");
    }
  pfn(commandBuffer, deviceMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDiscardRectangleEXT(VkCommandBuffer commandBuffer, uint32_t firstDiscardRectangle, uint32_t discardRectangleCount, const VkRect2D* pDiscardRectangles)
{
  static PFN_vkCmdSetDiscardRectangleEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDiscardRectangleEXT)vkl_resolve("vkCmdSetDiscardRectangleEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDiscardRectangleEXT");
    }
  pfn(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDiscardRectangleEnableEXT(VkCommandBuffer commandBuffer, VkBool32 discardRectangleEnable)
{
  static PFN_vkCmdSetDiscardRectangleEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDiscardRectangleEnableEXT)vkl_resolve("vkCmdSetDiscardRectangleEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDiscardRectangleEnableEXT");
    }
  pfn(commandBuffer, discardRectangleEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDiscardRectangleModeEXT(VkCommandBuffer commandBuffer, VkDiscardRectangleModeEXT discardRectangleMode)
{
  static PFN_vkCmdSetDiscardRectangleModeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDiscardRectangleModeEXT)vkl_resolve("vkCmdSetDiscardRectangleModeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDiscardRectangleModeEXT");
    }
  pfn(commandBuffer, discardRectangleMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetDispatchParametersARM(VkCommandBuffer commandBuffer, const VkDispatchParametersARM* pDispatchParameters)
{
  static PFN_vkCmdSetDispatchParametersARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetDispatchParametersARM)vkl_resolve("vkCmdSetDispatchParametersARM");
    if(pfn == NULL)
      vkl_missing("vkCmdSetDispatchParametersARM");
    }
  pfn(commandBuffer, pDispatchParameters);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
  static PFN_vkCmdSetEvent pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetEvent)vkl_resolve("vkCmdSetEvent");
    if(pfn == NULL)
      vkl_missing("vkCmdSetEvent");
    }
  pfn(commandBuffer, event, stageMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetEvent2(VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfo* pDependencyInfo)
{
  static PFN_vkCmdSetEvent2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetEvent2)vkl_resolve("vkCmdSetEvent2");
    if(pfn == NULL)
      vkl_missing("vkCmdSetEvent2");
    }
  pfn(commandBuffer, event, pDependencyInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetEvent2KHR(VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfo* pDependencyInfo)
{
  static PFN_vkCmdSetEvent2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetEvent2KHR)vkl_resolve("vkCmdSetEvent2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetEvent2KHR");
    }
  pfn(commandBuffer, event, pDependencyInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetExclusiveScissorEnableNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor, uint32_t exclusiveScissorCount, const VkBool32* pExclusiveScissorEnables)
{
  static PFN_vkCmdSetExclusiveScissorEnableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetExclusiveScissorEnableNV)vkl_resolve("vkCmdSetExclusiveScissorEnableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetExclusiveScissorEnableNV");
    }
  pfn(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissorEnables);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetExclusiveScissorNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor, uint32_t exclusiveScissorCount, const VkRect2D* pExclusiveScissors)
{
  static PFN_vkCmdSetExclusiveScissorNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetExclusiveScissorNV)vkl_resolve("vkCmdSetExclusiveScissorNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetExclusiveScissorNV");
    }
  pfn(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetExtraPrimitiveOverestimationSizeEXT(VkCommandBuffer commandBuffer, float extraPrimitiveOverestimationSize)
{
  static PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT)vkl_resolve("vkCmdSetExtraPrimitiveOverestimationSizeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetExtraPrimitiveOverestimationSizeEXT");
    }
  pfn(commandBuffer, extraPrimitiveOverestimationSize);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetFragmentShadingRateEnumNV(VkCommandBuffer commandBuffer, VkFragmentShadingRateNV shadingRate, const VkFragmentShadingRateCombinerOpKHR combinerOps[2])
{
  static PFN_vkCmdSetFragmentShadingRateEnumNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetFragmentShadingRateEnumNV)vkl_resolve("vkCmdSetFragmentShadingRateEnumNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetFragmentShadingRateEnumNV");
    }
  pfn(commandBuffer, shadingRate, combinerOps);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetFragmentShadingRateKHR(VkCommandBuffer commandBuffer, const VkExtent2D* pFragmentSize, const VkFragmentShadingRateCombinerOpKHR combinerOps[2])
{
  static PFN_vkCmdSetFragmentShadingRateKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetFragmentShadingRateKHR)vkl_resolve("vkCmdSetFragmentShadingRateKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetFragmentShadingRateKHR");
    }
  pfn(commandBuffer, pFragmentSize, combinerOps);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace)
{
  static PFN_vkCmdSetFrontFace pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetFrontFace)vkl_resolve("vkCmdSetFrontFace");
    if(pfn == NULL)
      vkl_missing("vkCmdSetFrontFace");
    }
  pfn(commandBuffer, frontFace);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetFrontFaceEXT(VkCommandBuffer commandBuffer, VkFrontFace frontFace)
{
  static PFN_vkCmdSetFrontFaceEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetFrontFaceEXT)vkl_resolve("vkCmdSetFrontFaceEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetFrontFaceEXT");
    }
  pfn(commandBuffer, frontFace);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineRasterizationModeEXT(VkCommandBuffer commandBuffer, VkLineRasterizationModeEXT lineRasterizationMode)
{
  static PFN_vkCmdSetLineRasterizationModeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLineRasterizationModeEXT)vkl_resolve("vkCmdSetLineRasterizationModeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLineRasterizationModeEXT");
    }
  pfn(commandBuffer, lineRasterizationMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineStipple(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor, uint16_t lineStipplePattern)
{
  static PFN_vkCmdSetLineStipple pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLineStipple)vkl_resolve("vkCmdSetLineStipple");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLineStipple");
    }
  pfn(commandBuffer, lineStippleFactor, lineStipplePattern);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineStippleEXT(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor, uint16_t lineStipplePattern)
{
  static PFN_vkCmdSetLineStippleEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLineStippleEXT)vkl_resolve("vkCmdSetLineStippleEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLineStippleEXT");
    }
  pfn(commandBuffer, lineStippleFactor, lineStipplePattern);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineStippleEnableEXT(VkCommandBuffer commandBuffer, VkBool32 stippledLineEnable)
{
  static PFN_vkCmdSetLineStippleEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLineStippleEnableEXT)vkl_resolve("vkCmdSetLineStippleEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLineStippleEnableEXT");
    }
  pfn(commandBuffer, stippledLineEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineStippleKHR(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor, uint16_t lineStipplePattern)
{
  static PFN_vkCmdSetLineStippleKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLineStippleKHR)vkl_resolve("vkCmdSetLineStippleKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLineStippleKHR");
    }
  pfn(commandBuffer, lineStippleFactor, lineStipplePattern);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
  static PFN_vkCmdSetLineWidth pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLineWidth)vkl_resolve("vkCmdSetLineWidth");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLineWidth");
    }
  pfn(commandBuffer, lineWidth);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLogicOpEXT(VkCommandBuffer commandBuffer, VkLogicOp logicOp)
{
  static PFN_vkCmdSetLogicOpEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLogicOpEXT)vkl_resolve("vkCmdSetLogicOpEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLogicOpEXT");
    }
  pfn(commandBuffer, logicOp);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetLogicOpEnableEXT(VkCommandBuffer commandBuffer, VkBool32 logicOpEnable)
{
  static PFN_vkCmdSetLogicOpEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetLogicOpEnableEXT)vkl_resolve("vkCmdSetLogicOpEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetLogicOpEnableEXT");
    }
  pfn(commandBuffer, logicOpEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPatchControlPointsEXT(VkCommandBuffer commandBuffer, uint32_t patchControlPoints)
{
  static PFN_vkCmdSetPatchControlPointsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPatchControlPointsEXT)vkl_resolve("vkCmdSetPatchControlPointsEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPatchControlPointsEXT");
    }
  pfn(commandBuffer, patchControlPoints);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCmdSetPerformanceMarkerINTEL(VkCommandBuffer commandBuffer, const VkPerformanceMarkerInfoINTEL* pMarkerInfo)
{
  static PFN_vkCmdSetPerformanceMarkerINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPerformanceMarkerINTEL)vkl_resolve("vkCmdSetPerformanceMarkerINTEL");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPerformanceMarkerINTEL");
    }
  return pfn(commandBuffer, pMarkerInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCmdSetPerformanceOverrideINTEL(VkCommandBuffer commandBuffer, const VkPerformanceOverrideInfoINTEL* pOverrideInfo)
{
  static PFN_vkCmdSetPerformanceOverrideINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPerformanceOverrideINTEL)vkl_resolve("vkCmdSetPerformanceOverrideINTEL");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPerformanceOverrideINTEL");
    }
  return pfn(commandBuffer, pOverrideInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCmdSetPerformanceStreamMarkerINTEL(VkCommandBuffer commandBuffer, const VkPerformanceStreamMarkerInfoINTEL* pMarkerInfo)
{
  static PFN_vkCmdSetPerformanceStreamMarkerINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPerformanceStreamMarkerINTEL)vkl_resolve("vkCmdSetPerformanceStreamMarkerINTEL");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPerformanceStreamMarkerINTEL");
    }
  return pfn(commandBuffer, pMarkerInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode)
{
  static PFN_vkCmdSetPolygonModeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPolygonModeEXT)vkl_resolve("vkCmdSetPolygonModeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPolygonModeEXT");
    }
  pfn(commandBuffer, polygonMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable)
{
  static PFN_vkCmdSetPrimitiveRestartEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPrimitiveRestartEnable)vkl_resolve("vkCmdSetPrimitiveRestartEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPrimitiveRestartEnable");
    }
  pfn(commandBuffer, primitiveRestartEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPrimitiveRestartEnableEXT(VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable)
{
  static PFN_vkCmdSetPrimitiveRestartEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPrimitiveRestartEnableEXT)vkl_resolve("vkCmdSetPrimitiveRestartEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPrimitiveRestartEnableEXT");
    }
  pfn(commandBuffer, primitiveRestartEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPrimitiveRestartIndexEXT(VkCommandBuffer commandBuffer, uint32_t primitiveRestartIndex)
{
  static PFN_vkCmdSetPrimitiveRestartIndexEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPrimitiveRestartIndexEXT)vkl_resolve("vkCmdSetPrimitiveRestartIndexEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPrimitiveRestartIndexEXT");
    }
  pfn(commandBuffer, primitiveRestartIndex);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPrimitiveTopology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology)
{
  static PFN_vkCmdSetPrimitiveTopology pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPrimitiveTopology)vkl_resolve("vkCmdSetPrimitiveTopology");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPrimitiveTopology");
    }
  pfn(commandBuffer, primitiveTopology);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetPrimitiveTopologyEXT(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology)
{
  static PFN_vkCmdSetPrimitiveTopologyEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetPrimitiveTopologyEXT)vkl_resolve("vkCmdSetPrimitiveTopologyEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetPrimitiveTopologyEXT");
    }
  pfn(commandBuffer, primitiveTopology);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetProvokingVertexModeEXT(VkCommandBuffer commandBuffer, VkProvokingVertexModeEXT provokingVertexMode)
{
  static PFN_vkCmdSetProvokingVertexModeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetProvokingVertexModeEXT)vkl_resolve("vkCmdSetProvokingVertexModeEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetProvokingVertexModeEXT");
    }
  pfn(commandBuffer, provokingVertexMode);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits rasterizationSamples)
{
  static PFN_vkCmdSetRasterizationSamplesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRasterizationSamplesEXT)vkl_resolve("vkCmdSetRasterizationSamplesEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRasterizationSamplesEXT");
    }
  pfn(commandBuffer, rasterizationSamples);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRasterizationStreamEXT(VkCommandBuffer commandBuffer, uint32_t rasterizationStream)
{
  static PFN_vkCmdSetRasterizationStreamEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRasterizationStreamEXT)vkl_resolve("vkCmdSetRasterizationStreamEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRasterizationStreamEXT");
    }
  pfn(commandBuffer, rasterizationStream);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable)
{
  static PFN_vkCmdSetRasterizerDiscardEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRasterizerDiscardEnable)vkl_resolve("vkCmdSetRasterizerDiscardEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRasterizerDiscardEnable");
    }
  pfn(commandBuffer, rasterizerDiscardEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRasterizerDiscardEnableEXT(VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable)
{
  static PFN_vkCmdSetRasterizerDiscardEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRasterizerDiscardEnableEXT)vkl_resolve("vkCmdSetRasterizerDiscardEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRasterizerDiscardEnableEXT");
    }
  pfn(commandBuffer, rasterizerDiscardEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRayTracingPipelineStackSizeKHR(VkCommandBuffer commandBuffer, uint32_t pipelineStackSize)
{
  static PFN_vkCmdSetRayTracingPipelineStackSizeKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)vkl_resolve("vkCmdSetRayTracingPipelineStackSizeKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRayTracingPipelineStackSizeKHR");
    }
  pfn(commandBuffer, pipelineStackSize);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingAttachmentLocations(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo)
{
  static PFN_vkCmdSetRenderingAttachmentLocations pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRenderingAttachmentLocations)vkl_resolve("vkCmdSetRenderingAttachmentLocations");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRenderingAttachmentLocations");
    }
  pfn(commandBuffer, pLocationInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingAttachmentLocationsKHR(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo)
{
  static PFN_vkCmdSetRenderingAttachmentLocationsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRenderingAttachmentLocationsKHR)vkl_resolve("vkCmdSetRenderingAttachmentLocationsKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRenderingAttachmentLocationsKHR");
    }
  pfn(commandBuffer, pLocationInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingInputAttachmentIndices(VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo)
{
  static PFN_vkCmdSetRenderingInputAttachmentIndices pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRenderingInputAttachmentIndices)vkl_resolve("vkCmdSetRenderingInputAttachmentIndices");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRenderingInputAttachmentIndices");
    }
  pfn(commandBuffer, pInputAttachmentIndexInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingInputAttachmentIndicesKHR(VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo)
{
  static PFN_vkCmdSetRenderingInputAttachmentIndicesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRenderingInputAttachmentIndicesKHR)vkl_resolve("vkCmdSetRenderingInputAttachmentIndicesKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRenderingInputAttachmentIndicesKHR");
    }
  pfn(commandBuffer, pInputAttachmentIndexInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetRepresentativeFragmentTestEnableNV(VkCommandBuffer commandBuffer, VkBool32 representativeFragmentTestEnable)
{
  static PFN_vkCmdSetRepresentativeFragmentTestEnableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetRepresentativeFragmentTestEnableNV)vkl_resolve("vkCmdSetRepresentativeFragmentTestEnableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetRepresentativeFragmentTestEnableNV");
    }
  pfn(commandBuffer, representativeFragmentTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT* pSampleLocationsInfo)
{
  static PFN_vkCmdSetSampleLocationsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetSampleLocationsEXT)vkl_resolve("vkCmdSetSampleLocationsEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetSampleLocationsEXT");
    }
  pfn(commandBuffer, pSampleLocationsInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetSampleLocationsEnableEXT(VkCommandBuffer commandBuffer, VkBool32 sampleLocationsEnable)
{
  static PFN_vkCmdSetSampleLocationsEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetSampleLocationsEnableEXT)vkl_resolve("vkCmdSetSampleLocationsEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetSampleLocationsEnableEXT");
    }
  pfn(commandBuffer, sampleLocationsEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetSampleMaskEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits samples, const VkSampleMask* pSampleMask)
{
  static PFN_vkCmdSetSampleMaskEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetSampleMaskEXT)vkl_resolve("vkCmdSetSampleMaskEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetSampleMaskEXT");
    }
  pfn(commandBuffer, samples, pSampleMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
  static PFN_vkCmdSetScissor pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetScissor)vkl_resolve("vkCmdSetScissor");
    if(pfn == NULL)
      vkl_missing("vkCmdSetScissor");
    }
  pfn(commandBuffer, firstScissor, scissorCount, pScissors);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors)
{
  static PFN_vkCmdSetScissorWithCount pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetScissorWithCount)vkl_resolve("vkCmdSetScissorWithCount");
    if(pfn == NULL)
      vkl_missing("vkCmdSetScissorWithCount");
    }
  pfn(commandBuffer, scissorCount, pScissors);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetScissorWithCountEXT(VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors)
{
  static PFN_vkCmdSetScissorWithCountEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetScissorWithCountEXT)vkl_resolve("vkCmdSetScissorWithCountEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetScissorWithCountEXT");
    }
  pfn(commandBuffer, scissorCount, pScissors);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetShadingRateImageEnableNV(VkCommandBuffer commandBuffer, VkBool32 shadingRateImageEnable)
{
  static PFN_vkCmdSetShadingRateImageEnableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetShadingRateImageEnableNV)vkl_resolve("vkCmdSetShadingRateImageEnableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetShadingRateImageEnableNV");
    }
  pfn(commandBuffer, shadingRateImageEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
  static PFN_vkCmdSetStencilCompareMask pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilCompareMask)vkl_resolve("vkCmdSetStencilCompareMask");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilCompareMask");
    }
  pfn(commandBuffer, faceMask, compareMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp)
{
  static PFN_vkCmdSetStencilOp pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilOp)vkl_resolve("vkCmdSetStencilOp");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilOp");
    }
  pfn(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilOpEXT(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp)
{
  static PFN_vkCmdSetStencilOpEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilOpEXT)vkl_resolve("vkCmdSetStencilOpEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilOpEXT");
    }
  pfn(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
  static PFN_vkCmdSetStencilReference pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilReference)vkl_resolve("vkCmdSetStencilReference");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilReference");
    }
  pfn(commandBuffer, faceMask, reference);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable)
{
  static PFN_vkCmdSetStencilTestEnable pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilTestEnable)vkl_resolve("vkCmdSetStencilTestEnable");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilTestEnable");
    }
  pfn(commandBuffer, stencilTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilTestEnableEXT(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable)
{
  static PFN_vkCmdSetStencilTestEnableEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilTestEnableEXT)vkl_resolve("vkCmdSetStencilTestEnableEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilTestEnableEXT");
    }
  pfn(commandBuffer, stencilTestEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
  static PFN_vkCmdSetStencilWriteMask pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetStencilWriteMask)vkl_resolve("vkCmdSetStencilWriteMask");
    if(pfn == NULL)
      vkl_missing("vkCmdSetStencilWriteMask");
    }
  pfn(commandBuffer, faceMask, writeMask);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetTessellationDomainOriginEXT(VkCommandBuffer commandBuffer, VkTessellationDomainOrigin domainOrigin)
{
  static PFN_vkCmdSetTessellationDomainOriginEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetTessellationDomainOriginEXT)vkl_resolve("vkCmdSetTessellationDomainOriginEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetTessellationDomainOriginEXT");
    }
  pfn(commandBuffer, domainOrigin);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetVertexInputEXT(VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions)
{
  static PFN_vkCmdSetVertexInputEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetVertexInputEXT)vkl_resolve("vkCmdSetVertexInputEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetVertexInputEXT");
    }
  pfn(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
  static PFN_vkCmdSetViewport pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewport)vkl_resolve("vkCmdSetViewport");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewport");
    }
  pfn(commandBuffer, firstViewport, viewportCount, pViewports);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportShadingRatePaletteNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkShadingRatePaletteNV* pShadingRatePalettes)
{
  static PFN_vkCmdSetViewportShadingRatePaletteNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewportShadingRatePaletteNV)vkl_resolve("vkCmdSetViewportShadingRatePaletteNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewportShadingRatePaletteNV");
    }
  pfn(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportSwizzleNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportSwizzleNV* pViewportSwizzles)
{
  static PFN_vkCmdSetViewportSwizzleNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewportSwizzleNV)vkl_resolve("vkCmdSetViewportSwizzleNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewportSwizzleNV");
    }
  pfn(commandBuffer, firstViewport, viewportCount, pViewportSwizzles);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportWScalingEnableNV(VkCommandBuffer commandBuffer, VkBool32 viewportWScalingEnable)
{
  static PFN_vkCmdSetViewportWScalingEnableNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewportWScalingEnableNV)vkl_resolve("vkCmdSetViewportWScalingEnableNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewportWScalingEnableNV");
    }
  pfn(commandBuffer, viewportWScalingEnable);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportWScalingNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportWScalingNV* pViewportWScalings)
{
  static PFN_vkCmdSetViewportWScalingNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewportWScalingNV)vkl_resolve("vkCmdSetViewportWScalingNV");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewportWScalingNV");
    }
  pfn(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports)
{
  static PFN_vkCmdSetViewportWithCount pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewportWithCount)vkl_resolve("vkCmdSetViewportWithCount");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewportWithCount");
    }
  pfn(commandBuffer, viewportCount, pViewports);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportWithCountEXT(VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports)
{
  static PFN_vkCmdSetViewportWithCountEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSetViewportWithCountEXT)vkl_resolve("vkCmdSetViewportWithCountEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdSetViewportWithCountEXT");
    }
  pfn(commandBuffer, viewportCount, pViewports);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdSubpassShadingHUAWEI(VkCommandBuffer commandBuffer)
{
  static PFN_vkCmdSubpassShadingHUAWEI pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdSubpassShadingHUAWEI)vkl_resolve("vkCmdSubpassShadingHUAWEI");
    if(pfn == NULL)
      vkl_missing("vkCmdSubpassShadingHUAWEI");
    }
  pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdTraceRaysIndirect2KHR(VkCommandBuffer commandBuffer, VkDeviceAddress indirectDeviceAddress)
{
  static PFN_vkCmdTraceRaysIndirect2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdTraceRaysIndirect2KHR)vkl_resolve("vkCmdTraceRaysIndirect2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdTraceRaysIndirect2KHR");
    }
  pfn(commandBuffer, indirectDeviceAddress);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdTraceRaysIndirectKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, VkDeviceAddress indirectDeviceAddress)
{
  static PFN_vkCmdTraceRaysIndirectKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdTraceRaysIndirectKHR)vkl_resolve("vkCmdTraceRaysIndirectKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdTraceRaysIndirectKHR");
    }
  pfn(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdTraceRaysKHR(VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, uint32_t width, uint32_t height, uint32_t depth)
{
  static PFN_vkCmdTraceRaysKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdTraceRaysKHR)vkl_resolve("vkCmdTraceRaysKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdTraceRaysKHR");
    }
  pfn(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdTraceRaysNV(VkCommandBuffer commandBuffer, VkBuffer raygenShaderBindingTableBuffer, VkDeviceSize raygenShaderBindingOffset, VkBuffer missShaderBindingTableBuffer, VkDeviceSize missShaderBindingOffset, VkDeviceSize missShaderBindingStride, VkBuffer hitShaderBindingTableBuffer, VkDeviceSize hitShaderBindingOffset, VkDeviceSize hitShaderBindingStride, VkBuffer callableShaderBindingTableBuffer, VkDeviceSize callableShaderBindingOffset, VkDeviceSize callableShaderBindingStride, uint32_t width, uint32_t height, uint32_t depth)
{
  static PFN_vkCmdTraceRaysNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdTraceRaysNV)vkl_resolve("vkCmdTraceRaysNV");
    if(pfn == NULL)
      vkl_missing("vkCmdTraceRaysNV");
    }
  pfn(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
{
  static PFN_vkCmdUpdateBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdUpdateBuffer)vkl_resolve("vkCmdUpdateBuffer");
    if(pfn == NULL)
      vkl_missing("vkCmdUpdateBuffer");
    }
  pfn(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdUpdateMemoryKHR(VkCommandBuffer commandBuffer, const VkDeviceAddressRangeKHR* pDstRange, VkAddressCommandFlagsKHR dstFlags, VkDeviceSize dataSize, const void* pData)
{
  static PFN_vkCmdUpdateMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdUpdateMemoryKHR)vkl_resolve("vkCmdUpdateMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdUpdateMemoryKHR");
    }
  pfn(commandBuffer, pDstRange, dstFlags, dataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdUpdatePipelineIndirectBufferNV(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
  static PFN_vkCmdUpdatePipelineIndirectBufferNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdUpdatePipelineIndirectBufferNV)vkl_resolve("vkCmdUpdatePipelineIndirectBufferNV");
    if(pfn == NULL)
      vkl_missing("vkCmdUpdatePipelineIndirectBufferNV");
    }
  pfn(commandBuffer, pipelineBindPoint, pipeline);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
  static PFN_vkCmdWaitEvents pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWaitEvents)vkl_resolve("vkCmdWaitEvents");
    if(pfn == NULL)
      vkl_missing("vkCmdWaitEvents");
    }
  pfn(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents2(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos)
{
  static PFN_vkCmdWaitEvents2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWaitEvents2)vkl_resolve("vkCmdWaitEvents2");
    if(pfn == NULL)
      vkl_missing("vkCmdWaitEvents2");
    }
  pfn(commandBuffer, eventCount, pEvents, pDependencyInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents2KHR(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos)
{
  static PFN_vkCmdWaitEvents2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWaitEvents2KHR)vkl_resolve("vkCmdWaitEvents2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdWaitEvents2KHR");
    }
  pfn(commandBuffer, eventCount, pEvents, pDependencyInfos);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteAccelerationStructuresPropertiesKHR(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
  static PFN_vkCmdWriteAccelerationStructuresPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)vkl_resolve("vkCmdWriteAccelerationStructuresPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteAccelerationStructuresPropertiesKHR");
    }
  pfn(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteAccelerationStructuresPropertiesNV(VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureNV* pAccelerationStructures, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
  static PFN_vkCmdWriteAccelerationStructuresPropertiesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteAccelerationStructuresPropertiesNV)vkl_resolve("vkCmdWriteAccelerationStructuresPropertiesNV");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteAccelerationStructuresPropertiesNV");
    }
  pfn(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteBufferMarker2AMD(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker)
{
  static PFN_vkCmdWriteBufferMarker2AMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteBufferMarker2AMD)vkl_resolve("vkCmdWriteBufferMarker2AMD");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteBufferMarker2AMD");
    }
  pfn(commandBuffer, stage, dstBuffer, dstOffset, marker);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteBufferMarkerAMD(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker)
{
  static PFN_vkCmdWriteBufferMarkerAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteBufferMarkerAMD)vkl_resolve("vkCmdWriteBufferMarkerAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteBufferMarkerAMD");
    }
  pfn(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteMarkerToMemoryAMD(VkCommandBuffer commandBuffer, const VkMemoryMarkerInfoAMD* pInfo)
{
  static PFN_vkCmdWriteMarkerToMemoryAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteMarkerToMemoryAMD)vkl_resolve("vkCmdWriteMarkerToMemoryAMD");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteMarkerToMemoryAMD");
    }
  pfn(commandBuffer, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteMicromapsPropertiesEXT(VkCommandBuffer commandBuffer, uint32_t micromapCount, const VkMicromapEXT* pMicromaps, VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery)
{
  static PFN_vkCmdWriteMicromapsPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteMicromapsPropertiesEXT)vkl_resolve("vkCmdWriteMicromapsPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteMicromapsPropertiesEXT");
    }
  pfn(commandBuffer, micromapCount, pMicromaps, queryType, queryPool, firstQuery);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
  static PFN_vkCmdWriteTimestamp pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteTimestamp)vkl_resolve("vkCmdWriteTimestamp");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteTimestamp");
    }
  pfn(commandBuffer, pipelineStage, queryPool, query);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteTimestamp2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query)
{
  static PFN_vkCmdWriteTimestamp2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteTimestamp2)vkl_resolve("vkCmdWriteTimestamp2");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteTimestamp2");
    }
  pfn(commandBuffer, stage, queryPool, query);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkCmdWriteTimestamp2KHR(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query)
{
  static PFN_vkCmdWriteTimestamp2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCmdWriteTimestamp2KHR)vkl_resolve("vkCmdWriteTimestamp2KHR");
    if(pfn == NULL)
      vkl_missing("vkCmdWriteTimestamp2KHR");
    }
  pfn(commandBuffer, stage, queryPool, query);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCompileDeferredNV(VkDevice device, VkPipeline pipeline, uint32_t shader)
{
  static PFN_vkCompileDeferredNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCompileDeferredNV)vkl_resolve("vkCompileDeferredNV");
    if(pfn == NULL)
      vkl_missing("vkCompileDeferredNV");
    }
  return pfn(device, pipeline, shader);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkConvertCooperativeVectorMatrixNV(VkDevice device, const VkConvertCooperativeVectorMatrixInfoNV* pInfo)
{
  static PFN_vkConvertCooperativeVectorMatrixNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkConvertCooperativeVectorMatrixNV)vkl_resolve("vkConvertCooperativeVectorMatrixNV");
    if(pfn == NULL)
      vkl_missing("vkConvertCooperativeVectorMatrixNV");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyAccelerationStructureKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureInfoKHR* pInfo)
{
  static PFN_vkCopyAccelerationStructureKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyAccelerationStructureKHR)vkl_resolve("vkCopyAccelerationStructureKHR");
    if(pfn == NULL)
      vkl_missing("vkCopyAccelerationStructureKHR");
    }
  return pfn(device, deferredOperation, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyAccelerationStructureToMemoryKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
  static PFN_vkCopyAccelerationStructureToMemoryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyAccelerationStructureToMemoryKHR)vkl_resolve("vkCopyAccelerationStructureToMemoryKHR");
    if(pfn == NULL)
      vkl_missing("vkCopyAccelerationStructureToMemoryKHR");
    }
  return pfn(device, deferredOperation, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyImageToImage(VkDevice device, const VkCopyImageToImageInfo* pCopyImageToImageInfo)
{
  static PFN_vkCopyImageToImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyImageToImage)vkl_resolve("vkCopyImageToImage");
    if(pfn == NULL)
      vkl_missing("vkCopyImageToImage");
    }
  return pfn(device, pCopyImageToImageInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyImageToImageEXT(VkDevice device, const VkCopyImageToImageInfo* pCopyImageToImageInfo)
{
  static PFN_vkCopyImageToImageEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyImageToImageEXT)vkl_resolve("vkCopyImageToImageEXT");
    if(pfn == NULL)
      vkl_missing("vkCopyImageToImageEXT");
    }
  return pfn(device, pCopyImageToImageInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyImageToMemory(VkDevice device, const VkCopyImageToMemoryInfo* pCopyImageToMemoryInfo)
{
  static PFN_vkCopyImageToMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyImageToMemory)vkl_resolve("vkCopyImageToMemory");
    if(pfn == NULL)
      vkl_missing("vkCopyImageToMemory");
    }
  return pfn(device, pCopyImageToMemoryInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyImageToMemoryEXT(VkDevice device, const VkCopyImageToMemoryInfo* pCopyImageToMemoryInfo)
{
  static PFN_vkCopyImageToMemoryEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyImageToMemoryEXT)vkl_resolve("vkCopyImageToMemoryEXT");
    if(pfn == NULL)
      vkl_missing("vkCopyImageToMemoryEXT");
    }
  return pfn(device, pCopyImageToMemoryInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyMemoryToAccelerationStructureKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
  static PFN_vkCopyMemoryToAccelerationStructureKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyMemoryToAccelerationStructureKHR)vkl_resolve("vkCopyMemoryToAccelerationStructureKHR");
    if(pfn == NULL)
      vkl_missing("vkCopyMemoryToAccelerationStructureKHR");
    }
  return pfn(device, deferredOperation, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyMemoryToImage(VkDevice device, const VkCopyMemoryToImageInfo* pCopyMemoryToImageInfo)
{
  static PFN_vkCopyMemoryToImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyMemoryToImage)vkl_resolve("vkCopyMemoryToImage");
    if(pfn == NULL)
      vkl_missing("vkCopyMemoryToImage");
    }
  return pfn(device, pCopyMemoryToImageInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyMemoryToImageEXT(VkDevice device, const VkCopyMemoryToImageInfo* pCopyMemoryToImageInfo)
{
  static PFN_vkCopyMemoryToImageEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyMemoryToImageEXT)vkl_resolve("vkCopyMemoryToImageEXT");
    if(pfn == NULL)
      vkl_missing("vkCopyMemoryToImageEXT");
    }
  return pfn(device, pCopyMemoryToImageInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyMemoryToMicromapEXT(VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyMemoryToMicromapInfoEXT* pInfo)
{
  static PFN_vkCopyMemoryToMicromapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyMemoryToMicromapEXT)vkl_resolve("vkCopyMemoryToMicromapEXT");
    if(pfn == NULL)
      vkl_missing("vkCopyMemoryToMicromapEXT");
    }
  return pfn(device, deferredOperation, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyMicromapEXT(VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyMicromapInfoEXT* pInfo)
{
  static PFN_vkCopyMicromapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyMicromapEXT)vkl_resolve("vkCopyMicromapEXT");
    if(pfn == NULL)
      vkl_missing("vkCopyMicromapEXT");
    }
  return pfn(device, deferredOperation, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCopyMicromapToMemoryEXT(VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyMicromapToMemoryInfoEXT* pInfo)
{
  static PFN_vkCopyMicromapToMemoryEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCopyMicromapToMemoryEXT)vkl_resolve("vkCopyMicromapToMemoryEXT");
    if(pfn == NULL)
      vkl_missing("vkCopyMicromapToMemoryEXT");
    }
  return pfn(device, deferredOperation, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateAccelerationStructure2KHR(VkDevice device, const VkAccelerationStructureCreateInfo2KHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure)
{
  static PFN_vkCreateAccelerationStructure2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateAccelerationStructure2KHR)vkl_resolve("vkCreateAccelerationStructure2KHR");
    if(pfn == NULL)
      vkl_missing("vkCreateAccelerationStructure2KHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateAccelerationStructureKHR(VkDevice device, const VkAccelerationStructureCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure)
{
  static PFN_vkCreateAccelerationStructureKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateAccelerationStructureKHR)vkl_resolve("vkCreateAccelerationStructureKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateAccelerationStructureKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateAccelerationStructureNV(VkDevice device, const VkAccelerationStructureCreateInfoNV* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkAccelerationStructureNV* pAccelerationStructure)
{
  static PFN_vkCreateAccelerationStructureNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateAccelerationStructureNV)vkl_resolve("vkCreateAccelerationStructureNV");
    if(pfn == NULL)
      vkl_missing("vkCreateAccelerationStructureNV");
    }
  return pfn(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
{
  static PFN_vkCreateBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateBuffer)vkl_resolve("vkCreateBuffer");
    if(pfn == NULL)
      vkl_missing("vkCreateBuffer");
    }
  return pfn(device, pCreateInfo, pAllocator, pBuffer);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView)
{
  static PFN_vkCreateBufferView pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateBufferView)vkl_resolve("vkCreateBufferView");
    if(pfn == NULL)
      vkl_missing("vkCreateBufferView");
    }
  return pfn(device, pCreateInfo, pAllocator, pView);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool)
{
  static PFN_vkCreateCommandPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateCommandPool)vkl_resolve("vkCreateCommandPool");
    if(pfn == NULL)
      vkl_missing("vkCreateCommandPool");
    }
  return pfn(device, pCreateInfo, pAllocator, pCommandPool);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
  static PFN_vkCreateComputePipelines pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateComputePipelines)vkl_resolve("vkCreateComputePipelines");
    if(pfn == NULL)
      vkl_missing("vkCreateComputePipelines");
    }
  return pfn(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateCuFunctionNVX(VkDevice device, const VkCuFunctionCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCuFunctionNVX* pFunction)
{
  static PFN_vkCreateCuFunctionNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateCuFunctionNVX)vkl_resolve("vkCreateCuFunctionNVX");
    if(pfn == NULL)
      vkl_missing("vkCreateCuFunctionNVX");
    }
  return pfn(device, pCreateInfo, pAllocator, pFunction);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateCuModuleNVX(VkDevice device, const VkCuModuleCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCuModuleNVX* pModule)
{
  static PFN_vkCreateCuModuleNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateCuModuleNVX)vkl_resolve("vkCreateCuModuleNVX");
    if(pfn == NULL)
      vkl_missing("vkCreateCuModuleNVX");
    }
  return pfn(device, pCreateInfo, pAllocator, pModule);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDataGraphPipelineSessionARM(VkDevice device, const VkDataGraphPipelineSessionCreateInfoARM* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDataGraphPipelineSessionARM* pSession)
{
  static PFN_vkCreateDataGraphPipelineSessionARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDataGraphPipelineSessionARM)vkl_resolve("vkCreateDataGraphPipelineSessionARM");
    if(pfn == NULL)
      vkl_missing("vkCreateDataGraphPipelineSessionARM");
    }
  return pfn(device, pCreateInfo, pAllocator, pSession);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDataGraphPipelinesARM(VkDevice device, VkDeferredOperationKHR deferredOperation, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkDataGraphPipelineCreateInfoARM* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
  static PFN_vkCreateDataGraphPipelinesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDataGraphPipelinesARM)vkl_resolve("vkCreateDataGraphPipelinesARM");
    if(pfn == NULL)
      vkl_missing("vkCreateDataGraphPipelinesARM");
    }
  return pfn(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
  static PFN_vkCreateDebugReportCallbackEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDebugReportCallbackEXT)vkl_resolve("vkCreateDebugReportCallbackEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateDebugReportCallbackEXT");
    }
  return pfn(instance, pCreateInfo, pAllocator, pCallback);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
  static PFN_vkCreateDebugUtilsMessengerEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDebugUtilsMessengerEXT)vkl_resolve("vkCreateDebugUtilsMessengerEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateDebugUtilsMessengerEXT");
    }
  return pfn(instance, pCreateInfo, pAllocator, pMessenger);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDeferredOperationKHR(VkDevice device, const VkAllocationCallbacks* pAllocator, VkDeferredOperationKHR* pDeferredOperation)
{
  static PFN_vkCreateDeferredOperationKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDeferredOperationKHR)vkl_resolve("vkCreateDeferredOperationKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateDeferredOperationKHR");
    }
  return pfn(device, pAllocator, pDeferredOperation);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool)
{
  static PFN_vkCreateDescriptorPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDescriptorPool)vkl_resolve("vkCreateDescriptorPool");
    if(pfn == NULL)
      vkl_missing("vkCreateDescriptorPool");
    }
  return pfn(device, pCreateInfo, pAllocator, pDescriptorPool);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout)
{
  static PFN_vkCreateDescriptorSetLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDescriptorSetLayout)vkl_resolve("vkCreateDescriptorSetLayout");
    if(pfn == NULL)
      vkl_missing("vkCreateDescriptorSetLayout");
    }
  return pfn(device, pCreateInfo, pAllocator, pSetLayout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate)
{
  static PFN_vkCreateDescriptorUpdateTemplate pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDescriptorUpdateTemplate)vkl_resolve("vkCreateDescriptorUpdateTemplate");
    if(pfn == NULL)
      vkl_missing("vkCreateDescriptorUpdateTemplate");
    }
  return pfn(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorUpdateTemplateKHR(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate)
{
  static PFN_vkCreateDescriptorUpdateTemplateKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDescriptorUpdateTemplateKHR)vkl_resolve("vkCreateDescriptorUpdateTemplateKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateDescriptorUpdateTemplateKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
  static PFN_vkCreateDevice pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDevice)vkl_resolve("vkCreateDevice");
    if(pfn == NULL)
      vkl_missing("vkCreateDevice");
    }
  return pfn(physicalDevice, pCreateInfo, pAllocator, pDevice);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, const VkDisplayModeCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode)
{
  static PFN_vkCreateDisplayModeKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDisplayModeKHR)vkl_resolve("vkCreateDisplayModeKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateDisplayModeKHR");
    }
  return pfn(physicalDevice, display, pCreateInfo, pAllocator, pMode);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDisplayPlaneSurfaceKHR(VkInstance instance, const VkDisplaySurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
{
  static PFN_vkCreateDisplayPlaneSurfaceKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateDisplayPlaneSurfaceKHR)vkl_resolve("vkCreateDisplayPlaneSurfaceKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateDisplayPlaneSurfaceKHR");
    }
  return pfn(instance, pCreateInfo, pAllocator, pSurface);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent)
{
  static PFN_vkCreateEvent pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateEvent)vkl_resolve("vkCreateEvent");
    if(pfn == NULL)
      vkl_missing("vkCreateEvent");
    }
  return pfn(device, pCreateInfo, pAllocator, pEvent);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateExternalComputeQueueNV(VkDevice device, const VkExternalComputeQueueCreateInfoNV* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkExternalComputeQueueNV* pExternalQueue)
{
  static PFN_vkCreateExternalComputeQueueNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateExternalComputeQueueNV)vkl_resolve("vkCreateExternalComputeQueueNV");
    if(pfn == NULL)
      vkl_missing("vkCreateExternalComputeQueueNV");
    }
  return pfn(device, pCreateInfo, pAllocator, pExternalQueue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence)
{
  static PFN_vkCreateFence pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateFence)vkl_resolve("vkCreateFence");
    if(pfn == NULL)
      vkl_missing("vkCreateFence");
    }
  return pfn(device, pCreateInfo, pAllocator, pFence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer)
{
  static PFN_vkCreateFramebuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateFramebuffer)vkl_resolve("vkCreateFramebuffer");
    if(pfn == NULL)
      vkl_missing("vkCreateFramebuffer");
    }
  return pfn(device, pCreateInfo, pAllocator, pFramebuffer);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateGpaSessionAMD(VkDevice device, const VkGpaSessionCreateInfoAMD* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkGpaSessionAMD* pGpaSession)
{
  static PFN_vkCreateGpaSessionAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateGpaSessionAMD)vkl_resolve("vkCreateGpaSessionAMD");
    if(pfn == NULL)
      vkl_missing("vkCreateGpaSessionAMD");
    }
  return pfn(device, pCreateInfo, pAllocator, pGpaSession);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
  static PFN_vkCreateGraphicsPipelines pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateGraphicsPipelines)vkl_resolve("vkCreateGraphicsPipelines");
    if(pfn == NULL)
      vkl_missing("vkCreateGraphicsPipelines");
    }
  return pfn(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateHeadlessSurfaceEXT(VkInstance instance, const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
{
  static PFN_vkCreateHeadlessSurfaceEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateHeadlessSurfaceEXT)vkl_resolve("vkCreateHeadlessSurfaceEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateHeadlessSurfaceEXT");
    }
  return pfn(instance, pCreateInfo, pAllocator, pSurface);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
  static PFN_vkCreateImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateImage)vkl_resolve("vkCreateImage");
    if(pfn == NULL)
      vkl_missing("vkCreateImage");
    }
  return pfn(device, pCreateInfo, pAllocator, pImage);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView)
{
  static PFN_vkCreateImageView pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateImageView)vkl_resolve("vkCreateImageView");
    if(pfn == NULL)
      vkl_missing("vkCreateImageView");
    }
  return pfn(device, pCreateInfo, pAllocator, pView);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateIndirectCommandsLayoutEXT(VkDevice device, const VkIndirectCommandsLayoutCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkIndirectCommandsLayoutEXT* pIndirectCommandsLayout)
{
  static PFN_vkCreateIndirectCommandsLayoutEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateIndirectCommandsLayoutEXT)vkl_resolve("vkCreateIndirectCommandsLayoutEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateIndirectCommandsLayoutEXT");
    }
  return pfn(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateIndirectCommandsLayoutNV(VkDevice device, const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkIndirectCommandsLayoutNV* pIndirectCommandsLayout)
{
  static PFN_vkCreateIndirectCommandsLayoutNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateIndirectCommandsLayoutNV)vkl_resolve("vkCreateIndirectCommandsLayoutNV");
    if(pfn == NULL)
      vkl_missing("vkCreateIndirectCommandsLayoutNV");
    }
  return pfn(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateIndirectExecutionSetEXT(VkDevice device, const VkIndirectExecutionSetCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkIndirectExecutionSetEXT* pIndirectExecutionSet)
{
  static PFN_vkCreateIndirectExecutionSetEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateIndirectExecutionSetEXT)vkl_resolve("vkCreateIndirectExecutionSetEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateIndirectExecutionSetEXT");
    }
  return pfn(device, pCreateInfo, pAllocator, pIndirectExecutionSet);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateMicromapEXT(VkDevice device, const VkMicromapCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkMicromapEXT* pMicromap)
{
  static PFN_vkCreateMicromapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateMicromapEXT)vkl_resolve("vkCreateMicromapEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateMicromapEXT");
    }
  return pfn(device, pCreateInfo, pAllocator, pMicromap);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateOpticalFlowSessionNV(VkDevice device, const VkOpticalFlowSessionCreateInfoNV* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkOpticalFlowSessionNV* pSession)
{
  static PFN_vkCreateOpticalFlowSessionNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateOpticalFlowSessionNV)vkl_resolve("vkCreateOpticalFlowSessionNV");
    if(pfn == NULL)
      vkl_missing("vkCreateOpticalFlowSessionNV");
    }
  return pfn(device, pCreateInfo, pAllocator, pSession);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineBinariesKHR(VkDevice device, const VkPipelineBinaryCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineBinaryHandlesInfoKHR* pBinaries)
{
  static PFN_vkCreatePipelineBinariesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreatePipelineBinariesKHR)vkl_resolve("vkCreatePipelineBinariesKHR");
    if(pfn == NULL)
      vkl_missing("vkCreatePipelineBinariesKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pBinaries);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache)
{
  static PFN_vkCreatePipelineCache pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreatePipelineCache)vkl_resolve("vkCreatePipelineCache");
    if(pfn == NULL)
      vkl_missing("vkCreatePipelineCache");
    }
  return pfn(device, pCreateInfo, pAllocator, pPipelineCache);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout)
{
  static PFN_vkCreatePipelineLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreatePipelineLayout)vkl_resolve("vkCreatePipelineLayout");
    if(pfn == NULL)
      vkl_missing("vkCreatePipelineLayout");
    }
  return pfn(device, pCreateInfo, pAllocator, pPipelineLayout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePrivateDataSlot(VkDevice device, const VkPrivateDataSlotCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPrivateDataSlot* pPrivateDataSlot)
{
  static PFN_vkCreatePrivateDataSlot pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreatePrivateDataSlot)vkl_resolve("vkCreatePrivateDataSlot");
    if(pfn == NULL)
      vkl_missing("vkCreatePrivateDataSlot");
    }
  return pfn(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePrivateDataSlotEXT(VkDevice device, const VkPrivateDataSlotCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPrivateDataSlot* pPrivateDataSlot)
{
  static PFN_vkCreatePrivateDataSlotEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreatePrivateDataSlotEXT)vkl_resolve("vkCreatePrivateDataSlotEXT");
    if(pfn == NULL)
      vkl_missing("vkCreatePrivateDataSlotEXT");
    }
  return pfn(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool)
{
  static PFN_vkCreateQueryPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateQueryPool)vkl_resolve("vkCreateQueryPool");
    if(pfn == NULL)
      vkl_missing("vkCreateQueryPool");
    }
  return pfn(device, pCreateInfo, pAllocator, pQueryPool);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRayTracingPipelinesKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkRayTracingPipelineCreateInfoKHR* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
  static PFN_vkCreateRayTracingPipelinesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateRayTracingPipelinesKHR)vkl_resolve("vkCreateRayTracingPipelinesKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateRayTracingPipelinesKHR");
    }
  return pfn(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRayTracingPipelinesNV(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkRayTracingPipelineCreateInfoNV* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
  static PFN_vkCreateRayTracingPipelinesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateRayTracingPipelinesNV)vkl_resolve("vkCreateRayTracingPipelinesNV");
    if(pfn == NULL)
      vkl_missing("vkCreateRayTracingPipelinesNV");
    }
  return pfn(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
{
  static PFN_vkCreateRenderPass pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateRenderPass)vkl_resolve("vkCreateRenderPass");
    if(pfn == NULL)
      vkl_missing("vkCreateRenderPass");
    }
  return pfn(device, pCreateInfo, pAllocator, pRenderPass);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRenderPass2(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
{
  static PFN_vkCreateRenderPass2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateRenderPass2)vkl_resolve("vkCreateRenderPass2");
    if(pfn == NULL)
      vkl_missing("vkCreateRenderPass2");
    }
  return pfn(device, pCreateInfo, pAllocator, pRenderPass);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateRenderPass2KHR(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
{
  static PFN_vkCreateRenderPass2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateRenderPass2KHR)vkl_resolve("vkCreateRenderPass2KHR");
    if(pfn == NULL)
      vkl_missing("vkCreateRenderPass2KHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pRenderPass);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler)
{
  static PFN_vkCreateSampler pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateSampler)vkl_resolve("vkCreateSampler");
    if(pfn == NULL)
      vkl_missing("vkCreateSampler");
    }
  return pfn(device, pCreateInfo, pAllocator, pSampler);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion)
{
  static PFN_vkCreateSamplerYcbcrConversion pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateSamplerYcbcrConversion)vkl_resolve("vkCreateSamplerYcbcrConversion");
    if(pfn == NULL)
      vkl_missing("vkCreateSamplerYcbcrConversion");
    }
  return pfn(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSamplerYcbcrConversionKHR(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion)
{
  static PFN_vkCreateSamplerYcbcrConversionKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateSamplerYcbcrConversionKHR)vkl_resolve("vkCreateSamplerYcbcrConversionKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateSamplerYcbcrConversionKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore)
{
  static PFN_vkCreateSemaphore pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateSemaphore)vkl_resolve("vkCreateSemaphore");
    if(pfn == NULL)
      vkl_missing("vkCreateSemaphore");
    }
  return pfn(device, pCreateInfo, pAllocator, pSemaphore);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateShaderInstrumentationARM(VkDevice device, const VkShaderInstrumentationCreateInfoARM* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderInstrumentationARM* pInstrumentation)
{
  static PFN_vkCreateShaderInstrumentationARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateShaderInstrumentationARM)vkl_resolve("vkCreateShaderInstrumentationARM");
    if(pfn == NULL)
      vkl_missing("vkCreateShaderInstrumentationARM");
    }
  return pfn(device, pCreateInfo, pAllocator, pInstrumentation);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule)
{
  static PFN_vkCreateShaderModule pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateShaderModule)vkl_resolve("vkCreateShaderModule");
    if(pfn == NULL)
      vkl_missing("vkCreateShaderModule");
    }
  return pfn(device, pCreateInfo, pAllocator, pShaderModule);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateShadersEXT(VkDevice device, uint32_t createInfoCount, const VkShaderCreateInfoEXT* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders)
{
  static PFN_vkCreateShadersEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateShadersEXT)vkl_resolve("vkCreateShadersEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateShadersEXT");
    }
  return pfn(device, createInfoCount, pCreateInfos, pAllocator, pShaders);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount, const VkSwapchainCreateInfoKHR* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchains)
{
  static PFN_vkCreateSharedSwapchainsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateSharedSwapchainsKHR)vkl_resolve("vkCreateSharedSwapchainsKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateSharedSwapchainsKHR");
    }
  return pfn(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain)
{
  static PFN_vkCreateSwapchainKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateSwapchainKHR)vkl_resolve("vkCreateSwapchainKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateSwapchainKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pSwapchain);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateTensorARM(VkDevice device, const VkTensorCreateInfoARM* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkTensorARM* pTensor)
{
  static PFN_vkCreateTensorARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateTensorARM)vkl_resolve("vkCreateTensorARM");
    if(pfn == NULL)
      vkl_missing("vkCreateTensorARM");
    }
  return pfn(device, pCreateInfo, pAllocator, pTensor);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateTensorViewARM(VkDevice device, const VkTensorViewCreateInfoARM* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkTensorViewARM* pView)
{
  static PFN_vkCreateTensorViewARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateTensorViewARM)vkl_resolve("vkCreateTensorViewARM");
    if(pfn == NULL)
      vkl_missing("vkCreateTensorViewARM");
    }
  return pfn(device, pCreateInfo, pAllocator, pView);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateValidationCacheEXT(VkDevice device, const VkValidationCacheCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkValidationCacheEXT* pValidationCache)
{
  static PFN_vkCreateValidationCacheEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateValidationCacheEXT)vkl_resolve("vkCreateValidationCacheEXT");
    if(pfn == NULL)
      vkl_missing("vkCreateValidationCacheEXT");
    }
  return pfn(device, pCreateInfo, pAllocator, pValidationCache);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateVideoSessionKHR(VkDevice device, const VkVideoSessionCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkVideoSessionKHR* pVideoSession)
{
  static PFN_vkCreateVideoSessionKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateVideoSessionKHR)vkl_resolve("vkCreateVideoSessionKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateVideoSessionKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pVideoSession);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkCreateVideoSessionParametersKHR(VkDevice device, const VkVideoSessionParametersCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkVideoSessionParametersKHR* pVideoSessionParameters)
{
  static PFN_vkCreateVideoSessionParametersKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkCreateVideoSessionParametersKHR)vkl_resolve("vkCreateVideoSessionParametersKHR");
    if(pfn == NULL)
      vkl_missing("vkCreateVideoSessionParametersKHR");
    }
  return pfn(device, pCreateInfo, pAllocator, pVideoSessionParameters);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkDebugMarkerSetObjectNameEXT(VkDevice device, const VkDebugMarkerObjectNameInfoEXT* pNameInfo)
{
  static PFN_vkDebugMarkerSetObjectNameEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDebugMarkerSetObjectNameEXT)vkl_resolve("vkDebugMarkerSetObjectNameEXT");
    if(pfn == NULL)
      vkl_missing("vkDebugMarkerSetObjectNameEXT");
    }
  return pfn(device, pNameInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkDebugMarkerSetObjectTagEXT(VkDevice device, const VkDebugMarkerObjectTagInfoEXT* pTagInfo)
{
  static PFN_vkDebugMarkerSetObjectTagEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDebugMarkerSetObjectTagEXT)vkl_resolve("vkDebugMarkerSetObjectTagEXT");
    if(pfn == NULL)
      vkl_missing("vkDebugMarkerSetObjectTagEXT");
    }
  return pfn(device, pTagInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDebugReportMessageEXT(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage)
{
  static PFN_vkDebugReportMessageEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDebugReportMessageEXT)vkl_resolve("vkDebugReportMessageEXT");
    if(pfn == NULL)
      vkl_missing("vkDebugReportMessageEXT");
    }
  pfn(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkDeferredOperationJoinKHR(VkDevice device, VkDeferredOperationKHR operation)
{
  static PFN_vkDeferredOperationJoinKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDeferredOperationJoinKHR)vkl_resolve("vkDeferredOperationJoinKHR");
    if(pfn == NULL)
      vkl_missing("vkDeferredOperationJoinKHR");
    }
  return pfn(device, operation);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyAccelerationStructureKHR(VkDevice device, VkAccelerationStructureKHR accelerationStructure, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyAccelerationStructureKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyAccelerationStructureKHR)vkl_resolve("vkDestroyAccelerationStructureKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroyAccelerationStructureKHR");
    }
  pfn(device, accelerationStructure, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyAccelerationStructureNV(VkDevice device, VkAccelerationStructureNV accelerationStructure, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyAccelerationStructureNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyAccelerationStructureNV)vkl_resolve("vkDestroyAccelerationStructureNV");
    if(pfn == NULL)
      vkl_missing("vkDestroyAccelerationStructureNV");
    }
  pfn(device, accelerationStructure, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyBuffer)vkl_resolve("vkDestroyBuffer");
    if(pfn == NULL)
      vkl_missing("vkDestroyBuffer");
    }
  pfn(device, buffer, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyBufferView pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyBufferView)vkl_resolve("vkDestroyBufferView");
    if(pfn == NULL)
      vkl_missing("vkDestroyBufferView");
    }
  pfn(device, bufferView, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyCommandPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyCommandPool)vkl_resolve("vkDestroyCommandPool");
    if(pfn == NULL)
      vkl_missing("vkDestroyCommandPool");
    }
  pfn(device, commandPool, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyCuFunctionNVX(VkDevice device, VkCuFunctionNVX function, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyCuFunctionNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyCuFunctionNVX)vkl_resolve("vkDestroyCuFunctionNVX");
    if(pfn == NULL)
      vkl_missing("vkDestroyCuFunctionNVX");
    }
  pfn(device, function, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyCuModuleNVX(VkDevice device, VkCuModuleNVX module, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyCuModuleNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyCuModuleNVX)vkl_resolve("vkDestroyCuModuleNVX");
    if(pfn == NULL)
      vkl_missing("vkDestroyCuModuleNVX");
    }
  pfn(device, module, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDataGraphPipelineSessionARM(VkDevice device, VkDataGraphPipelineSessionARM session, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDataGraphPipelineSessionARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDataGraphPipelineSessionARM)vkl_resolve("vkDestroyDataGraphPipelineSessionARM");
    if(pfn == NULL)
      vkl_missing("vkDestroyDataGraphPipelineSessionARM");
    }
  pfn(device, session, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDebugReportCallbackEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDebugReportCallbackEXT)vkl_resolve("vkDestroyDebugReportCallbackEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyDebugReportCallbackEXT");
    }
  pfn(instance, callback, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDebugUtilsMessengerEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkl_resolve("vkDestroyDebugUtilsMessengerEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyDebugUtilsMessengerEXT");
    }
  pfn(instance, messenger, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDeferredOperationKHR(VkDevice device, VkDeferredOperationKHR operation, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDeferredOperationKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDeferredOperationKHR)vkl_resolve("vkDestroyDeferredOperationKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroyDeferredOperationKHR");
    }
  pfn(device, operation, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDescriptorPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDescriptorPool)vkl_resolve("vkDestroyDescriptorPool");
    if(pfn == NULL)
      vkl_missing("vkDestroyDescriptorPool");
    }
  pfn(device, descriptorPool, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDescriptorSetLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDescriptorSetLayout)vkl_resolve("vkDestroyDescriptorSetLayout");
    if(pfn == NULL)
      vkl_missing("vkDestroyDescriptorSetLayout");
    }
  pfn(device, descriptorSetLayout, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDescriptorUpdateTemplate pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDescriptorUpdateTemplate)vkl_resolve("vkDestroyDescriptorUpdateTemplate");
    if(pfn == NULL)
      vkl_missing("vkDestroyDescriptorUpdateTemplate");
    }
  pfn(device, descriptorUpdateTemplate, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorUpdateTemplateKHR(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDescriptorUpdateTemplateKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDescriptorUpdateTemplateKHR)vkl_resolve("vkDestroyDescriptorUpdateTemplateKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroyDescriptorUpdateTemplateKHR");
    }
  pfn(device, descriptorUpdateTemplate, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyDevice pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyDevice)vkl_resolve("vkDestroyDevice");
    if(pfn == NULL)
      vkl_missing("vkDestroyDevice");
    }
  pfn(device, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyEvent pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyEvent)vkl_resolve("vkDestroyEvent");
    if(pfn == NULL)
      vkl_missing("vkDestroyEvent");
    }
  pfn(device, event, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyExternalComputeQueueNV(VkDevice device, VkExternalComputeQueueNV externalQueue, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyExternalComputeQueueNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyExternalComputeQueueNV)vkl_resolve("vkDestroyExternalComputeQueueNV");
    if(pfn == NULL)
      vkl_missing("vkDestroyExternalComputeQueueNV");
    }
  pfn(device, externalQueue, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyFence pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyFence)vkl_resolve("vkDestroyFence");
    if(pfn == NULL)
      vkl_missing("vkDestroyFence");
    }
  pfn(device, fence, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyFramebuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyFramebuffer)vkl_resolve("vkDestroyFramebuffer");
    if(pfn == NULL)
      vkl_missing("vkDestroyFramebuffer");
    }
  pfn(device, framebuffer, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyGpaSessionAMD(VkDevice device, VkGpaSessionAMD gpaSession, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyGpaSessionAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyGpaSessionAMD)vkl_resolve("vkDestroyGpaSessionAMD");
    if(pfn == NULL)
      vkl_missing("vkDestroyGpaSessionAMD");
    }
  pfn(device, gpaSession, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyImage pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyImage)vkl_resolve("vkDestroyImage");
    if(pfn == NULL)
      vkl_missing("vkDestroyImage");
    }
  pfn(device, image, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyImageView pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyImageView)vkl_resolve("vkDestroyImageView");
    if(pfn == NULL)
      vkl_missing("vkDestroyImageView");
    }
  pfn(device, imageView, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyIndirectCommandsLayoutEXT(VkDevice device, VkIndirectCommandsLayoutEXT indirectCommandsLayout, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyIndirectCommandsLayoutEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyIndirectCommandsLayoutEXT)vkl_resolve("vkDestroyIndirectCommandsLayoutEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyIndirectCommandsLayoutEXT");
    }
  pfn(device, indirectCommandsLayout, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyIndirectCommandsLayoutNV(VkDevice device, VkIndirectCommandsLayoutNV indirectCommandsLayout, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyIndirectCommandsLayoutNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyIndirectCommandsLayoutNV)vkl_resolve("vkDestroyIndirectCommandsLayoutNV");
    if(pfn == NULL)
      vkl_missing("vkDestroyIndirectCommandsLayoutNV");
    }
  pfn(device, indirectCommandsLayout, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyIndirectExecutionSetEXT(VkDevice device, VkIndirectExecutionSetEXT indirectExecutionSet, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyIndirectExecutionSetEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyIndirectExecutionSetEXT)vkl_resolve("vkDestroyIndirectExecutionSetEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyIndirectExecutionSetEXT");
    }
  pfn(device, indirectExecutionSet, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyInstance pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyInstance)vkl_resolve("vkDestroyInstance");
    if(pfn == NULL)
      vkl_missing("vkDestroyInstance");
    }
  pfn(instance, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyMicromapEXT(VkDevice device, VkMicromapEXT micromap, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyMicromapEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyMicromapEXT)vkl_resolve("vkDestroyMicromapEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyMicromapEXT");
    }
  pfn(device, micromap, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyOpticalFlowSessionNV(VkDevice device, VkOpticalFlowSessionNV session, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyOpticalFlowSessionNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyOpticalFlowSessionNV)vkl_resolve("vkDestroyOpticalFlowSessionNV");
    if(pfn == NULL)
      vkl_missing("vkDestroyOpticalFlowSessionNV");
    }
  pfn(device, session, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyPipeline pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyPipeline)vkl_resolve("vkDestroyPipeline");
    if(pfn == NULL)
      vkl_missing("vkDestroyPipeline");
    }
  pfn(device, pipeline, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineBinaryKHR(VkDevice device, VkPipelineBinaryKHR pipelineBinary, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyPipelineBinaryKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyPipelineBinaryKHR)vkl_resolve("vkDestroyPipelineBinaryKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroyPipelineBinaryKHR");
    }
  pfn(device, pipelineBinary, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyPipelineCache pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyPipelineCache)vkl_resolve("vkDestroyPipelineCache");
    if(pfn == NULL)
      vkl_missing("vkDestroyPipelineCache");
    }
  pfn(device, pipelineCache, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyPipelineLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyPipelineLayout)vkl_resolve("vkDestroyPipelineLayout");
    if(pfn == NULL)
      vkl_missing("vkDestroyPipelineLayout");
    }
  pfn(device, pipelineLayout, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyPrivateDataSlot(VkDevice device, VkPrivateDataSlot privateDataSlot, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyPrivateDataSlot pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyPrivateDataSlot)vkl_resolve("vkDestroyPrivateDataSlot");
    if(pfn == NULL)
      vkl_missing("vkDestroyPrivateDataSlot");
    }
  pfn(device, privateDataSlot, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyPrivateDataSlotEXT(VkDevice device, VkPrivateDataSlot privateDataSlot, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyPrivateDataSlotEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyPrivateDataSlotEXT)vkl_resolve("vkDestroyPrivateDataSlotEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyPrivateDataSlotEXT");
    }
  pfn(device, privateDataSlot, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyQueryPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyQueryPool)vkl_resolve("vkDestroyQueryPool");
    if(pfn == NULL)
      vkl_missing("vkDestroyQueryPool");
    }
  pfn(device, queryPool, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyRenderPass pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyRenderPass)vkl_resolve("vkDestroyRenderPass");
    if(pfn == NULL)
      vkl_missing("vkDestroyRenderPass");
    }
  pfn(device, renderPass, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroySampler pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroySampler)vkl_resolve("vkDestroySampler");
    if(pfn == NULL)
      vkl_missing("vkDestroySampler");
    }
  pfn(device, sampler, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroySamplerYcbcrConversion pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroySamplerYcbcrConversion)vkl_resolve("vkDestroySamplerYcbcrConversion");
    if(pfn == NULL)
      vkl_missing("vkDestroySamplerYcbcrConversion");
    }
  pfn(device, ycbcrConversion, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroySamplerYcbcrConversionKHR(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroySamplerYcbcrConversionKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroySamplerYcbcrConversionKHR)vkl_resolve("vkDestroySamplerYcbcrConversionKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroySamplerYcbcrConversionKHR");
    }
  pfn(device, ycbcrConversion, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroySemaphore pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroySemaphore)vkl_resolve("vkDestroySemaphore");
    if(pfn == NULL)
      vkl_missing("vkDestroySemaphore");
    }
  pfn(device, semaphore, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyShaderEXT(VkDevice device, VkShaderEXT shader, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyShaderEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyShaderEXT)vkl_resolve("vkDestroyShaderEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyShaderEXT");
    }
  pfn(device, shader, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyShaderInstrumentationARM(VkDevice device, VkShaderInstrumentationARM instrumentation, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyShaderInstrumentationARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyShaderInstrumentationARM)vkl_resolve("vkDestroyShaderInstrumentationARM");
    if(pfn == NULL)
      vkl_missing("vkDestroyShaderInstrumentationARM");
    }
  pfn(device, instrumentation, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyShaderModule pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyShaderModule)vkl_resolve("vkDestroyShaderModule");
    if(pfn == NULL)
      vkl_missing("vkDestroyShaderModule");
    }
  pfn(device, shaderModule, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroySurfaceKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroySurfaceKHR)vkl_resolve("vkDestroySurfaceKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroySurfaceKHR");
    }
  pfn(instance, surface, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroySwapchainKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroySwapchainKHR)vkl_resolve("vkDestroySwapchainKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroySwapchainKHR");
    }
  pfn(device, swapchain, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyTensorARM(VkDevice device, VkTensorARM tensor, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyTensorARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyTensorARM)vkl_resolve("vkDestroyTensorARM");
    if(pfn == NULL)
      vkl_missing("vkDestroyTensorARM");
    }
  pfn(device, tensor, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyTensorViewARM(VkDevice device, VkTensorViewARM tensorView, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyTensorViewARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyTensorViewARM)vkl_resolve("vkDestroyTensorViewARM");
    if(pfn == NULL)
      vkl_missing("vkDestroyTensorViewARM");
    }
  pfn(device, tensorView, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyValidationCacheEXT(VkDevice device, VkValidationCacheEXT validationCache, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyValidationCacheEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyValidationCacheEXT)vkl_resolve("vkDestroyValidationCacheEXT");
    if(pfn == NULL)
      vkl_missing("vkDestroyValidationCacheEXT");
    }
  pfn(device, validationCache, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyVideoSessionKHR(VkDevice device, VkVideoSessionKHR videoSession, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyVideoSessionKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyVideoSessionKHR)vkl_resolve("vkDestroyVideoSessionKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroyVideoSessionKHR");
    }
  pfn(device, videoSession, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkDestroyVideoSessionParametersKHR(VkDevice device, VkVideoSessionParametersKHR videoSessionParameters, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkDestroyVideoSessionParametersKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDestroyVideoSessionParametersKHR)vkl_resolve("vkDestroyVideoSessionParametersKHR");
    if(pfn == NULL)
      vkl_missing("vkDestroyVideoSessionParametersKHR");
    }
  pfn(device, videoSessionParameters, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkDeviceWaitIdle(VkDevice device)
{
  static PFN_vkDeviceWaitIdle pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDeviceWaitIdle)vkl_resolve("vkDeviceWaitIdle");
    if(pfn == NULL)
      vkl_missing("vkDeviceWaitIdle");
    }
  return pfn(device);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkDisplayPowerControlEXT(VkDevice device, VkDisplayKHR display, const VkDisplayPowerInfoEXT* pDisplayPowerInfo)
{
  static PFN_vkDisplayPowerControlEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkDisplayPowerControlEXT)vkl_resolve("vkDisplayPowerControlEXT");
    if(pfn == NULL)
      vkl_missing("vkDisplayPowerControlEXT");
    }
  return pfn(device, display, pDisplayPowerInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
  static PFN_vkEndCommandBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEndCommandBuffer)vkl_resolve("vkEndCommandBuffer");
    if(pfn == NULL)
      vkl_missing("vkEndCommandBuffer");
    }
  return pfn(commandBuffer);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties)
{
  static PFN_vkEnumerateDeviceExtensionProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumerateDeviceExtensionProperties)vkl_resolve("vkEnumerateDeviceExtensionProperties");
    if(pfn == NULL)
      vkl_missing("vkEnumerateDeviceExtensionProperties");
    }
  return pfn(physicalDevice, pLayerName, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
  static PFN_vkEnumerateDeviceLayerProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumerateDeviceLayerProperties)vkl_resolve("vkEnumerateDeviceLayerProperties");
    if(pfn == NULL)
      vkl_missing("vkEnumerateDeviceLayerProperties");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceVersion(uint32_t* pApiVersion)
{
  static PFN_vkEnumerateInstanceVersion pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumerateInstanceVersion)vkl_resolve("vkEnumerateInstanceVersion");
    if(pfn == NULL)
      vkl_missing("vkEnumerateInstanceVersion");
    }
  return pfn(pApiVersion);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties)
{
  static PFN_vkEnumeratePhysicalDeviceGroups pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumeratePhysicalDeviceGroups)vkl_resolve("vkEnumeratePhysicalDeviceGroups");
    if(pfn == NULL)
      vkl_missing("vkEnumeratePhysicalDeviceGroups");
    }
  return pfn(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceGroupsKHR(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties)
{
  static PFN_vkEnumeratePhysicalDeviceGroupsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkl_resolve("vkEnumeratePhysicalDeviceGroupsKHR");
    if(pfn == NULL)
      vkl_missing("vkEnumeratePhysicalDeviceGroupsKHR");
    }
  return pfn(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, uint32_t* pCounterCount, VkPerformanceCounterARM* pCounters, VkPerformanceCounterDescriptionARM* pCounterDescriptions)
{
  static PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM)vkl_resolve("vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM");
    if(pfn == NULL)
      vkl_missing("vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM");
    }
  return pfn(physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, uint32_t* pCounterCount, VkPerformanceCounterKHR* pCounters, VkPerformanceCounterDescriptionKHR* pCounterDescriptions)
{
  static PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR)vkl_resolve("vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    if(pfn == NULL)
      vkl_missing("vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    }
  return pfn(physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM(VkPhysicalDevice physicalDevice, uint32_t* pDescriptionCount, VkShaderInstrumentationMetricDescriptionARM* pDescriptions)
{
  static PFN_vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM)vkl_resolve("vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM");
    if(pfn == NULL)
      vkl_missing("vkEnumeratePhysicalDeviceShaderInstrumentationMetricsARM");
    }
  return pfn(physicalDevice, pDescriptionCount, pDescriptions);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices)
{
  static PFN_vkEnumeratePhysicalDevices pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkEnumeratePhysicalDevices)vkl_resolve("vkEnumeratePhysicalDevices");
    if(pfn == NULL)
      vkl_missing("vkEnumeratePhysicalDevices");
    }
  return pfn(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
  static PFN_vkFlushMappedMemoryRanges pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkFlushMappedMemoryRanges)vkl_resolve("vkFlushMappedMemoryRanges");
    if(pfn == NULL)
      vkl_missing("vkFlushMappedMemoryRanges");
    }
  return pfn(device, memoryRangeCount, pMemoryRanges);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
  static PFN_vkFreeCommandBuffers pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkFreeCommandBuffers)vkl_resolve("vkFreeCommandBuffers");
    if(pfn == NULL)
      vkl_missing("vkFreeCommandBuffers");
    }
  pfn(device, commandPool, commandBufferCount, pCommandBuffers);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets)
{
  static PFN_vkFreeDescriptorSets pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkFreeDescriptorSets)vkl_resolve("vkFreeDescriptorSets");
    if(pfn == NULL)
      vkl_missing("vkFreeDescriptorSets");
    }
  return pfn(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkFreeMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkFreeMemory)vkl_resolve("vkFreeMemory");
    if(pfn == NULL)
      vkl_missing("vkFreeMemory");
    }
  pfn(device, memory, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetAccelerationStructureBuildSizesKHR(VkDevice device, VkAccelerationStructureBuildTypeKHR buildType, const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo, const uint32_t* pMaxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo)
{
  static PFN_vkGetAccelerationStructureBuildSizesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetAccelerationStructureBuildSizesKHR)vkl_resolve("vkGetAccelerationStructureBuildSizesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetAccelerationStructureBuildSizesKHR");
    }
  pfn(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetAccelerationStructureDeviceAddressKHR(VkDevice device, const VkAccelerationStructureDeviceAddressInfoKHR* pInfo)
{
  static PFN_vkGetAccelerationStructureDeviceAddressKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetAccelerationStructureDeviceAddressKHR)vkl_resolve("vkGetAccelerationStructureDeviceAddressKHR");
    if(pfn == NULL)
      vkl_missing("vkGetAccelerationStructureDeviceAddressKHR");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetAccelerationStructureHandleNV(VkDevice device, VkAccelerationStructureNV accelerationStructure, size_t dataSize, void* pData)
{
  static PFN_vkGetAccelerationStructureHandleNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetAccelerationStructureHandleNV)vkl_resolve("vkGetAccelerationStructureHandleNV");
    if(pfn == NULL)
      vkl_missing("vkGetAccelerationStructureHandleNV");
    }
  return pfn(device, accelerationStructure, dataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetAccelerationStructureMemoryRequirementsNV(VkDevice device, const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetAccelerationStructureMemoryRequirementsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetAccelerationStructureMemoryRequirementsNV)vkl_resolve("vkGetAccelerationStructureMemoryRequirementsNV");
    if(pfn == NULL)
      vkl_missing("vkGetAccelerationStructureMemoryRequirementsNV");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkAccelerationStructureCaptureDescriptorDataInfoEXT* pInfo, void* pData)
{
  static PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT)vkl_resolve("vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetBufferDeviceAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo)
{
  static PFN_vkGetBufferDeviceAddress pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferDeviceAddress)vkl_resolve("vkGetBufferDeviceAddress");
    if(pfn == NULL)
      vkl_missing("vkGetBufferDeviceAddress");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetBufferDeviceAddressEXT(VkDevice device, const VkBufferDeviceAddressInfo* pInfo)
{
  static PFN_vkGetBufferDeviceAddressEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferDeviceAddressEXT)vkl_resolve("vkGetBufferDeviceAddressEXT");
    if(pfn == NULL)
      vkl_missing("vkGetBufferDeviceAddressEXT");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetBufferDeviceAddressKHR(VkDevice device, const VkBufferDeviceAddressInfo* pInfo)
{
  static PFN_vkGetBufferDeviceAddressKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferDeviceAddressKHR)vkl_resolve("vkGetBufferDeviceAddressKHR");
    if(pfn == NULL)
      vkl_missing("vkGetBufferDeviceAddressKHR");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements)
{
  static PFN_vkGetBufferMemoryRequirements pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferMemoryRequirements)vkl_resolve("vkGetBufferMemoryRequirements");
    if(pfn == NULL)
      vkl_missing("vkGetBufferMemoryRequirements");
    }
  pfn(device, buffer, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetBufferMemoryRequirements2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferMemoryRequirements2)vkl_resolve("vkGetBufferMemoryRequirements2");
    if(pfn == NULL)
      vkl_missing("vkGetBufferMemoryRequirements2");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetBufferMemoryRequirements2KHR(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetBufferMemoryRequirements2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferMemoryRequirements2KHR)vkl_resolve("vkGetBufferMemoryRequirements2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetBufferMemoryRequirements2KHR");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR uint64_t VKAPI_CALL
vkGetBufferOpaqueCaptureAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo)
{
  static PFN_vkGetBufferOpaqueCaptureAddress pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferOpaqueCaptureAddress)vkl_resolve("vkGetBufferOpaqueCaptureAddress");
    if(pfn == NULL)
      vkl_missing("vkGetBufferOpaqueCaptureAddress");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR uint64_t VKAPI_CALL
vkGetBufferOpaqueCaptureAddressKHR(VkDevice device, const VkBufferDeviceAddressInfo* pInfo)
{
  static PFN_vkGetBufferOpaqueCaptureAddressKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferOpaqueCaptureAddressKHR)vkl_resolve("vkGetBufferOpaqueCaptureAddressKHR");
    if(pfn == NULL)
      vkl_missing("vkGetBufferOpaqueCaptureAddressKHR");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetBufferOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkBufferCaptureDescriptorDataInfoEXT* pInfo, void* pData)
{
  static PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT)vkl_resolve("vkGetBufferOpaqueCaptureDescriptorDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetBufferOpaqueCaptureDescriptorDataEXT");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetCalibratedTimestampsEXT(VkDevice device, uint32_t timestampCount, const VkCalibratedTimestampInfoKHR* pTimestampInfos, uint64_t* pTimestamps, uint64_t* pMaxDeviation)
{
  static PFN_vkGetCalibratedTimestampsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetCalibratedTimestampsEXT)vkl_resolve("vkGetCalibratedTimestampsEXT");
    if(pfn == NULL)
      vkl_missing("vkGetCalibratedTimestampsEXT");
    }
  return pfn(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetCalibratedTimestampsKHR(VkDevice device, uint32_t timestampCount, const VkCalibratedTimestampInfoKHR* pTimestampInfos, uint64_t* pTimestamps, uint64_t* pMaxDeviation)
{
  static PFN_vkGetCalibratedTimestampsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetCalibratedTimestampsKHR)vkl_resolve("vkGetCalibratedTimestampsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetCalibratedTimestampsKHR");
    }
  return pfn(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetClusterAccelerationStructureBuildSizesNV(VkDevice device, const VkClusterAccelerationStructureInputInfoNV* pInfo, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo)
{
  static PFN_vkGetClusterAccelerationStructureBuildSizesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetClusterAccelerationStructureBuildSizesNV)vkl_resolve("vkGetClusterAccelerationStructureBuildSizesNV");
    if(pfn == NULL)
      vkl_missing("vkGetClusterAccelerationStructureBuildSizesNV");
    }
  pfn(device, pInfo, pSizeInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDataGraphPipelineAvailablePropertiesARM(VkDevice device, const VkDataGraphPipelineInfoARM* pPipelineInfo, uint32_t* pPropertiesCount, VkDataGraphPipelinePropertyARM* pProperties)
{
  static PFN_vkGetDataGraphPipelineAvailablePropertiesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDataGraphPipelineAvailablePropertiesARM)vkl_resolve("vkGetDataGraphPipelineAvailablePropertiesARM");
    if(pfn == NULL)
      vkl_missing("vkGetDataGraphPipelineAvailablePropertiesARM");
    }
  return pfn(device, pPipelineInfo, pPropertiesCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDataGraphPipelinePropertiesARM(VkDevice device, const VkDataGraphPipelineInfoARM* pPipelineInfo, uint32_t propertiesCount, VkDataGraphPipelinePropertyQueryResultARM* pProperties)
{
  static PFN_vkGetDataGraphPipelinePropertiesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDataGraphPipelinePropertiesARM)vkl_resolve("vkGetDataGraphPipelinePropertiesARM");
    if(pfn == NULL)
      vkl_missing("vkGetDataGraphPipelinePropertiesARM");
    }
  return pfn(device, pPipelineInfo, propertiesCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDataGraphPipelineSessionBindPointRequirementsARM(VkDevice device, const VkDataGraphPipelineSessionBindPointRequirementsInfoARM* pInfo, uint32_t* pBindPointRequirementCount, VkDataGraphPipelineSessionBindPointRequirementARM* pBindPointRequirements)
{
  static PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM)vkl_resolve("vkGetDataGraphPipelineSessionBindPointRequirementsARM");
    if(pfn == NULL)
      vkl_missing("vkGetDataGraphPipelineSessionBindPointRequirementsARM");
    }
  return pfn(device, pInfo, pBindPointRequirementCount, pBindPointRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDataGraphPipelineSessionMemoryRequirementsARM(VkDevice device, const VkDataGraphPipelineSessionMemoryRequirementsInfoARM* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM)vkl_resolve("vkGetDataGraphPipelineSessionMemoryRequirementsARM");
    if(pfn == NULL)
      vkl_missing("vkGetDataGraphPipelineSessionMemoryRequirementsARM");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR uint32_t VKAPI_CALL
vkGetDeferredOperationMaxConcurrencyKHR(VkDevice device, VkDeferredOperationKHR operation)
{
  static PFN_vkGetDeferredOperationMaxConcurrencyKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeferredOperationMaxConcurrencyKHR)vkl_resolve("vkGetDeferredOperationMaxConcurrencyKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeferredOperationMaxConcurrencyKHR");
    }
  return pfn(device, operation);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeferredOperationResultKHR(VkDevice device, VkDeferredOperationKHR operation)
{
  static PFN_vkGetDeferredOperationResultKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeferredOperationResultKHR)vkl_resolve("vkGetDeferredOperationResultKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeferredOperationResultKHR");
    }
  return pfn(device, operation);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorEXT(VkDevice device, const VkDescriptorGetInfoEXT* pDescriptorInfo, size_t dataSize, void* pDescriptor)
{
  static PFN_vkGetDescriptorEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorEXT)vkl_resolve("vkGetDescriptorEXT");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorEXT");
    }
  pfn(device, pDescriptorInfo, dataSize, pDescriptor);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetHostMappingVALVE(VkDevice device, VkDescriptorSet descriptorSet, void** ppData)
{
  static PFN_vkGetDescriptorSetHostMappingVALVE pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorSetHostMappingVALVE)vkl_resolve("vkGetDescriptorSetHostMappingVALVE");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorSetHostMappingVALVE");
    }
  pfn(device, descriptorSet, ppData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetLayoutBindingOffsetEXT(VkDevice device, VkDescriptorSetLayout layout, uint32_t binding, VkDeviceSize* pOffset)
{
  static PFN_vkGetDescriptorSetLayoutBindingOffsetEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorSetLayoutBindingOffsetEXT)vkl_resolve("vkGetDescriptorSetLayoutBindingOffsetEXT");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorSetLayoutBindingOffsetEXT");
    }
  pfn(device, layout, binding, pOffset);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetLayoutHostMappingInfoVALVE(VkDevice device, const VkDescriptorSetBindingReferenceVALVE* pBindingReference, VkDescriptorSetLayoutHostMappingInfoVALVE* pHostMapping)
{
  static PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE)vkl_resolve("vkGetDescriptorSetLayoutHostMappingInfoVALVE");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorSetLayoutHostMappingInfoVALVE");
    }
  pfn(device, pBindingReference, pHostMapping);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetLayoutSizeEXT(VkDevice device, VkDescriptorSetLayout layout, VkDeviceSize* pLayoutSizeInBytes)
{
  static PFN_vkGetDescriptorSetLayoutSizeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorSetLayoutSizeEXT)vkl_resolve("vkGetDescriptorSetLayoutSizeEXT");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorSetLayoutSizeEXT");
    }
  pfn(device, layout, pLayoutSizeInBytes);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport)
{
  static PFN_vkGetDescriptorSetLayoutSupport pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorSetLayoutSupport)vkl_resolve("vkGetDescriptorSetLayoutSupport");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorSetLayoutSupport");
    }
  pfn(device, pCreateInfo, pSupport);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetLayoutSupportKHR(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport)
{
  static PFN_vkGetDescriptorSetLayoutSupportKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDescriptorSetLayoutSupportKHR)vkl_resolve("vkGetDescriptorSetLayoutSupportKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDescriptorSetLayoutSupportKHR");
    }
  pfn(device, pCreateInfo, pSupport);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceAccelerationStructureCompatibilityKHR(VkDevice device, const VkAccelerationStructureVersionInfoKHR* pVersionInfo, VkAccelerationStructureCompatibilityKHR* pCompatibility)
{
  static PFN_vkGetDeviceAccelerationStructureCompatibilityKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)vkl_resolve("vkGetDeviceAccelerationStructureCompatibilityKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceAccelerationStructureCompatibilityKHR");
    }
  pfn(device, pVersionInfo, pCompatibility);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceBufferMemoryRequirements(VkDevice device, const VkDeviceBufferMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetDeviceBufferMemoryRequirements pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceBufferMemoryRequirements)vkl_resolve("vkGetDeviceBufferMemoryRequirements");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceBufferMemoryRequirements");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceBufferMemoryRequirementsKHR(VkDevice device, const VkDeviceBufferMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetDeviceBufferMemoryRequirementsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceBufferMemoryRequirementsKHR)vkl_resolve("vkGetDeviceBufferMemoryRequirementsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceBufferMemoryRequirementsKHR");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR uint64_t VKAPI_CALL
vkGetDeviceCombinedImageSamplerIndexNVX(VkDevice device, uint64_t imageViewIndex, uint64_t samplerIndex)
{
  static PFN_vkGetDeviceCombinedImageSamplerIndexNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceCombinedImageSamplerIndexNVX)vkl_resolve("vkGetDeviceCombinedImageSamplerIndexNVX");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceCombinedImageSamplerIndexNVX");
    }
  return pfn(device, imageViewIndex, samplerIndex);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceFaultDebugInfoKHR(VkDevice device, VkDeviceFaultDebugInfoKHR* pDebugInfo)
{
  static PFN_vkGetDeviceFaultDebugInfoKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceFaultDebugInfoKHR)vkl_resolve("vkGetDeviceFaultDebugInfoKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceFaultDebugInfoKHR");
    }
  return pfn(device, pDebugInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceFaultInfoEXT(VkDevice device, VkDeviceFaultCountsEXT* pFaultCounts, VkDeviceFaultInfoEXT* pFaultInfo)
{
  static PFN_vkGetDeviceFaultInfoEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceFaultInfoEXT)vkl_resolve("vkGetDeviceFaultInfoEXT");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceFaultInfoEXT");
    }
  return pfn(device, pFaultCounts, pFaultInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceFaultReportsKHR(VkDevice device, uint64_t timeout, uint32_t* pFaultCounts, VkDeviceFaultInfoKHR* pFaultInfo)
{
  static PFN_vkGetDeviceFaultReportsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceFaultReportsKHR)vkl_resolve("vkGetDeviceFaultReportsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceFaultReportsKHR");
    }
  return pfn(device, timeout, pFaultCounts, pFaultInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures)
{
  static PFN_vkGetDeviceGroupPeerMemoryFeatures pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceGroupPeerMemoryFeatures)vkl_resolve("vkGetDeviceGroupPeerMemoryFeatures");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceGroupPeerMemoryFeatures");
    }
  pfn(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceGroupPeerMemoryFeaturesKHR(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures)
{
  static PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR)vkl_resolve("vkGetDeviceGroupPeerMemoryFeaturesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceGroupPeerMemoryFeaturesKHR");
    }
  pfn(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities)
{
  static PFN_vkGetDeviceGroupPresentCapabilitiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceGroupPresentCapabilitiesKHR)vkl_resolve("vkGetDeviceGroupPresentCapabilitiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceGroupPresentCapabilitiesKHR");
    }
  return pfn(device, pDeviceGroupPresentCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR* pModes)
{
  static PFN_vkGetDeviceGroupSurfacePresentModesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceGroupSurfacePresentModesKHR)vkl_resolve("vkGetDeviceGroupSurfacePresentModesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceGroupSurfacePresentModesKHR");
    }
  return pfn(device, surface, pModes);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceImageMemoryRequirements(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetDeviceImageMemoryRequirements pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceImageMemoryRequirements)vkl_resolve("vkGetDeviceImageMemoryRequirements");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceImageMemoryRequirements");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceImageMemoryRequirementsKHR(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetDeviceImageMemoryRequirementsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceImageMemoryRequirementsKHR)vkl_resolve("vkGetDeviceImageMemoryRequirementsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceImageMemoryRequirementsKHR");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceImageSparseMemoryRequirements(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements)
{
  static PFN_vkGetDeviceImageSparseMemoryRequirements pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceImageSparseMemoryRequirements)vkl_resolve("vkGetDeviceImageSparseMemoryRequirements");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceImageSparseMemoryRequirements");
    }
  pfn(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceImageSparseMemoryRequirementsKHR(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements)
{
  static PFN_vkGetDeviceImageSparseMemoryRequirementsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceImageSparseMemoryRequirementsKHR)vkl_resolve("vkGetDeviceImageSparseMemoryRequirementsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceImageSparseMemoryRequirementsKHR");
    }
  pfn(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceImageSubresourceLayout(VkDevice device, const VkDeviceImageSubresourceInfo* pInfo, VkSubresourceLayout2* pLayout)
{
  static PFN_vkGetDeviceImageSubresourceLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceImageSubresourceLayout)vkl_resolve("vkGetDeviceImageSubresourceLayout");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceImageSubresourceLayout");
    }
  pfn(device, pInfo, pLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceImageSubresourceLayoutKHR(VkDevice device, const VkDeviceImageSubresourceInfo* pInfo, VkSubresourceLayout2* pLayout)
{
  static PFN_vkGetDeviceImageSubresourceLayoutKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceImageSubresourceLayoutKHR)vkl_resolve("vkGetDeviceImageSubresourceLayoutKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceImageSubresourceLayoutKHR");
    }
  pfn(device, pInfo, pLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes)
{
  static PFN_vkGetDeviceMemoryCommitment pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceMemoryCommitment)vkl_resolve("vkGetDeviceMemoryCommitment");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceMemoryCommitment");
    }
  pfn(device, memory, pCommittedMemoryInBytes);
}

__attribute__((weak)) VKAPI_ATTR uint64_t VKAPI_CALL
vkGetDeviceMemoryOpaqueCaptureAddress(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo)
{
  static PFN_vkGetDeviceMemoryOpaqueCaptureAddress pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceMemoryOpaqueCaptureAddress)vkl_resolve("vkGetDeviceMemoryOpaqueCaptureAddress");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceMemoryOpaqueCaptureAddress");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR uint64_t VKAPI_CALL
vkGetDeviceMemoryOpaqueCaptureAddressKHR(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo)
{
  static PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR)vkl_resolve("vkGetDeviceMemoryOpaqueCaptureAddressKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceMemoryOpaqueCaptureAddressKHR");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceMicromapCompatibilityEXT(VkDevice device, const VkMicromapVersionInfoEXT* pVersionInfo, VkAccelerationStructureCompatibilityKHR* pCompatibility)
{
  static PFN_vkGetDeviceMicromapCompatibilityEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceMicromapCompatibilityEXT)vkl_resolve("vkGetDeviceMicromapCompatibilityEXT");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceMicromapCompatibilityEXT");
    }
  pfn(device, pVersionInfo, pCompatibility);
}

__attribute__((weak)) VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice device, const char* pName)
{
  static PFN_vkGetDeviceProcAddr pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceProcAddr)vkl_resolve("vkGetDeviceProcAddr");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceProcAddr");
    }
  return pfn(device, pName);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue)
{
  static PFN_vkGetDeviceQueue pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceQueue)vkl_resolve("vkGetDeviceQueue");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceQueue");
    }
  pfn(device, queueFamilyIndex, queueIndex, pQueue);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue)
{
  static PFN_vkGetDeviceQueue2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceQueue2)vkl_resolve("vkGetDeviceQueue2");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceQueue2");
    }
  pfn(device, pQueueInfo, pQueue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(VkDevice device, VkRenderPass renderpass, VkExtent2D* pMaxWorkgroupSize)
{
  static PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI)vkl_resolve("vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
    }
  return pfn(device, renderpass, pMaxWorkgroupSize);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetDeviceTensorMemoryRequirementsARM(VkDevice device, const VkDeviceTensorMemoryRequirementsARM* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetDeviceTensorMemoryRequirementsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDeviceTensorMemoryRequirementsARM)vkl_resolve("vkGetDeviceTensorMemoryRequirementsARM");
    if(pfn == NULL)
      vkl_missing("vkGetDeviceTensorMemoryRequirementsARM");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, uint32_t* pPropertyCount, VkDisplayModeProperties2KHR* pProperties)
{
  static PFN_vkGetDisplayModeProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDisplayModeProperties2KHR)vkl_resolve("vkGetDisplayModeProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetDisplayModeProperties2KHR");
    }
  return pfn(physicalDevice, display, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, uint32_t* pPropertyCount, VkDisplayModePropertiesKHR* pProperties)
{
  static PFN_vkGetDisplayModePropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDisplayModePropertiesKHR)vkl_resolve("vkGetDisplayModePropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDisplayModePropertiesKHR");
    }
  return pfn(physicalDevice, display, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo, VkDisplayPlaneCapabilities2KHR* pCapabilities)
{
  static PFN_vkGetDisplayPlaneCapabilities2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDisplayPlaneCapabilities2KHR)vkl_resolve("vkGetDisplayPlaneCapabilities2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetDisplayPlaneCapabilities2KHR");
    }
  return pfn(physicalDevice, pDisplayPlaneInfo, pCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode, uint32_t planeIndex, VkDisplayPlaneCapabilitiesKHR* pCapabilities)
{
  static PFN_vkGetDisplayPlaneCapabilitiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDisplayPlaneCapabilitiesKHR)vkl_resolve("vkGetDisplayPlaneCapabilitiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDisplayPlaneCapabilitiesKHR");
    }
  return pfn(physicalDevice, mode, planeIndex, pCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex, uint32_t* pDisplayCount, VkDisplayKHR* pDisplays)
{
  static PFN_vkGetDisplayPlaneSupportedDisplaysKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR)vkl_resolve("vkGetDisplayPlaneSupportedDisplaysKHR");
    if(pfn == NULL)
      vkl_missing("vkGetDisplayPlaneSupportedDisplaysKHR");
    }
  return pfn(physicalDevice, planeIndex, pDisplayCount, pDisplays);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDrmDisplayEXT(VkPhysicalDevice physicalDevice, int32_t drmFd, uint32_t connectorId, VkDisplayKHR* display)
{
  static PFN_vkGetDrmDisplayEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDrmDisplayEXT)vkl_resolve("vkGetDrmDisplayEXT");
    if(pfn == NULL)
      vkl_missing("vkGetDrmDisplayEXT");
    }
  return pfn(physicalDevice, drmFd, connectorId, display);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetDynamicRenderingTilePropertiesQCOM(VkDevice device, const VkRenderingInfo* pRenderingInfo, VkTilePropertiesQCOM* pProperties)
{
  static PFN_vkGetDynamicRenderingTilePropertiesQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetDynamicRenderingTilePropertiesQCOM)vkl_resolve("vkGetDynamicRenderingTilePropertiesQCOM");
    if(pfn == NULL)
      vkl_missing("vkGetDynamicRenderingTilePropertiesQCOM");
    }
  return pfn(device, pRenderingInfo, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetEncodedVideoSessionParametersKHR(VkDevice device, const VkVideoEncodeSessionParametersGetInfoKHR* pVideoSessionParametersInfo, VkVideoEncodeSessionParametersFeedbackInfoKHR* pFeedbackInfo, size_t* pDataSize, void* pData)
{
  static PFN_vkGetEncodedVideoSessionParametersKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetEncodedVideoSessionParametersKHR)vkl_resolve("vkGetEncodedVideoSessionParametersKHR");
    if(pfn == NULL)
      vkl_missing("vkGetEncodedVideoSessionParametersKHR");
    }
  return pfn(device, pVideoSessionParametersInfo, pFeedbackInfo, pDataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetEventStatus(VkDevice device, VkEvent event)
{
  static PFN_vkGetEventStatus pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetEventStatus)vkl_resolve("vkGetEventStatus");
    if(pfn == NULL)
      vkl_missing("vkGetEventStatus");
    }
  return pfn(device, event);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetExternalComputeQueueDataNV(VkExternalComputeQueueNV externalQueue, VkExternalComputeQueueDataParamsNV* params, void* pData)
{
  static PFN_vkGetExternalComputeQueueDataNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetExternalComputeQueueDataNV)vkl_resolve("vkGetExternalComputeQueueDataNV");
    if(pfn == NULL)
      vkl_missing("vkGetExternalComputeQueueDataNV");
    }
  pfn(externalQueue, params, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetFenceFdKHR(VkDevice device, const VkFenceGetFdInfoKHR* pGetFdInfo, int* pFd)
{
  static PFN_vkGetFenceFdKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetFenceFdKHR)vkl_resolve("vkGetFenceFdKHR");
    if(pfn == NULL)
      vkl_missing("vkGetFenceFdKHR");
    }
  return pfn(device, pGetFdInfo, pFd);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetFenceStatus(VkDevice device, VkFence fence)
{
  static PFN_vkGetFenceStatus pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetFenceStatus)vkl_resolve("vkGetFenceStatus");
    if(pfn == NULL)
      vkl_missing("vkGetFenceStatus");
    }
  return pfn(device, fence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetFramebufferTilePropertiesQCOM(VkDevice device, VkFramebuffer framebuffer, uint32_t* pPropertiesCount, VkTilePropertiesQCOM* pProperties)
{
  static PFN_vkGetFramebufferTilePropertiesQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetFramebufferTilePropertiesQCOM)vkl_resolve("vkGetFramebufferTilePropertiesQCOM");
    if(pfn == NULL)
      vkl_missing("vkGetFramebufferTilePropertiesQCOM");
    }
  return pfn(device, framebuffer, pPropertiesCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetGeneratedCommandsMemoryRequirementsEXT(VkDevice device, const VkGeneratedCommandsMemoryRequirementsInfoEXT* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetGeneratedCommandsMemoryRequirementsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetGeneratedCommandsMemoryRequirementsEXT)vkl_resolve("vkGetGeneratedCommandsMemoryRequirementsEXT");
    if(pfn == NULL)
      vkl_missing("vkGetGeneratedCommandsMemoryRequirementsEXT");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetGeneratedCommandsMemoryRequirementsNV(VkDevice device, const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetGeneratedCommandsMemoryRequirementsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetGeneratedCommandsMemoryRequirementsNV)vkl_resolve("vkGetGeneratedCommandsMemoryRequirementsNV");
    if(pfn == NULL)
      vkl_missing("vkGetGeneratedCommandsMemoryRequirementsNV");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetGpaDeviceClockInfoAMD(VkDevice device, VkGpaDeviceGetClockInfoAMD* pInfo)
{
  static PFN_vkGetGpaDeviceClockInfoAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetGpaDeviceClockInfoAMD)vkl_resolve("vkGetGpaDeviceClockInfoAMD");
    if(pfn == NULL)
      vkl_missing("vkGetGpaDeviceClockInfoAMD");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetGpaSessionResultsAMD(VkDevice device, VkGpaSessionAMD gpaSession, uint32_t sampleID, size_t* pSizeInBytes, void* pData)
{
  static PFN_vkGetGpaSessionResultsAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetGpaSessionResultsAMD)vkl_resolve("vkGetGpaSessionResultsAMD");
    if(pfn == NULL)
      vkl_missing("vkGetGpaSessionResultsAMD");
    }
  return pfn(device, gpaSession, sampleID, pSizeInBytes, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetGpaSessionStatusAMD(VkDevice device, VkGpaSessionAMD gpaSession)
{
  static PFN_vkGetGpaSessionStatusAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetGpaSessionStatusAMD)vkl_resolve("vkGetGpaSessionStatusAMD");
    if(pfn == NULL)
      vkl_missing("vkGetGpaSessionStatusAMD");
    }
  return pfn(device, gpaSession);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageDrmFormatModifierPropertiesEXT(VkDevice device, VkImage image, VkImageDrmFormatModifierPropertiesEXT* pProperties)
{
  static PFN_vkGetImageDrmFormatModifierPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageDrmFormatModifierPropertiesEXT)vkl_resolve("vkGetImageDrmFormatModifierPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetImageDrmFormatModifierPropertiesEXT");
    }
  return pfn(device, image, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements)
{
  static PFN_vkGetImageMemoryRequirements pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageMemoryRequirements)vkl_resolve("vkGetImageMemoryRequirements");
    if(pfn == NULL)
      vkl_missing("vkGetImageMemoryRequirements");
    }
  pfn(device, image, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetImageMemoryRequirements2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageMemoryRequirements2)vkl_resolve("vkGetImageMemoryRequirements2");
    if(pfn == NULL)
      vkl_missing("vkGetImageMemoryRequirements2");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageMemoryRequirements2KHR(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetImageMemoryRequirements2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageMemoryRequirements2KHR)vkl_resolve("vkGetImageMemoryRequirements2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetImageMemoryRequirements2KHR");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageOpaqueCaptureDataEXT(VkDevice device, uint32_t imageCount, const VkImage* pImages, VkHostAddressRangeEXT* pDatas)
{
  static PFN_vkGetImageOpaqueCaptureDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageOpaqueCaptureDataEXT)vkl_resolve("vkGetImageOpaqueCaptureDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetImageOpaqueCaptureDataEXT");
    }
  return pfn(device, imageCount, pImages, pDatas);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkImageCaptureDescriptorDataInfoEXT* pInfo, void* pData)
{
  static PFN_vkGetImageOpaqueCaptureDescriptorDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageOpaqueCaptureDescriptorDataEXT)vkl_resolve("vkGetImageOpaqueCaptureDescriptorDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetImageOpaqueCaptureDescriptorDataEXT");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements)
{
  static PFN_vkGetImageSparseMemoryRequirements pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSparseMemoryRequirements)vkl_resolve("vkGetImageSparseMemoryRequirements");
    if(pfn == NULL)
      vkl_missing("vkGetImageSparseMemoryRequirements");
    }
  pfn(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements)
{
  static PFN_vkGetImageSparseMemoryRequirements2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSparseMemoryRequirements2)vkl_resolve("vkGetImageSparseMemoryRequirements2");
    if(pfn == NULL)
      vkl_missing("vkGetImageSparseMemoryRequirements2");
    }
  pfn(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSparseMemoryRequirements2KHR(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements)
{
  static PFN_vkGetImageSparseMemoryRequirements2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSparseMemoryRequirements2KHR)vkl_resolve("vkGetImageSparseMemoryRequirements2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetImageSparseMemoryRequirements2KHR");
    }
  pfn(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout)
{
  static PFN_vkGetImageSubresourceLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSubresourceLayout)vkl_resolve("vkGetImageSubresourceLayout");
    if(pfn == NULL)
      vkl_missing("vkGetImageSubresourceLayout");
    }
  pfn(device, image, pSubresource, pLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout2(VkDevice device, VkImage image, const VkImageSubresource2* pSubresource, VkSubresourceLayout2* pLayout)
{
  static PFN_vkGetImageSubresourceLayout2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSubresourceLayout2)vkl_resolve("vkGetImageSubresourceLayout2");
    if(pfn == NULL)
      vkl_missing("vkGetImageSubresourceLayout2");
    }
  pfn(device, image, pSubresource, pLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout2EXT(VkDevice device, VkImage image, const VkImageSubresource2* pSubresource, VkSubresourceLayout2* pLayout)
{
  static PFN_vkGetImageSubresourceLayout2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSubresourceLayout2EXT)vkl_resolve("vkGetImageSubresourceLayout2EXT");
    if(pfn == NULL)
      vkl_missing("vkGetImageSubresourceLayout2EXT");
    }
  pfn(device, image, pSubresource, pLayout);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout2KHR(VkDevice device, VkImage image, const VkImageSubresource2* pSubresource, VkSubresourceLayout2* pLayout)
{
  static PFN_vkGetImageSubresourceLayout2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageSubresourceLayout2KHR)vkl_resolve("vkGetImageSubresourceLayout2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetImageSubresourceLayout2KHR");
    }
  pfn(device, image, pSubresource, pLayout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageViewAddressNVX(VkDevice device, VkImageView imageView, VkImageViewAddressPropertiesNVX* pProperties)
{
  static PFN_vkGetImageViewAddressNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageViewAddressNVX)vkl_resolve("vkGetImageViewAddressNVX");
    if(pfn == NULL)
      vkl_missing("vkGetImageViewAddressNVX");
    }
  return pfn(device, imageView, pProperties);
}

__attribute__((weak)) VKAPI_ATTR uint64_t VKAPI_CALL
vkGetImageViewHandle64NVX(VkDevice device, const VkImageViewHandleInfoNVX* pInfo)
{
  static PFN_vkGetImageViewHandle64NVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageViewHandle64NVX)vkl_resolve("vkGetImageViewHandle64NVX");
    if(pfn == NULL)
      vkl_missing("vkGetImageViewHandle64NVX");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR uint32_t VKAPI_CALL
vkGetImageViewHandleNVX(VkDevice device, const VkImageViewHandleInfoNVX* pInfo)
{
  static PFN_vkGetImageViewHandleNVX pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageViewHandleNVX)vkl_resolve("vkGetImageViewHandleNVX");
    if(pfn == NULL)
      vkl_missing("vkGetImageViewHandleNVX");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageViewOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkImageViewCaptureDescriptorDataInfoEXT* pInfo, void* pData)
{
  static PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT)vkl_resolve("vkGetImageViewOpaqueCaptureDescriptorDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetImageViewOpaqueCaptureDescriptorDataEXT");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetLatencyTimingsLegacyNV(VkDevice device, void* pTimings)
{
  static PFN_vkGetLatencyTimingsLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetLatencyTimingsLegacyNV)vkl_resolve("vkGetLatencyTimingsLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkGetLatencyTimingsLegacyNV");
    }
  pfn(device, pTimings);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetLatencyTimingsNV(VkDevice device, VkSwapchainKHR swapchain, VkGetLatencyMarkerInfoNV* pLatencyMarkerInfo)
{
  static PFN_vkGetLatencyTimingsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetLatencyTimingsNV)vkl_resolve("vkGetLatencyTimingsNV");
    if(pfn == NULL)
      vkl_missing("vkGetLatencyTimingsNV");
    }
  pfn(device, swapchain, pLatencyMarkerInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetMemoryFdKHR(VkDevice device, const VkMemoryGetFdInfoKHR* pGetFdInfo, int* pFd)
{
  static PFN_vkGetMemoryFdKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetMemoryFdKHR)vkl_resolve("vkGetMemoryFdKHR");
    if(pfn == NULL)
      vkl_missing("vkGetMemoryFdKHR");
    }
  return pfn(device, pGetFdInfo, pFd);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetMemoryFdPropertiesKHR(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, int fd, VkMemoryFdPropertiesKHR* pMemoryFdProperties)
{
  static PFN_vkGetMemoryFdPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetMemoryFdPropertiesKHR)vkl_resolve("vkGetMemoryFdPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetMemoryFdPropertiesKHR");
    }
  return pfn(device, handleType, fd, pMemoryFdProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetMemoryHostPointerPropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void* pHostPointer, VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties)
{
  static PFN_vkGetMemoryHostPointerPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetMemoryHostPointerPropertiesEXT)vkl_resolve("vkGetMemoryHostPointerPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetMemoryHostPointerPropertiesEXT");
    }
  return pfn(device, handleType, pHostPointer, pMemoryHostPointerProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetMemoryRemoteAddressNV(VkDevice device, const VkMemoryGetRemoteAddressInfoNV* pMemoryGetRemoteAddressInfo, VkRemoteAddressNV* pAddress)
{
  static PFN_vkGetMemoryRemoteAddressNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetMemoryRemoteAddressNV)vkl_resolve("vkGetMemoryRemoteAddressNV");
    if(pfn == NULL)
      vkl_missing("vkGetMemoryRemoteAddressNV");
    }
  return pfn(device, pMemoryGetRemoteAddressInfo, pAddress);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetMicromapBuildSizesEXT(VkDevice device, VkAccelerationStructureBuildTypeKHR buildType, const VkMicromapBuildInfoEXT* pBuildInfo, VkMicromapBuildSizesInfoEXT* pSizeInfo)
{
  static PFN_vkGetMicromapBuildSizesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetMicromapBuildSizesEXT)vkl_resolve("vkGetMicromapBuildSizesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetMicromapBuildSizesEXT");
    }
  pfn(device, buildType, pBuildInfo, pSizeInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPartitionedAccelerationStructuresBuildSizesNV(VkDevice device, const VkPartitionedAccelerationStructureInstancesInputNV* pInfo, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo)
{
  static PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV)vkl_resolve("vkGetPartitionedAccelerationStructuresBuildSizesNV");
    if(pfn == NULL)
      vkl_missing("vkGetPartitionedAccelerationStructuresBuildSizesNV");
    }
  pfn(device, pInfo, pSizeInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPastPresentationTimingEXT(VkDevice device, const VkPastPresentationTimingInfoEXT* pPastPresentationTimingInfo, VkPastPresentationTimingPropertiesEXT* pPastPresentationTimingProperties)
{
  static PFN_vkGetPastPresentationTimingEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPastPresentationTimingEXT)vkl_resolve("vkGetPastPresentationTimingEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPastPresentationTimingEXT");
    }
  return pfn(device, pPastPresentationTimingInfo, pPastPresentationTimingProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPastPresentationTimingGOOGLE(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pPresentationTimingCount, VkPastPresentationTimingGOOGLE* pPresentationTimings)
{
  static PFN_vkGetPastPresentationTimingGOOGLE pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPastPresentationTimingGOOGLE)vkl_resolve("vkGetPastPresentationTimingGOOGLE");
    if(pfn == NULL)
      vkl_missing("vkGetPastPresentationTimingGOOGLE");
    }
  return pfn(device, swapchain, pPresentationTimingCount, pPresentationTimings);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPerformanceParameterINTEL(VkDevice device, VkPerformanceParameterTypeINTEL parameter, VkPerformanceValueINTEL* pValue)
{
  static PFN_vkGetPerformanceParameterINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPerformanceParameterINTEL)vkl_resolve("vkGetPerformanceParameterINTEL");
    if(pfn == NULL)
      vkl_missing("vkGetPerformanceParameterINTEL");
    }
  return pfn(device, parameter, pValue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(VkPhysicalDevice physicalDevice, uint32_t* pTimeDomainCount, VkTimeDomainKHR* pTimeDomains)
{
  static PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT)vkl_resolve("vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
    }
  return pfn(physicalDevice, pTimeDomainCount, pTimeDomains);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCalibrateableTimeDomainsKHR(VkPhysicalDevice physicalDevice, uint32_t* pTimeDomainCount, VkTimeDomainKHR* pTimeDomains)
{
  static PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR)vkl_resolve("vkGetPhysicalDeviceCalibrateableTimeDomainsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCalibrateableTimeDomainsKHR");
    }
  return pfn(physicalDevice, pTimeDomainCount, pTimeDomains);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeMatrixFlexibleDimensionsPropertiesNV* pProperties)
{
  static PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV)vkl_resolve("vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCooperativeMatrixProperties2EXT(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceCooperativeMatrixInfo2EXT* pCooperativeMatrixInfo, uint32_t* pPropertyCount, VkCooperativeMatrixProperties2EXT* pProperties)
{
  static PFN_vkGetPhysicalDeviceCooperativeMatrixProperties2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCooperativeMatrixProperties2EXT)vkl_resolve("vkGetPhysicalDeviceCooperativeMatrixProperties2EXT");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCooperativeMatrixProperties2EXT");
    }
  return pfn(physicalDevice, pCooperativeMatrixInfo, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeMatrixPropertiesKHR* pProperties)
{
  static PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR)vkl_resolve("vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCooperativeMatrixPropertiesNV(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeMatrixPropertiesNV* pProperties)
{
  static PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV)vkl_resolve("vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceCooperativeVectorPropertiesNV(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeVectorPropertiesNV* pProperties)
{
  static PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV)vkl_resolve("vkGetPhysicalDeviceCooperativeVectorPropertiesNV");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceCooperativeVectorPropertiesNV");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceSize VKAPI_CALL
vkGetPhysicalDeviceDescriptorSizeEXT(VkPhysicalDevice physicalDevice, VkDescriptorType descriptorType)
{
  static PFN_vkGetPhysicalDeviceDescriptorSizeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceDescriptorSizeEXT)vkl_resolve("vkGetPhysicalDeviceDescriptorSizeEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceDescriptorSizeEXT");
    }
  return pfn(physicalDevice, descriptorType);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceDisplayPlaneProperties2KHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlaneProperties2KHR* pProperties)
{
  static PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR)vkl_resolve("vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties)
{
  static PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR)vkl_resolve("vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayProperties2KHR* pProperties)
{
  static PFN_vkGetPhysicalDeviceDisplayProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceDisplayProperties2KHR)vkl_resolve("vkGetPhysicalDeviceDisplayProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceDisplayProperties2KHR");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPropertiesKHR* pProperties)
{
  static PFN_vkGetPhysicalDeviceDisplayPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR)vkl_resolve("vkGetPhysicalDeviceDisplayPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceDisplayPropertiesKHR");
    }
  return pfn(physicalDevice, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties)
{
  static PFN_vkGetPhysicalDeviceExternalBufferProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalBufferProperties)vkl_resolve("vkGetPhysicalDeviceExternalBufferProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalBufferProperties");
    }
  pfn(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalBufferPropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties)
{
  static PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR)vkl_resolve("vkGetPhysicalDeviceExternalBufferPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalBufferPropertiesKHR");
    }
  pfn(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties)
{
  static PFN_vkGetPhysicalDeviceExternalFenceProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalFenceProperties)vkl_resolve("vkGetPhysicalDeviceExternalFenceProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalFenceProperties");
    }
  pfn(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalFencePropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties)
{
  static PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR)vkl_resolve("vkGetPhysicalDeviceExternalFencePropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalFencePropertiesKHR");
    }
  pfn(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceExternalImageFormatPropertiesNV(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkExternalMemoryHandleTypeFlagsNV externalHandleType, VkExternalImageFormatPropertiesNV* pExternalImageFormatProperties)
{
  static PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV)vkl_resolve("vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
    }
  return pfn(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties)
{
  static PFN_vkGetPhysicalDeviceExternalSemaphoreProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties)vkl_resolve("vkGetPhysicalDeviceExternalSemaphoreProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalSemaphoreProperties");
    }
  pfn(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalSemaphorePropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties)
{
  static PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR)vkl_resolve("vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
    }
  pfn(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceExternalTensorPropertiesARM(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalTensorInfoARM* pExternalTensorInfo, VkExternalTensorPropertiesARM* pExternalTensorProperties)
{
  static PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM)vkl_resolve("vkGetPhysicalDeviceExternalTensorPropertiesARM");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceExternalTensorPropertiesARM");
    }
  pfn(physicalDevice, pExternalTensorInfo, pExternalTensorProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures)
{
  static PFN_vkGetPhysicalDeviceFeatures pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFeatures)vkl_resolve("vkGetPhysicalDeviceFeatures");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFeatures");
    }
  pfn(physicalDevice, pFeatures);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures)
{
  static PFN_vkGetPhysicalDeviceFeatures2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFeatures2)vkl_resolve("vkGetPhysicalDeviceFeatures2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFeatures2");
    }
  pfn(physicalDevice, pFeatures);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures)
{
  static PFN_vkGetPhysicalDeviceFeatures2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFeatures2KHR)vkl_resolve("vkGetPhysicalDeviceFeatures2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFeatures2KHR");
    }
  pfn(physicalDevice, pFeatures);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties)
{
  static PFN_vkGetPhysicalDeviceFormatProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFormatProperties)vkl_resolve("vkGetPhysicalDeviceFormatProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFormatProperties");
    }
  pfn(physicalDevice, format, pFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties)
{
  static PFN_vkGetPhysicalDeviceFormatProperties2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFormatProperties2)vkl_resolve("vkGetPhysicalDeviceFormatProperties2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFormatProperties2");
    }
  pfn(physicalDevice, format, pFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFormatProperties2KHR(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties)
{
  static PFN_vkGetPhysicalDeviceFormatProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFormatProperties2KHR)vkl_resolve("vkGetPhysicalDeviceFormatProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFormatProperties2KHR");
    }
  pfn(physicalDevice, format, pFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceFragmentShadingRatesKHR(VkPhysicalDevice physicalDevice, uint32_t* pFragmentShadingRateCount, VkPhysicalDeviceFragmentShadingRateKHR* pFragmentShadingRates)
{
  static PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR)vkl_resolve("vkGetPhysicalDeviceFragmentShadingRatesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceFragmentShadingRatesKHR");
    }
  return pfn(physicalDevice, pFragmentShadingRateCount, pFragmentShadingRates);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties)
{
  static PFN_vkGetPhysicalDeviceImageFormatProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceImageFormatProperties)vkl_resolve("vkGetPhysicalDeviceImageFormatProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceImageFormatProperties");
    }
  return pfn(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties)
{
  static PFN_vkGetPhysicalDeviceImageFormatProperties2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceImageFormatProperties2)vkl_resolve("vkGetPhysicalDeviceImageFormatProperties2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceImageFormatProperties2");
    }
  return pfn(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties)
{
  static PFN_vkGetPhysicalDeviceImageFormatProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceImageFormatProperties2KHR)vkl_resolve("vkGetPhysicalDeviceImageFormatProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceImageFormatProperties2KHR");
    }
  return pfn(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties)
{
  static PFN_vkGetPhysicalDeviceMemoryProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceMemoryProperties)vkl_resolve("vkGetPhysicalDeviceMemoryProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceMemoryProperties");
    }
  pfn(physicalDevice, pMemoryProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties)
{
  static PFN_vkGetPhysicalDeviceMemoryProperties2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceMemoryProperties2)vkl_resolve("vkGetPhysicalDeviceMemoryProperties2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceMemoryProperties2");
    }
  pfn(physicalDevice, pMemoryProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties)
{
  static PFN_vkGetPhysicalDeviceMemoryProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceMemoryProperties2KHR)vkl_resolve("vkGetPhysicalDeviceMemoryProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceMemoryProperties2KHR");
    }
  pfn(physicalDevice, pMemoryProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMultisamplePropertiesEXT(VkPhysicalDevice physicalDevice, VkSampleCountFlagBits samples, VkMultisamplePropertiesEXT* pMultisampleProperties)
{
  static PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT)vkl_resolve("vkGetPhysicalDeviceMultisamplePropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceMultisamplePropertiesEXT");
    }
  pfn(physicalDevice, samples, pMultisampleProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceOpticalFlowImageFormatsNV(VkPhysicalDevice physicalDevice, const VkOpticalFlowImageFormatInfoNV* pOpticalFlowImageFormatInfo, uint32_t* pFormatCount, VkOpticalFlowImageFormatPropertiesNV* pImageFormatProperties)
{
  static PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV)vkl_resolve("vkGetPhysicalDeviceOpticalFlowImageFormatsNV");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceOpticalFlowImageFormatsNV");
    }
  return pfn(physicalDevice, pOpticalFlowImageFormatInfo, pFormatCount, pImageFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects)
{
  static PFN_vkGetPhysicalDevicePresentRectanglesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDevicePresentRectanglesKHR)vkl_resolve("vkGetPhysicalDevicePresentRectanglesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDevicePresentRectanglesKHR");
    }
  return pfn(physicalDevice, surface, pRectCount, pRects);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties)
{
  static PFN_vkGetPhysicalDeviceProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceProperties)vkl_resolve("vkGetPhysicalDeviceProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceProperties");
    }
  pfn(physicalDevice, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties)
{
  static PFN_vkGetPhysicalDeviceProperties2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceProperties2)vkl_resolve("vkGetPhysicalDeviceProperties2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceProperties2");
    }
  pfn(physicalDevice, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties)
{
  static PFN_vkGetPhysicalDeviceProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceProperties2KHR)vkl_resolve("vkGetPhysicalDeviceProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceProperties2KHR");
    }
  pfn(physicalDevice, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const VkQueueFamilyDataGraphPropertiesARM* pQueueFamilyDataGraphProperties, VkBaseOutStructure* pProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM)vkl_resolve("vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyDataGraphEngineOperationPropertiesARM");
    }
  return pfn(physicalDevice, queueFamilyIndex, pQueueFamilyDataGraphProperties, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, const VkQueueFamilyDataGraphPropertiesARM* pQueueFamilyDataGraphProperties, const VkDataGraphOpticalFlowImageFormatInfoARM* pOpticalFlowImageFormatInfo, uint32_t* pFormatCount, VkDataGraphOpticalFlowImageFormatPropertiesARM* pImageFormatProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM)vkl_resolve("vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyDataGraphOpticalFlowImageFormatsARM");
    }
  return pfn(physicalDevice, queueFamilyIndex, pQueueFamilyDataGraphProperties, pOpticalFlowImageFormatInfo, pFormatCount, pImageFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceQueueFamilyDataGraphProcessingEngineInfoARM* pQueueFamilyDataGraphProcessingEngineInfo, VkQueueFamilyDataGraphProcessingEnginePropertiesARM* pQueueFamilyDataGraphProcessingEngineProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM)vkl_resolve("vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM");
    }
  pfn(physicalDevice, pQueueFamilyDataGraphProcessingEngineInfo, pQueueFamilyDataGraphProcessingEngineProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, uint32_t* pQueueFamilyDataGraphPropertyCount, VkQueueFamilyDataGraphPropertiesARM* pQueueFamilyDataGraphProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM)vkl_resolve("vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM");
    }
  return pfn(physicalDevice, queueFamilyIndex, pQueueFamilyDataGraphPropertyCount, pQueueFamilyDataGraphProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(VkPhysicalDevice physicalDevice, const VkQueryPoolPerformanceCreateInfoKHR* pPerformanceQueryCreateInfo, uint32_t* pNumPasses)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR)vkl_resolve("vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
    }
  pfn(physicalDevice, pPerformanceQueryCreateInfo, pNumPasses);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)vkl_resolve("vkGetPhysicalDeviceQueueFamilyProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyProperties");
    }
  pfn(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2)vkl_resolve("vkGetPhysicalDeviceQueueFamilyProperties2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyProperties2");
    }
  pfn(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyProperties2KHR(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties)
{
  static PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR)vkl_resolve("vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    }
  pfn(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties)
{
  static PFN_vkGetPhysicalDeviceSparseImageFormatProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)vkl_resolve("vkGetPhysicalDeviceSparseImageFormatProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSparseImageFormatProperties");
    }
  pfn(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties)
{
  static PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2)vkl_resolve("vkGetPhysicalDeviceSparseImageFormatProperties2");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSparseImageFormatProperties2");
    }
  pfn(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceSparseImageFormatProperties2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties)
{
  static PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR)vkl_resolve("vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
    }
  pfn(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(VkPhysicalDevice physicalDevice, uint32_t* pCombinationCount, VkFramebufferMixedSamplesCombinationNV* pCombinations)
{
  static PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV)vkl_resolve("vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    }
  return pfn(physicalDevice, pCombinationCount, pCombinations);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilities2EXT(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilities2EXT* pSurfaceCapabilities)
{
  static PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT)vkl_resolve("vkGetPhysicalDeviceSurfaceCapabilities2EXT");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfaceCapabilities2EXT");
    }
  return pfn(physicalDevice, surface, pSurfaceCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, VkSurfaceCapabilities2KHR* pSurfaceCapabilities)
{
  static PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR)vkl_resolve("vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    }
  return pfn(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities)
{
  static PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkl_resolve("vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    }
  return pfn(physicalDevice, surface, pSurfaceCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats)
{
  static PFN_vkGetPhysicalDeviceSurfaceFormats2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfaceFormats2KHR)vkl_resolve("vkGetPhysicalDeviceSurfaceFormats2KHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfaceFormats2KHR");
    }
  return pfn(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats)
{
  static PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkl_resolve("vkGetPhysicalDeviceSurfaceFormatsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfaceFormatsKHR");
    }
  return pfn(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes)
{
  static PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkl_resolve("vkGetPhysicalDeviceSurfacePresentModesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfacePresentModesKHR");
    }
  return pfn(physicalDevice, surface, pPresentModeCount, pPresentModes);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported)
{
  static PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkl_resolve("vkGetPhysicalDeviceSurfaceSupportKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceSurfaceSupportKHR");
    }
  return pfn(physicalDevice, queueFamilyIndex, surface, pSupported);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceToolProperties(VkPhysicalDevice physicalDevice, uint32_t* pToolCount, VkPhysicalDeviceToolProperties* pToolProperties)
{
  static PFN_vkGetPhysicalDeviceToolProperties pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceToolProperties)vkl_resolve("vkGetPhysicalDeviceToolProperties");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceToolProperties");
    }
  return pfn(physicalDevice, pToolCount, pToolProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceToolPropertiesEXT(VkPhysicalDevice physicalDevice, uint32_t* pToolCount, VkPhysicalDeviceToolProperties* pToolProperties)
{
  static PFN_vkGetPhysicalDeviceToolPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceToolPropertiesEXT)vkl_resolve("vkGetPhysicalDeviceToolPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceToolPropertiesEXT");
    }
  return pfn(physicalDevice, pToolCount, pToolProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceVideoCapabilitiesKHR(VkPhysicalDevice physicalDevice, const VkVideoProfileInfoKHR* pVideoProfile, VkVideoCapabilitiesKHR* pCapabilities)
{
  static PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)vkl_resolve("vkGetPhysicalDeviceVideoCapabilitiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceVideoCapabilitiesKHR");
    }
  return pfn(physicalDevice, pVideoProfile, pCapabilities);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceVideoEncodeQualityLevelInfoKHR* pQualityLevelInfo, VkVideoEncodeQualityLevelPropertiesKHR* pQualityLevelProperties)
{
  static PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR)vkl_resolve("vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR");
    }
  return pfn(physicalDevice, pQualityLevelInfo, pQualityLevelProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceVideoFormatPropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceVideoFormatInfoKHR* pVideoFormatInfo, uint32_t* pVideoFormatPropertyCount, VkVideoFormatPropertiesKHR* pVideoFormatProperties)
{
  static PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR)vkl_resolve("vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    }
  return pfn(physicalDevice, pVideoFormatInfo, pVideoFormatPropertyCount, pVideoFormatProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineBinaryDataKHR(VkDevice device, const VkPipelineBinaryDataInfoKHR* pInfo, VkPipelineBinaryKeyKHR* pPipelineBinaryKey, size_t* pPipelineBinaryDataSize, void* pPipelineBinaryData)
{
  static PFN_vkGetPipelineBinaryDataKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineBinaryDataKHR)vkl_resolve("vkGetPipelineBinaryDataKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineBinaryDataKHR");
    }
  return pfn(device, pInfo, pPipelineBinaryKey, pPipelineBinaryDataSize, pPipelineBinaryData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData)
{
  static PFN_vkGetPipelineCacheData pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineCacheData)vkl_resolve("vkGetPipelineCacheData");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineCacheData");
    }
  return pfn(device, pipelineCache, pDataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineExecutableInternalRepresentationsKHR(VkDevice device, const VkPipelineExecutableInfoKHR* pExecutableInfo, uint32_t* pInternalRepresentationCount, VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations)
{
  static PFN_vkGetPipelineExecutableInternalRepresentationsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineExecutableInternalRepresentationsKHR)vkl_resolve("vkGetPipelineExecutableInternalRepresentationsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineExecutableInternalRepresentationsKHR");
    }
  return pfn(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineExecutablePropertiesKHR(VkDevice device, const VkPipelineInfoKHR* pPipelineInfo, uint32_t* pExecutableCount, VkPipelineExecutablePropertiesKHR* pProperties)
{
  static PFN_vkGetPipelineExecutablePropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineExecutablePropertiesKHR)vkl_resolve("vkGetPipelineExecutablePropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineExecutablePropertiesKHR");
    }
  return pfn(device, pPipelineInfo, pExecutableCount, pProperties);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineExecutableStatisticsKHR(VkDevice device, const VkPipelineExecutableInfoKHR* pExecutableInfo, uint32_t* pStatisticCount, VkPipelineExecutableStatisticKHR* pStatistics)
{
  static PFN_vkGetPipelineExecutableStatisticsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineExecutableStatisticsKHR)vkl_resolve("vkGetPipelineExecutableStatisticsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineExecutableStatisticsKHR");
    }
  return pfn(device, pExecutableInfo, pStatisticCount, pStatistics);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetPipelineIndirectDeviceAddressNV(VkDevice device, const VkPipelineIndirectDeviceAddressInfoNV* pInfo)
{
  static PFN_vkGetPipelineIndirectDeviceAddressNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineIndirectDeviceAddressNV)vkl_resolve("vkGetPipelineIndirectDeviceAddressNV");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineIndirectDeviceAddressNV");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPipelineIndirectMemoryRequirementsNV(VkDevice device, const VkComputePipelineCreateInfo* pCreateInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetPipelineIndirectMemoryRequirementsNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineIndirectMemoryRequirementsNV)vkl_resolve("vkGetPipelineIndirectMemoryRequirementsNV");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineIndirectMemoryRequirementsNV");
    }
  pfn(device, pCreateInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineKeyKHR(VkDevice device, const VkPipelineCreateInfoKHR* pPipelineCreateInfo, VkPipelineBinaryKeyKHR* pPipelineKey)
{
  static PFN_vkGetPipelineKeyKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelineKeyKHR)vkl_resolve("vkGetPipelineKeyKHR");
    if(pfn == NULL)
      vkl_missing("vkGetPipelineKeyKHR");
    }
  return pfn(device, pPipelineCreateInfo, pPipelineKey);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelinePropertiesEXT(VkDevice device, const VkPipelineInfoKHR* pPipelineInfo, VkBaseOutStructure* pPipelineProperties)
{
  static PFN_vkGetPipelinePropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPipelinePropertiesEXT)vkl_resolve("vkGetPipelinePropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPipelinePropertiesEXT");
    }
  return pfn(device, pPipelineInfo, pPipelineProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t* pData)
{
  static PFN_vkGetPrivateData pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPrivateData)vkl_resolve("vkGetPrivateData");
    if(pfn == NULL)
      vkl_missing("vkGetPrivateData");
    }
  pfn(device, objectType, objectHandle, privateDataSlot, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetPrivateDataEXT(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t* pData)
{
  static PFN_vkGetPrivateDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetPrivateDataEXT)vkl_resolve("vkGetPrivateDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetPrivateDataEXT");
    }
  pfn(device, objectType, objectHandle, privateDataSlot, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags)
{
  static PFN_vkGetQueryPoolResults pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetQueryPoolResults)vkl_resolve("vkGetQueryPoolResults");
    if(pfn == NULL)
      vkl_missing("vkGetQueryPoolResults");
    }
  return pfn(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetQueueCheckpointData2NV(VkQueue queue, uint32_t* pCheckpointDataCount, VkCheckpointData2NV* pCheckpointData)
{
  static PFN_vkGetQueueCheckpointData2NV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetQueueCheckpointData2NV)vkl_resolve("vkGetQueueCheckpointData2NV");
    if(pfn == NULL)
      vkl_missing("vkGetQueueCheckpointData2NV");
    }
  pfn(queue, pCheckpointDataCount, pCheckpointData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetQueueCheckpointDataNV(VkQueue queue, uint32_t* pCheckpointDataCount, VkCheckpointDataNV* pCheckpointData)
{
  static PFN_vkGetQueueCheckpointDataNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetQueueCheckpointDataNV)vkl_resolve("vkGetQueueCheckpointDataNV");
    if(pfn == NULL)
      vkl_missing("vkGetQueueCheckpointDataNV");
    }
  pfn(queue, pCheckpointDataCount, pCheckpointData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetRayTracingCaptureReplayShaderGroupHandlesKHR(VkDevice device, VkPipeline pipeline, uint32_t firstGroup, uint32_t groupCount, size_t dataSize, void* pData)
{
  static PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)vkl_resolve("vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    }
  return pfn(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetRayTracingShaderGroupHandlesKHR(VkDevice device, VkPipeline pipeline, uint32_t firstGroup, uint32_t groupCount, size_t dataSize, void* pData)
{
  static PFN_vkGetRayTracingShaderGroupHandlesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkl_resolve("vkGetRayTracingShaderGroupHandlesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetRayTracingShaderGroupHandlesKHR");
    }
  return pfn(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetRayTracingShaderGroupHandlesNV(VkDevice device, VkPipeline pipeline, uint32_t firstGroup, uint32_t groupCount, size_t dataSize, void* pData)
{
  static PFN_vkGetRayTracingShaderGroupHandlesNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRayTracingShaderGroupHandlesNV)vkl_resolve("vkGetRayTracingShaderGroupHandlesNV");
    if(pfn == NULL)
      vkl_missing("vkGetRayTracingShaderGroupHandlesNV");
    }
  return pfn(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkDeviceSize VKAPI_CALL
vkGetRayTracingShaderGroupStackSizeKHR(VkDevice device, VkPipeline pipeline, uint32_t group, VkShaderGroupShaderKHR groupShader)
{
  static PFN_vkGetRayTracingShaderGroupStackSizeKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)vkl_resolve("vkGetRayTracingShaderGroupStackSizeKHR");
    if(pfn == NULL)
      vkl_missing("vkGetRayTracingShaderGroupStackSizeKHR");
    }
  return pfn(device, pipeline, group, groupShader);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetRefreshCycleDurationGOOGLE(VkDevice device, VkSwapchainKHR swapchain, VkRefreshCycleDurationGOOGLE* pDisplayTimingProperties)
{
  static PFN_vkGetRefreshCycleDurationGOOGLE pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRefreshCycleDurationGOOGLE)vkl_resolve("vkGetRefreshCycleDurationGOOGLE");
    if(pfn == NULL)
      vkl_missing("vkGetRefreshCycleDurationGOOGLE");
    }
  return pfn(device, swapchain, pDisplayTimingProperties);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity)
{
  static PFN_vkGetRenderAreaGranularity pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRenderAreaGranularity)vkl_resolve("vkGetRenderAreaGranularity");
    if(pfn == NULL)
      vkl_missing("vkGetRenderAreaGranularity");
    }
  pfn(device, renderPass, pGranularity);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetRenderingAreaGranularity(VkDevice device, const VkRenderingAreaInfo* pRenderingAreaInfo, VkExtent2D* pGranularity)
{
  static PFN_vkGetRenderingAreaGranularity pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRenderingAreaGranularity)vkl_resolve("vkGetRenderingAreaGranularity");
    if(pfn == NULL)
      vkl_missing("vkGetRenderingAreaGranularity");
    }
  pfn(device, pRenderingAreaInfo, pGranularity);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetRenderingAreaGranularityKHR(VkDevice device, const VkRenderingAreaInfo* pRenderingAreaInfo, VkExtent2D* pGranularity)
{
  static PFN_vkGetRenderingAreaGranularityKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetRenderingAreaGranularityKHR)vkl_resolve("vkGetRenderingAreaGranularityKHR");
    if(pfn == NULL)
      vkl_missing("vkGetRenderingAreaGranularityKHR");
    }
  pfn(device, pRenderingAreaInfo, pGranularity);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSamplerOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkSamplerCaptureDescriptorDataInfoEXT* pInfo, void* pData)
{
  static PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT)vkl_resolve("vkGetSamplerOpaqueCaptureDescriptorDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetSamplerOpaqueCaptureDescriptorDataEXT");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore, uint64_t* pValue)
{
  static PFN_vkGetSemaphoreCounterValue pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSemaphoreCounterValue)vkl_resolve("vkGetSemaphoreCounterValue");
    if(pfn == NULL)
      vkl_missing("vkGetSemaphoreCounterValue");
    }
  return pfn(device, semaphore, pValue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSemaphoreCounterValueKHR(VkDevice device, VkSemaphore semaphore, uint64_t* pValue)
{
  static PFN_vkGetSemaphoreCounterValueKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSemaphoreCounterValueKHR)vkl_resolve("vkGetSemaphoreCounterValueKHR");
    if(pfn == NULL)
      vkl_missing("vkGetSemaphoreCounterValueKHR");
    }
  return pfn(device, semaphore, pValue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSemaphoreFdKHR(VkDevice device, const VkSemaphoreGetFdInfoKHR* pGetFdInfo, int* pFd)
{
  static PFN_vkGetSemaphoreFdKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSemaphoreFdKHR)vkl_resolve("vkGetSemaphoreFdKHR");
    if(pfn == NULL)
      vkl_missing("vkGetSemaphoreFdKHR");
    }
  return pfn(device, pGetFdInfo, pFd);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetShaderBinaryDataEXT(VkDevice device, VkShaderEXT shader, size_t* pDataSize, void* pData)
{
  static PFN_vkGetShaderBinaryDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetShaderBinaryDataEXT)vkl_resolve("vkGetShaderBinaryDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetShaderBinaryDataEXT");
    }
  return pfn(device, shader, pDataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetShaderInfoAMD(VkDevice device, VkPipeline pipeline, VkShaderStageFlagBits shaderStage, VkShaderInfoTypeAMD infoType, size_t* pInfoSize, void* pInfo)
{
  static PFN_vkGetShaderInfoAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetShaderInfoAMD)vkl_resolve("vkGetShaderInfoAMD");
    if(pfn == NULL)
      vkl_missing("vkGetShaderInfoAMD");
    }
  return pfn(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetShaderInstrumentationValuesARM(VkDevice device, VkShaderInstrumentationARM instrumentation, uint32_t* pMetricBlockCount, void* pMetricValues, VkShaderInstrumentationValuesFlagsARM flags)
{
  static PFN_vkGetShaderInstrumentationValuesARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetShaderInstrumentationValuesARM)vkl_resolve("vkGetShaderInstrumentationValuesARM");
    if(pfn == NULL)
      vkl_missing("vkGetShaderInstrumentationValuesARM");
    }
  return pfn(device, instrumentation, pMetricBlockCount, pMetricValues, flags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetShaderModuleCreateInfoIdentifierEXT(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, VkShaderModuleIdentifierEXT* pIdentifier)
{
  static PFN_vkGetShaderModuleCreateInfoIdentifierEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetShaderModuleCreateInfoIdentifierEXT)vkl_resolve("vkGetShaderModuleCreateInfoIdentifierEXT");
    if(pfn == NULL)
      vkl_missing("vkGetShaderModuleCreateInfoIdentifierEXT");
    }
  pfn(device, pCreateInfo, pIdentifier);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetShaderModuleIdentifierEXT(VkDevice device, VkShaderModule shaderModule, VkShaderModuleIdentifierEXT* pIdentifier)
{
  static PFN_vkGetShaderModuleIdentifierEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetShaderModuleIdentifierEXT)vkl_resolve("vkGetShaderModuleIdentifierEXT");
    if(pfn == NULL)
      vkl_missing("vkGetShaderModuleIdentifierEXT");
    }
  pfn(device, shaderModule, pIdentifier);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetSleepStatusLegacyNV(VkDevice device, VkBool32* pLowLatencyMode)
{
  static PFN_vkGetSleepStatusLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSleepStatusLegacyNV)vkl_resolve("vkGetSleepStatusLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkGetSleepStatusLegacyNV");
    }
  pfn(device, pLowLatencyMode);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainCounterEXT(VkDevice device, VkSwapchainKHR swapchain, VkSurfaceCounterFlagBitsEXT counter, uint64_t* pCounterValue)
{
  static PFN_vkGetSwapchainCounterEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSwapchainCounterEXT)vkl_resolve("vkGetSwapchainCounterEXT");
    if(pfn == NULL)
      vkl_missing("vkGetSwapchainCounterEXT");
    }
  return pfn(device, swapchain, counter, pCounterValue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages)
{
  static PFN_vkGetSwapchainImagesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSwapchainImagesKHR)vkl_resolve("vkGetSwapchainImagesKHR");
    if(pfn == NULL)
      vkl_missing("vkGetSwapchainImagesKHR");
    }
  return pfn(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainStatusKHR(VkDevice device, VkSwapchainKHR swapchain)
{
  static PFN_vkGetSwapchainStatusKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSwapchainStatusKHR)vkl_resolve("vkGetSwapchainStatusKHR");
    if(pfn == NULL)
      vkl_missing("vkGetSwapchainStatusKHR");
    }
  return pfn(device, swapchain);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainTimeDomainPropertiesEXT(VkDevice device, VkSwapchainKHR swapchain, VkSwapchainTimeDomainPropertiesEXT* pSwapchainTimeDomainProperties, uint64_t* pTimeDomainsCounter)
{
  static PFN_vkGetSwapchainTimeDomainPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSwapchainTimeDomainPropertiesEXT)vkl_resolve("vkGetSwapchainTimeDomainPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetSwapchainTimeDomainPropertiesEXT");
    }
  return pfn(device, swapchain, pSwapchainTimeDomainProperties, pTimeDomainsCounter);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainTimingPropertiesEXT(VkDevice device, VkSwapchainKHR swapchain, VkSwapchainTimingPropertiesEXT* pSwapchainTimingProperties, uint64_t* pSwapchainTimingPropertiesCounter)
{
  static PFN_vkGetSwapchainTimingPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetSwapchainTimingPropertiesEXT)vkl_resolve("vkGetSwapchainTimingPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkGetSwapchainTimingPropertiesEXT");
    }
  return pfn(device, swapchain, pSwapchainTimingProperties, pSwapchainTimingPropertiesCounter);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkGetTensorMemoryRequirementsARM(VkDevice device, const VkTensorMemoryRequirementsInfoARM* pInfo, VkMemoryRequirements2* pMemoryRequirements)
{
  static PFN_vkGetTensorMemoryRequirementsARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetTensorMemoryRequirementsARM)vkl_resolve("vkGetTensorMemoryRequirementsARM");
    if(pfn == NULL)
      vkl_missing("vkGetTensorMemoryRequirementsARM");
    }
  pfn(device, pInfo, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetTensorOpaqueCaptureDataARM(VkDevice device, uint32_t tensorCount, const VkTensorARM* pTensors, VkHostAddressRangeEXT* pDatas)
{
  static PFN_vkGetTensorOpaqueCaptureDataARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetTensorOpaqueCaptureDataARM)vkl_resolve("vkGetTensorOpaqueCaptureDataARM");
    if(pfn == NULL)
      vkl_missing("vkGetTensorOpaqueCaptureDataARM");
    }
  return pfn(device, tensorCount, pTensors, pDatas);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetTensorOpaqueCaptureDescriptorDataARM(VkDevice device, const VkTensorCaptureDescriptorDataInfoARM* pInfo, void* pData)
{
  static PFN_vkGetTensorOpaqueCaptureDescriptorDataARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetTensorOpaqueCaptureDescriptorDataARM)vkl_resolve("vkGetTensorOpaqueCaptureDescriptorDataARM");
    if(pfn == NULL)
      vkl_missing("vkGetTensorOpaqueCaptureDescriptorDataARM");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetTensorViewOpaqueCaptureDescriptorDataARM(VkDevice device, const VkTensorViewCaptureDescriptorDataInfoARM* pInfo, void* pData)
{
  static PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM)vkl_resolve("vkGetTensorViewOpaqueCaptureDescriptorDataARM");
    if(pfn == NULL)
      vkl_missing("vkGetTensorViewOpaqueCaptureDescriptorDataARM");
    }
  return pfn(device, pInfo, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetValidationCacheDataEXT(VkDevice device, VkValidationCacheEXT validationCache, size_t* pDataSize, void* pData)
{
  static PFN_vkGetValidationCacheDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetValidationCacheDataEXT)vkl_resolve("vkGetValidationCacheDataEXT");
    if(pfn == NULL)
      vkl_missing("vkGetValidationCacheDataEXT");
    }
  return pfn(device, validationCache, pDataSize, pData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkGetVideoSessionMemoryRequirementsKHR(VkDevice device, VkVideoSessionKHR videoSession, uint32_t* pMemoryRequirementsCount, VkVideoSessionMemoryRequirementsKHR* pMemoryRequirements)
{
  static PFN_vkGetVideoSessionMemoryRequirementsKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkGetVideoSessionMemoryRequirementsKHR)vkl_resolve("vkGetVideoSessionMemoryRequirementsKHR");
    if(pfn == NULL)
      vkl_missing("vkGetVideoSessionMemoryRequirementsKHR");
    }
  return pfn(device, videoSession, pMemoryRequirementsCount, pMemoryRequirements);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkImportFenceFdKHR(VkDevice device, const VkImportFenceFdInfoKHR* pImportFenceFdInfo)
{
  static PFN_vkImportFenceFdKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkImportFenceFdKHR)vkl_resolve("vkImportFenceFdKHR");
    if(pfn == NULL)
      vkl_missing("vkImportFenceFdKHR");
    }
  return pfn(device, pImportFenceFdInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkImportSemaphoreFdKHR(VkDevice device, const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo)
{
  static PFN_vkImportSemaphoreFdKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkImportSemaphoreFdKHR)vkl_resolve("vkImportSemaphoreFdKHR");
    if(pfn == NULL)
      vkl_missing("vkImportSemaphoreFdKHR");
    }
  return pfn(device, pImportSemaphoreFdInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkInitializePerformanceApiINTEL(VkDevice device, const VkInitializePerformanceApiInfoINTEL* pInitializeInfo)
{
  static PFN_vkInitializePerformanceApiINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkInitializePerformanceApiINTEL)vkl_resolve("vkInitializePerformanceApiINTEL");
    if(pfn == NULL)
      vkl_missing("vkInitializePerformanceApiINTEL");
    }
  return pfn(device, pInitializeInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
  static PFN_vkInvalidateMappedMemoryRanges pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkInvalidateMappedMemoryRanges)vkl_resolve("vkInvalidateMappedMemoryRanges");
    if(pfn == NULL)
      vkl_missing("vkInvalidateMappedMemoryRanges");
    }
  return pfn(device, memoryRangeCount, pMemoryRanges);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkLatencySleepLegacyNV(VkDevice device, VkSemaphore signalSemaphore, uint64_t value)
{
  static PFN_vkLatencySleepLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkLatencySleepLegacyNV)vkl_resolve("vkLatencySleepLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkLatencySleepLegacyNV");
    }
  pfn(device, signalSemaphore, value);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkLatencySleepNV(VkDevice device, VkSwapchainKHR swapchain, const VkLatencySleepInfoNV* pSleepInfo)
{
  static PFN_vkLatencySleepNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkLatencySleepNV)vkl_resolve("vkLatencySleepNV");
    if(pfn == NULL)
      vkl_missing("vkLatencySleepNV");
    }
  return pfn(device, swapchain, pSleepInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData)
{
  static PFN_vkMapMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkMapMemory)vkl_resolve("vkMapMemory");
    if(pfn == NULL)
      vkl_missing("vkMapMemory");
    }
  return pfn(device, memory, offset, size, flags, ppData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkMapMemory2(VkDevice device, const VkMemoryMapInfo* pMemoryMapInfo, void** ppData)
{
  static PFN_vkMapMemory2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkMapMemory2)vkl_resolve("vkMapMemory2");
    if(pfn == NULL)
      vkl_missing("vkMapMemory2");
    }
  return pfn(device, pMemoryMapInfo, ppData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkMapMemory2KHR(VkDevice device, const VkMemoryMapInfo* pMemoryMapInfo, void** ppData)
{
  static PFN_vkMapMemory2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkMapMemory2KHR)vkl_resolve("vkMapMemory2KHR");
    if(pfn == NULL)
      vkl_missing("vkMapMemory2KHR");
    }
  return pfn(device, pMemoryMapInfo, ppData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches)
{
  static PFN_vkMergePipelineCaches pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkMergePipelineCaches)vkl_resolve("vkMergePipelineCaches");
    if(pfn == NULL)
      vkl_missing("vkMergePipelineCaches");
    }
  return pfn(device, dstCache, srcCacheCount, pSrcCaches);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkMergeValidationCachesEXT(VkDevice device, VkValidationCacheEXT dstCache, uint32_t srcCacheCount, const VkValidationCacheEXT* pSrcCaches)
{
  static PFN_vkMergeValidationCachesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkMergeValidationCachesEXT)vkl_resolve("vkMergeValidationCachesEXT");
    if(pfn == NULL)
      vkl_missing("vkMergeValidationCachesEXT");
    }
  return pfn(device, dstCache, srcCacheCount, pSrcCaches);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkQueueBeginDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo)
{
  static PFN_vkQueueBeginDebugUtilsLabelEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueBeginDebugUtilsLabelEXT)vkl_resolve("vkQueueBeginDebugUtilsLabelEXT");
    if(pfn == NULL)
      vkl_missing("vkQueueBeginDebugUtilsLabelEXT");
    }
  pfn(queue, pLabelInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence)
{
  static PFN_vkQueueBindSparse pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueBindSparse)vkl_resolve("vkQueueBindSparse");
    if(pfn == NULL)
      vkl_missing("vkQueueBindSparse");
    }
  return pfn(queue, bindInfoCount, pBindInfo, fence);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkQueueEndDebugUtilsLabelEXT(VkQueue queue)
{
  static PFN_vkQueueEndDebugUtilsLabelEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueEndDebugUtilsLabelEXT)vkl_resolve("vkQueueEndDebugUtilsLabelEXT");
    if(pfn == NULL)
      vkl_missing("vkQueueEndDebugUtilsLabelEXT");
    }
  pfn(queue);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkQueueInsertDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo)
{
  static PFN_vkQueueInsertDebugUtilsLabelEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueInsertDebugUtilsLabelEXT)vkl_resolve("vkQueueInsertDebugUtilsLabelEXT");
    if(pfn == NULL)
      vkl_missing("vkQueueInsertDebugUtilsLabelEXT");
    }
  pfn(queue, pLabelInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkQueueNotifyOutOfBandLegacyNV(VkQueue queue, uint32_t queueType)
{
  static PFN_vkQueueNotifyOutOfBandLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueNotifyOutOfBandLegacyNV)vkl_resolve("vkQueueNotifyOutOfBandLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkQueueNotifyOutOfBandLegacyNV");
    }
  pfn(queue, queueType);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkQueueNotifyOutOfBandNV(VkQueue queue, const VkOutOfBandQueueTypeInfoNV* pQueueTypeInfo)
{
  static PFN_vkQueueNotifyOutOfBandNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueNotifyOutOfBandNV)vkl_resolve("vkQueueNotifyOutOfBandNV");
    if(pfn == NULL)
      vkl_missing("vkQueueNotifyOutOfBandNV");
    }
  pfn(queue, pQueueTypeInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
  static PFN_vkQueuePresentKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueuePresentKHR)vkl_resolve("vkQueuePresentKHR");
    if(pfn == NULL)
      vkl_missing("vkQueuePresentKHR");
    }
  return pfn(queue, pPresentInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSetPerfHintQCOM(VkQueue queue, const VkPerfHintInfoQCOM* pPerfHintInfo)
{
  static PFN_vkQueueSetPerfHintQCOM pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueSetPerfHintQCOM)vkl_resolve("vkQueueSetPerfHintQCOM");
    if(pfn == NULL)
      vkl_missing("vkQueueSetPerfHintQCOM");
    }
  return pfn(queue, pPerfHintInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSetPerformanceConfigurationINTEL(VkQueue queue, VkPerformanceConfigurationINTEL configuration)
{
  static PFN_vkQueueSetPerformanceConfigurationINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueSetPerformanceConfigurationINTEL)vkl_resolve("vkQueueSetPerformanceConfigurationINTEL");
    if(pfn == NULL)
      vkl_missing("vkQueueSetPerformanceConfigurationINTEL");
    }
  return pfn(queue, configuration);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence)
{
  static PFN_vkQueueSubmit pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueSubmit)vkl_resolve("vkQueueSubmit");
    if(pfn == NULL)
      vkl_missing("vkQueueSubmit");
    }
  return pfn(queue, submitCount, pSubmits, fence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSubmit2(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence)
{
  static PFN_vkQueueSubmit2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueSubmit2)vkl_resolve("vkQueueSubmit2");
    if(pfn == NULL)
      vkl_missing("vkQueueSubmit2");
    }
  return pfn(queue, submitCount, pSubmits, fence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSubmit2KHR(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence)
{
  static PFN_vkQueueSubmit2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueSubmit2KHR)vkl_resolve("vkQueueSubmit2KHR");
    if(pfn == NULL)
      vkl_missing("vkQueueSubmit2KHR");
    }
  return pfn(queue, submitCount, pSubmits, fence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkQueueWaitIdle(VkQueue queue)
{
  static PFN_vkQueueWaitIdle pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkQueueWaitIdle)vkl_resolve("vkQueueWaitIdle");
    if(pfn == NULL)
      vkl_missing("vkQueueWaitIdle");
    }
  return pfn(queue);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkRegisterCustomBorderColorEXT(VkDevice device, const VkSamplerCustomBorderColorCreateInfoEXT* pBorderColor, VkBool32 requestIndex, uint32_t* pIndex)
{
  static PFN_vkRegisterCustomBorderColorEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkRegisterCustomBorderColorEXT)vkl_resolve("vkRegisterCustomBorderColorEXT");
    if(pfn == NULL)
      vkl_missing("vkRegisterCustomBorderColorEXT");
    }
  return pfn(device, pBorderColor, requestIndex, pIndex);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkRegisterDeviceEventEXT(VkDevice device, const VkDeviceEventInfoEXT* pDeviceEventInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence)
{
  static PFN_vkRegisterDeviceEventEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkRegisterDeviceEventEXT)vkl_resolve("vkRegisterDeviceEventEXT");
    if(pfn == NULL)
      vkl_missing("vkRegisterDeviceEventEXT");
    }
  return pfn(device, pDeviceEventInfo, pAllocator, pFence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkRegisterDisplayEventEXT(VkDevice device, VkDisplayKHR display, const VkDisplayEventInfoEXT* pDisplayEventInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence)
{
  static PFN_vkRegisterDisplayEventEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkRegisterDisplayEventEXT)vkl_resolve("vkRegisterDisplayEventEXT");
    if(pfn == NULL)
      vkl_missing("vkRegisterDisplayEventEXT");
    }
  return pfn(device, display, pDisplayEventInfo, pAllocator, pFence);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkReleaseCapturedPipelineDataKHR(VkDevice device, const VkReleaseCapturedPipelineDataInfoKHR* pInfo, const VkAllocationCallbacks* pAllocator)
{
  static PFN_vkReleaseCapturedPipelineDataKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkReleaseCapturedPipelineDataKHR)vkl_resolve("vkReleaseCapturedPipelineDataKHR");
    if(pfn == NULL)
      vkl_missing("vkReleaseCapturedPipelineDataKHR");
    }
  return pfn(device, pInfo, pAllocator);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkReleaseDisplayEXT(VkPhysicalDevice physicalDevice, VkDisplayKHR display)
{
  static PFN_vkReleaseDisplayEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkReleaseDisplayEXT)vkl_resolve("vkReleaseDisplayEXT");
    if(pfn == NULL)
      vkl_missing("vkReleaseDisplayEXT");
    }
  return pfn(physicalDevice, display);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkReleasePerformanceConfigurationINTEL(VkDevice device, VkPerformanceConfigurationINTEL configuration)
{
  static PFN_vkReleasePerformanceConfigurationINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkReleasePerformanceConfigurationINTEL)vkl_resolve("vkReleasePerformanceConfigurationINTEL");
    if(pfn == NULL)
      vkl_missing("vkReleasePerformanceConfigurationINTEL");
    }
  return pfn(device, configuration);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkReleaseProfilingLockKHR(VkDevice device)
{
  static PFN_vkReleaseProfilingLockKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkReleaseProfilingLockKHR)vkl_resolve("vkReleaseProfilingLockKHR");
    if(pfn == NULL)
      vkl_missing("vkReleaseProfilingLockKHR");
    }
  pfn(device);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkReleaseSwapchainImagesEXT(VkDevice device, const VkReleaseSwapchainImagesInfoKHR* pReleaseInfo)
{
  static PFN_vkReleaseSwapchainImagesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkReleaseSwapchainImagesEXT)vkl_resolve("vkReleaseSwapchainImagesEXT");
    if(pfn == NULL)
      vkl_missing("vkReleaseSwapchainImagesEXT");
    }
  return pfn(device, pReleaseInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkReleaseSwapchainImagesKHR(VkDevice device, const VkReleaseSwapchainImagesInfoKHR* pReleaseInfo)
{
  static PFN_vkReleaseSwapchainImagesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkReleaseSwapchainImagesKHR)vkl_resolve("vkReleaseSwapchainImagesKHR");
    if(pfn == NULL)
      vkl_missing("vkReleaseSwapchainImagesKHR");
    }
  return pfn(device, pReleaseInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
{
  static PFN_vkResetCommandBuffer pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetCommandBuffer)vkl_resolve("vkResetCommandBuffer");
    if(pfn == NULL)
      vkl_missing("vkResetCommandBuffer");
    }
  return pfn(commandBuffer, flags);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
  static PFN_vkResetCommandPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetCommandPool)vkl_resolve("vkResetCommandPool");
    if(pfn == NULL)
      vkl_missing("vkResetCommandPool");
    }
  return pfn(device, commandPool, flags);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags)
{
  static PFN_vkResetDescriptorPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetDescriptorPool)vkl_resolve("vkResetDescriptorPool");
    if(pfn == NULL)
      vkl_missing("vkResetDescriptorPool");
    }
  return pfn(device, descriptorPool, flags);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkResetEvent(VkDevice device, VkEvent event)
{
  static PFN_vkResetEvent pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetEvent)vkl_resolve("vkResetEvent");
    if(pfn == NULL)
      vkl_missing("vkResetEvent");
    }
  return pfn(device, event);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences)
{
  static PFN_vkResetFences pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetFences)vkl_resolve("vkResetFences");
    if(pfn == NULL)
      vkl_missing("vkResetFences");
    }
  return pfn(device, fenceCount, pFences);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkResetGpaSessionAMD(VkDevice device, VkGpaSessionAMD gpaSession)
{
  static PFN_vkResetGpaSessionAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetGpaSessionAMD)vkl_resolve("vkResetGpaSessionAMD");
    if(pfn == NULL)
      vkl_missing("vkResetGpaSessionAMD");
    }
  return pfn(device, gpaSession);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkResetQueryPool(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
  static PFN_vkResetQueryPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetQueryPool)vkl_resolve("vkResetQueryPool");
    if(pfn == NULL)
      vkl_missing("vkResetQueryPool");
    }
  pfn(device, queryPool, firstQuery, queryCount);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkResetQueryPoolEXT(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
  static PFN_vkResetQueryPoolEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkResetQueryPoolEXT)vkl_resolve("vkResetQueryPoolEXT");
    if(pfn == NULL)
      vkl_missing("vkResetQueryPoolEXT");
    }
  pfn(device, queryPool, firstQuery, queryCount);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetDebugUtilsObjectNameEXT(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
{
  static PFN_vkSetDebugUtilsObjectNameEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetDebugUtilsObjectNameEXT)vkl_resolve("vkSetDebugUtilsObjectNameEXT");
    if(pfn == NULL)
      vkl_missing("vkSetDebugUtilsObjectNameEXT");
    }
  return pfn(device, pNameInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetDebugUtilsObjectTagEXT(VkDevice device, const VkDebugUtilsObjectTagInfoEXT* pTagInfo)
{
  static PFN_vkSetDebugUtilsObjectTagEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetDebugUtilsObjectTagEXT)vkl_resolve("vkSetDebugUtilsObjectTagEXT");
    if(pfn == NULL)
      vkl_missing("vkSetDebugUtilsObjectTagEXT");
    }
  return pfn(device, pTagInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSetDeviceMemoryPriorityEXT(VkDevice device, VkDeviceMemory memory, float priority)
{
  static PFN_vkSetDeviceMemoryPriorityEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetDeviceMemoryPriorityEXT)vkl_resolve("vkSetDeviceMemoryPriorityEXT");
    if(pfn == NULL)
      vkl_missing("vkSetDeviceMemoryPriorityEXT");
    }
  pfn(device, memory, priority);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetEvent(VkDevice device, VkEvent event)
{
  static PFN_vkSetEvent pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetEvent)vkl_resolve("vkSetEvent");
    if(pfn == NULL)
      vkl_missing("vkSetEvent");
    }
  return pfn(device, event);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetGpaDeviceClockModeAMD(VkDevice device, VkGpaDeviceClockModeInfoAMD* pInfo)
{
  static PFN_vkSetGpaDeviceClockModeAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetGpaDeviceClockModeAMD)vkl_resolve("vkSetGpaDeviceClockModeAMD");
    if(pfn == NULL)
      vkl_missing("vkSetGpaDeviceClockModeAMD");
    }
  return pfn(device, pInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSetHdrMetadataEXT(VkDevice device, uint32_t swapchainCount, const VkSwapchainKHR* pSwapchains, const VkHdrMetadataEXT* pMetadata)
{
  static PFN_vkSetHdrMetadataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetHdrMetadataEXT)vkl_resolve("vkSetHdrMetadataEXT");
    if(pfn == NULL)
      vkl_missing("vkSetHdrMetadataEXT");
    }
  pfn(device, swapchainCount, pSwapchains, pMetadata);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSetLatencyMarkerLegacyNV(VkDevice device, uint64_t frameID, uint32_t marker)
{
  static PFN_vkSetLatencyMarkerLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetLatencyMarkerLegacyNV)vkl_resolve("vkSetLatencyMarkerLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkSetLatencyMarkerLegacyNV");
    }
  pfn(device, frameID, marker);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSetLatencyMarkerNV(VkDevice device, VkSwapchainKHR swapchain, const VkSetLatencyMarkerInfoNV* pLatencyMarkerInfo)
{
  static PFN_vkSetLatencyMarkerNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetLatencyMarkerNV)vkl_resolve("vkSetLatencyMarkerNV");
    if(pfn == NULL)
      vkl_missing("vkSetLatencyMarkerNV");
    }
  pfn(device, swapchain, pLatencyMarkerInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSetLatencySleepModeLegacyNV(VkDevice device, VkBool32 lowLatencyMode, VkBool32 lowLatencyBoost, uint32_t minimumIntervalUs)
{
  static PFN_vkSetLatencySleepModeLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetLatencySleepModeLegacyNV)vkl_resolve("vkSetLatencySleepModeLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkSetLatencySleepModeLegacyNV");
    }
  pfn(device, lowLatencyMode, lowLatencyBoost, minimumIntervalUs);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetLatencySleepModeNV(VkDevice device, VkSwapchainKHR swapchain, const VkLatencySleepModeInfoNV* pSleepModeInfo)
{
  static PFN_vkSetLatencySleepModeNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetLatencySleepModeNV)vkl_resolve("vkSetLatencySleepModeNV");
    if(pfn == NULL)
      vkl_missing("vkSetLatencySleepModeNV");
    }
  return pfn(device, swapchain, pSleepModeInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSetLocalDimmingAMD(VkDevice device, VkSwapchainKHR swapChain, VkBool32 localDimmingEnable)
{
  static PFN_vkSetLocalDimmingAMD pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetLocalDimmingAMD)vkl_resolve("vkSetLocalDimmingAMD");
    if(pfn == NULL)
      vkl_missing("vkSetLocalDimmingAMD");
    }
  pfn(device, swapChain, localDimmingEnable);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data)
{
  static PFN_vkSetPrivateData pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetPrivateData)vkl_resolve("vkSetPrivateData");
    if(pfn == NULL)
      vkl_missing("vkSetPrivateData");
    }
  return pfn(device, objectType, objectHandle, privateDataSlot, data);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetPrivateDataEXT(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data)
{
  static PFN_vkSetPrivateDataEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetPrivateDataEXT)vkl_resolve("vkSetPrivateDataEXT");
    if(pfn == NULL)
      vkl_missing("vkSetPrivateDataEXT");
    }
  return pfn(device, objectType, objectHandle, privateDataSlot, data);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSetSwapchainPresentTimingQueueSizeEXT(VkDevice device, VkSwapchainKHR swapchain, uint32_t size)
{
  static PFN_vkSetSwapchainPresentTimingQueueSizeEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSetSwapchainPresentTimingQueueSizeEXT)vkl_resolve("vkSetSwapchainPresentTimingQueueSizeEXT");
    if(pfn == NULL)
      vkl_missing("vkSetSwapchainPresentTimingQueueSizeEXT");
    }
  return pfn(device, swapchain, size);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkShutdownLatencyDeviceLegacyNV(VkDevice device)
{
  static PFN_vkShutdownLatencyDeviceLegacyNV pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkShutdownLatencyDeviceLegacyNV)vkl_resolve("vkShutdownLatencyDeviceLegacyNV");
    if(pfn == NULL)
      vkl_missing("vkShutdownLatencyDeviceLegacyNV");
    }
  pfn(device);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSignalSemaphore(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo)
{
  static PFN_vkSignalSemaphore pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSignalSemaphore)vkl_resolve("vkSignalSemaphore");
    if(pfn == NULL)
      vkl_missing("vkSignalSemaphore");
    }
  return pfn(device, pSignalInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkSignalSemaphoreKHR(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo)
{
  static PFN_vkSignalSemaphoreKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSignalSemaphoreKHR)vkl_resolve("vkSignalSemaphoreKHR");
    if(pfn == NULL)
      vkl_missing("vkSignalSemaphoreKHR");
    }
  return pfn(device, pSignalInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
  static PFN_vkSubmitDebugUtilsMessageEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkSubmitDebugUtilsMessageEXT)vkl_resolve("vkSubmitDebugUtilsMessageEXT");
    if(pfn == NULL)
      vkl_missing("vkSubmitDebugUtilsMessageEXT");
    }
  pfn(instance, messageSeverity, messageTypes, pCallbackData);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkTransitionImageLayout(VkDevice device, uint32_t transitionCount, const VkHostImageLayoutTransitionInfo* pTransitions)
{
  static PFN_vkTransitionImageLayout pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkTransitionImageLayout)vkl_resolve("vkTransitionImageLayout");
    if(pfn == NULL)
      vkl_missing("vkTransitionImageLayout");
    }
  return pfn(device, transitionCount, pTransitions);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkTransitionImageLayoutEXT(VkDevice device, uint32_t transitionCount, const VkHostImageLayoutTransitionInfo* pTransitions)
{
  static PFN_vkTransitionImageLayoutEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkTransitionImageLayoutEXT)vkl_resolve("vkTransitionImageLayoutEXT");
    if(pfn == NULL)
      vkl_missing("vkTransitionImageLayoutEXT");
    }
  return pfn(device, transitionCount, pTransitions);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkTrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags)
{
  static PFN_vkTrimCommandPool pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkTrimCommandPool)vkl_resolve("vkTrimCommandPool");
    if(pfn == NULL)
      vkl_missing("vkTrimCommandPool");
    }
  pfn(device, commandPool, flags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkTrimCommandPoolKHR(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags)
{
  static PFN_vkTrimCommandPoolKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkTrimCommandPoolKHR)vkl_resolve("vkTrimCommandPoolKHR");
    if(pfn == NULL)
      vkl_missing("vkTrimCommandPoolKHR");
    }
  pfn(device, commandPool, flags);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUninitializePerformanceApiINTEL(VkDevice device)
{
  static PFN_vkUninitializePerformanceApiINTEL pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUninitializePerformanceApiINTEL)vkl_resolve("vkUninitializePerformanceApiINTEL");
    if(pfn == NULL)
      vkl_missing("vkUninitializePerformanceApiINTEL");
    }
  pfn(device);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
  static PFN_vkUnmapMemory pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUnmapMemory)vkl_resolve("vkUnmapMemory");
    if(pfn == NULL)
      vkl_missing("vkUnmapMemory");
    }
  pfn(device, memory);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkUnmapMemory2(VkDevice device, const VkMemoryUnmapInfo* pMemoryUnmapInfo)
{
  static PFN_vkUnmapMemory2 pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUnmapMemory2)vkl_resolve("vkUnmapMemory2");
    if(pfn == NULL)
      vkl_missing("vkUnmapMemory2");
    }
  return pfn(device, pMemoryUnmapInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkUnmapMemory2KHR(VkDevice device, const VkMemoryUnmapInfo* pMemoryUnmapInfo)
{
  static PFN_vkUnmapMemory2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUnmapMemory2KHR)vkl_resolve("vkUnmapMemory2KHR");
    if(pfn == NULL)
      vkl_missing("vkUnmapMemory2KHR");
    }
  return pfn(device, pMemoryUnmapInfo);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUnregisterCustomBorderColorEXT(VkDevice device, uint32_t index)
{
  static PFN_vkUnregisterCustomBorderColorEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUnregisterCustomBorderColorEXT)vkl_resolve("vkUnregisterCustomBorderColorEXT");
    if(pfn == NULL)
      vkl_missing("vkUnregisterCustomBorderColorEXT");
    }
  pfn(device, index);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData)
{
  static PFN_vkUpdateDescriptorSetWithTemplate pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUpdateDescriptorSetWithTemplate)vkl_resolve("vkUpdateDescriptorSetWithTemplate");
    if(pfn == NULL)
      vkl_missing("vkUpdateDescriptorSetWithTemplate");
    }
  pfn(device, descriptorSet, descriptorUpdateTemplate, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUpdateDescriptorSetWithTemplateKHR(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData)
{
  static PFN_vkUpdateDescriptorSetWithTemplateKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUpdateDescriptorSetWithTemplateKHR)vkl_resolve("vkUpdateDescriptorSetWithTemplateKHR");
    if(pfn == NULL)
      vkl_missing("vkUpdateDescriptorSetWithTemplateKHR");
    }
  pfn(device, descriptorSet, descriptorUpdateTemplate, pData);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies)
{
  static PFN_vkUpdateDescriptorSets pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUpdateDescriptorSets)vkl_resolve("vkUpdateDescriptorSets");
    if(pfn == NULL)
      vkl_missing("vkUpdateDescriptorSets");
    }
  pfn(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUpdateIndirectExecutionSetPipelineEXT(VkDevice device, VkIndirectExecutionSetEXT indirectExecutionSet, uint32_t executionSetWriteCount, const VkWriteIndirectExecutionSetPipelineEXT* pExecutionSetWrites)
{
  static PFN_vkUpdateIndirectExecutionSetPipelineEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUpdateIndirectExecutionSetPipelineEXT)vkl_resolve("vkUpdateIndirectExecutionSetPipelineEXT");
    if(pfn == NULL)
      vkl_missing("vkUpdateIndirectExecutionSetPipelineEXT");
    }
  pfn(device, indirectExecutionSet, executionSetWriteCount, pExecutionSetWrites);
}

__attribute__((weak)) VKAPI_ATTR void VKAPI_CALL
vkUpdateIndirectExecutionSetShaderEXT(VkDevice device, VkIndirectExecutionSetEXT indirectExecutionSet, uint32_t executionSetWriteCount, const VkWriteIndirectExecutionSetShaderEXT* pExecutionSetWrites)
{
  static PFN_vkUpdateIndirectExecutionSetShaderEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUpdateIndirectExecutionSetShaderEXT)vkl_resolve("vkUpdateIndirectExecutionSetShaderEXT");
    if(pfn == NULL)
      vkl_missing("vkUpdateIndirectExecutionSetShaderEXT");
    }
  pfn(device, indirectExecutionSet, executionSetWriteCount, pExecutionSetWrites);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkUpdateVideoSessionParametersKHR(VkDevice device, VkVideoSessionParametersKHR videoSessionParameters, const VkVideoSessionParametersUpdateInfoKHR* pUpdateInfo)
{
  static PFN_vkUpdateVideoSessionParametersKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkUpdateVideoSessionParametersKHR)vkl_resolve("vkUpdateVideoSessionParametersKHR");
    if(pfn == NULL)
      vkl_missing("vkUpdateVideoSessionParametersKHR");
    }
  return pfn(device, videoSessionParameters, pUpdateInfo);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout)
{
  static PFN_vkWaitForFences pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWaitForFences)vkl_resolve("vkWaitForFences");
    if(pfn == NULL)
      vkl_missing("vkWaitForFences");
    }
  return pfn(device, fenceCount, pFences, waitAll, timeout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWaitForPresent2KHR(VkDevice device, VkSwapchainKHR swapchain, const VkPresentWait2InfoKHR* pPresentWait2Info)
{
  static PFN_vkWaitForPresent2KHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWaitForPresent2KHR)vkl_resolve("vkWaitForPresent2KHR");
    if(pfn == NULL)
      vkl_missing("vkWaitForPresent2KHR");
    }
  return pfn(device, swapchain, pPresentWait2Info);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWaitForPresentKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t presentId, uint64_t timeout)
{
  static PFN_vkWaitForPresentKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWaitForPresentKHR)vkl_resolve("vkWaitForPresentKHR");
    if(pfn == NULL)
      vkl_missing("vkWaitForPresentKHR");
    }
  return pfn(device, swapchain, presentId, timeout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWaitSemaphores(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo, uint64_t timeout)
{
  static PFN_vkWaitSemaphores pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWaitSemaphores)vkl_resolve("vkWaitSemaphores");
    if(pfn == NULL)
      vkl_missing("vkWaitSemaphores");
    }
  return pfn(device, pWaitInfo, timeout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWaitSemaphoresKHR(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo, uint64_t timeout)
{
  static PFN_vkWaitSemaphoresKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWaitSemaphoresKHR)vkl_resolve("vkWaitSemaphoresKHR");
    if(pfn == NULL)
      vkl_missing("vkWaitSemaphoresKHR");
    }
  return pfn(device, pWaitInfo, timeout);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWriteAccelerationStructuresPropertiesKHR(VkDevice device, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures, VkQueryType queryType, size_t dataSize, void* pData, size_t stride)
{
  static PFN_vkWriteAccelerationStructuresPropertiesKHR pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWriteAccelerationStructuresPropertiesKHR)vkl_resolve("vkWriteAccelerationStructuresPropertiesKHR");
    if(pfn == NULL)
      vkl_missing("vkWriteAccelerationStructuresPropertiesKHR");
    }
  return pfn(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWriteMicromapsPropertiesEXT(VkDevice device, uint32_t micromapCount, const VkMicromapEXT* pMicromaps, VkQueryType queryType, size_t dataSize, void* pData, size_t stride)
{
  static PFN_vkWriteMicromapsPropertiesEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWriteMicromapsPropertiesEXT)vkl_resolve("vkWriteMicromapsPropertiesEXT");
    if(pfn == NULL)
      vkl_missing("vkWriteMicromapsPropertiesEXT");
    }
  return pfn(device, micromapCount, pMicromaps, queryType, dataSize, pData, stride);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWriteResourceDescriptorsEXT(VkDevice device, uint32_t resourceCount, const VkResourceDescriptorInfoEXT* pResources, const VkHostAddressRangeEXT* pDescriptors)
{
  static PFN_vkWriteResourceDescriptorsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWriteResourceDescriptorsEXT)vkl_resolve("vkWriteResourceDescriptorsEXT");
    if(pfn == NULL)
      vkl_missing("vkWriteResourceDescriptorsEXT");
    }
  return pfn(device, resourceCount, pResources, pDescriptors);
}

__attribute__((weak)) VKAPI_ATTR VkResult VKAPI_CALL
vkWriteSamplerDescriptorsEXT(VkDevice device, uint32_t samplerCount, const VkSamplerCreateInfo* pSamplers, const VkHostAddressRangeEXT* pDescriptors)
{
  static PFN_vkWriteSamplerDescriptorsEXT pfn;
  if(pfn == NULL) {
    pfn = (PFN_vkWriteSamplerDescriptorsEXT)vkl_resolve("vkWriteSamplerDescriptorsEXT");
    if(pfn == NULL)
      vkl_missing("vkWriteSamplerDescriptorsEXT");
    }
  return pfn(device, samplerCount, pSamplers, pDescriptors);
}
