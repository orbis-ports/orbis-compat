/* Copyright © 2026 Mikołaj Mikołajczyk
 * SPDX-License-Identifier: MIT
 *
 * A triangle on the television, from the bundle, in one file.
 *
 * WHAT THIS IS FOR. hello/ proves the bundle's LAYOUT: corrected headers, the overlay archive under
 * --whole-archive, the linker script, crt1.o, and create-fself turning the result into an eboot. It
 * deliberately touches no Mesa. This one is the other half - the workflow a person actually came
 * here for. It answers, in one run and with no engine, whether RADV comes up on this console and
 * whether a frame reaches the screen.
 *
 * ⚠ THE SURFACE IS HEADLESS, AND THAT IS THE ONE PLATFORM-SPECIFIC LINE IN THE WHOLE FILE. There is
 * no window system here, so there is no VkSurfaceKHR to attach to one. The port creates a
 * VK_EXT_headless_surface and mesa-ps4's wsi_orbis.c turns its presents into
 * sceVideoOutRegisterBuffers + sceVideoOutSubmitFlip. Tempest does exactly this and says why:
 *
 *     ⚠ hwnd IS IGNORED (...) a headless surface has nothing to attach to anyway: it names THE
 *     display, of which there is one.                    - Tempest vswapchain.cpp:264
 *
 * Everything else below is ordinary Vulkan and would compile for any platform.
 *
 * ⚠ AND IT DOES NOT RETURN FROM main(). Returning is reported as CE-34878-0 on this console and
 * reads exactly like a crash - see README section 0. It draws, presents, reports, and idles. */

#include <orbis_prefix.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <vulkan/vulkan.h>
#include <orbis/libkernel.h>

#include "tri_vert_spv.h"
#include "tri_frag_spv.h"

/* One channel, used by everything below. sceKernelDebugOutText is the console's own and needs no
 * library of ours, which matters when the thing being tested is the graphics stack. */
static void say(const char *fmt, ...)
{
    char    buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf) - 2, fmt, ap);
    va_end(ap);
    strcat(buf, "\n");
    sceKernelDebugOutText(0, buf);
    fputs(buf, stdout);
    fflush(stdout);
}

/* ⚠ EVERY CALL IS CHECKED AND NAMED. A Vulkan example that ignores results is a Vulkan example that
 * reports a black screen and nothing else - and a black screen is what this console shows for a
 * dozen different reasons. The step that failed is the whole value of running this. */
static void must(VkResult r, const char *what)
{
    if (r != VK_SUCCESS) {
        say("triangle: FAILED at %s -> VkResult %d", what, (int)r);
        say("triangle: idling so the log can be read; close from the PS button menu");
        for (;;)
            sleep(1);
    }
    say("triangle: ok   %s", what);
}

int main(void)
{
    say("orbis-sdk bundle: triangle");

    /* ---------------------------------------------------------------- instance */
    const char *inst_ext[] = { "VK_KHR_surface", "VK_EXT_headless_surface" };
    VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "orbis-triangle",
        .apiVersion = VK_API_VERSION_1_1,
    };
    VkInstanceCreateInfo ici = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app,
        .enabledExtensionCount = 2,
        .ppEnabledExtensionNames = inst_ext,
    };
    VkInstance instance;
    must(vkCreateInstance(&ici, NULL, &instance), "vkCreateInstance");

    /* ---------------------------------------------------------------- the display */
    VkHeadlessSurfaceCreateInfoEXT hsci = { .sType = VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT };
    VkSurfaceKHR surface;
    must(vkCreateHeadlessSurfaceEXT(instance, &hsci, NULL, &surface), "vkCreateHeadlessSurfaceEXT");

    /* ---------------------------------------------------------------- device */
    uint32_t ndev = 0;
    must(vkEnumeratePhysicalDevices(instance, &ndev, NULL), "vkEnumeratePhysicalDevices(count)");
    if (ndev == 0) {
        say("triangle: FAILED - no physical device. RADV did not enumerate this GPU.");
        for (;;) sleep(1);
    }
    VkPhysicalDevice phys[4];
    if (ndev > 4) ndev = 4;
    must(vkEnumeratePhysicalDevices(instance, &ndev, phys), "vkEnumeratePhysicalDevices");

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(phys[0], &props);
    say("triangle: GPU '%s', api %u.%u.%u, driver 0x%08x",
        props.deviceName,
        VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
        VK_VERSION_PATCH(props.apiVersion), props.driverVersion);

    /* The one queue family that can both render and present. On this driver they are the same
     * family, but asking is two lines and assuming is a bug on the next GPU. */
    uint32_t nqf = 0, qf = UINT32_MAX;
    vkGetPhysicalDeviceQueueFamilyProperties(phys[0], &nqf, NULL);
    VkQueueFamilyProperties *qfp = calloc(nqf, sizeof(*qfp));
    vkGetPhysicalDeviceQueueFamilyProperties(phys[0], &nqf, qfp);
    for (uint32_t i = 0; i < nqf; i++) {
        VkBool32 present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(phys[0], i, surface, &present);
        if ((qfp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present) { qf = i; break; }
    }
    free(qfp);
    if (qf == UINT32_MAX) {
        say("triangle: FAILED - no queue family that both renders and presents");
        for (;;) sleep(1);
    }
    say("triangle: queue family %u renders and presents", qf);

    const float prio = 1.0f;
    const char *dev_ext[] = { "VK_KHR_swapchain" };
    VkDeviceQueueCreateInfo qci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = qf, .queueCount = 1, .pQueuePriorities = &prio,
    };
    VkDeviceCreateInfo dci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1, .pQueueCreateInfos = &qci,
        .enabledExtensionCount = 1, .ppEnabledExtensionNames = dev_ext,
    };
    VkDevice device;
    must(vkCreateDevice(phys[0], &dci, NULL, &device), "vkCreateDevice");

    VkQueue queue;
    vkGetDeviceQueue(device, qf, 0, &queue);

    /* ---------------------------------------------------------------- swapchain */
    VkSurfaceCapabilitiesKHR caps;
    must(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys[0], surface, &caps),
         "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

    uint32_t nfmt = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys[0], surface, &nfmt, NULL);
    VkSurfaceFormatKHR *fmts = calloc(nfmt, sizeof(*fmts));
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys[0], surface, &nfmt, fmts);
    const VkSurfaceFormatKHR fmt = fmts[0];
    free(fmts);

    VkExtent2D extent = caps.currentExtent;
    if (extent.width == UINT32_MAX) { extent.width = 1920; extent.height = 1080; }
    say("triangle: surface %ux%u, format %d, %u..%u images",
        extent.width, extent.height, (int)fmt.format, caps.minImageCount, caps.maxImageCount);

    VkSwapchainCreateInfoKHR sci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = caps.minImageCount < 2 ? 2 : caps.minImageCount,
        .imageFormat = fmt.format, .imageColorSpace = fmt.colorSpace,
        .imageExtent = extent, .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = caps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,   /* always supported, and vsync is what a TV wants */
        .clipped = VK_TRUE,
    };
    VkSwapchainKHR swapchain;
    must(vkCreateSwapchainKHR(device, &sci, NULL, &swapchain), "vkCreateSwapchainKHR");

    uint32_t nimg = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &nimg, NULL);
    VkImage *images = calloc(nimg, sizeof(*images));
    vkGetSwapchainImagesKHR(device, swapchain, &nimg, images);
    say("triangle: swapchain has %u images", nimg);

    /* ---------------------------------------------------------------- render pass */
    VkAttachmentDescription att = {
        .format = fmt.format, .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    VkAttachmentReference ref = { .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
    VkSubpassDescription sub = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1, .pColorAttachments = &ref,
    };
    VkRenderPassCreateInfo rpci = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1, .pAttachments = &att,
        .subpassCount = 1, .pSubpasses = &sub,
    };
    VkRenderPass rp;
    must(vkCreateRenderPass(device, &rpci, NULL, &rp), "vkCreateRenderPass");

    /* ---------------------------------------------------------------- views, framebuffers */
    VkImageView   *views = calloc(nimg, sizeof(*views));
    VkFramebuffer *fbs   = calloc(nimg, sizeof(*fbs));
    for (uint32_t i = 0; i < nimg; i++) {
        VkImageViewCreateInfo ivci = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i], .viewType = VK_IMAGE_VIEW_TYPE_2D, .format = fmt.format,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };
        must(vkCreateImageView(device, &ivci, NULL, &views[i]), "vkCreateImageView");
        VkFramebufferCreateInfo fci = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = rp, .attachmentCount = 1, .pAttachments = &views[i],
            .width = extent.width, .height = extent.height, .layers = 1,
        };
        must(vkCreateFramebuffer(device, &fci, NULL, &fbs[i]), "vkCreateFramebuffer");
    }

    /* ---------------------------------------------------------------- pipeline */
    VkShaderModuleCreateInfo vsci = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = sizeof(tri_vert_spv), .pCode = tri_vert_spv,
    };
    VkShaderModuleCreateInfo fsci = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = sizeof(tri_frag_spv), .pCode = tri_frag_spv,
    };
    VkShaderModule vs, fs;
    must(vkCreateShaderModule(device, &vsci, NULL, &vs), "vkCreateShaderModule(vert)");
    must(vkCreateShaderModule(device, &fsci, NULL, &fs), "vkCreateShaderModule(frag)");

    VkPipelineShaderStageCreateInfo stages[2] = {
        { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
          .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = vs, .pName = "main" },
        { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
          .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = fs, .pName = "main" },
    };
    VkPipelineVertexInputStateCreateInfo vi = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    VkPipelineInputAssemblyStateCreateInfo ia = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };
    VkViewport vp = { 0, 0, (float)extent.width, (float)extent.height, 0.0f, 1.0f };
    VkRect2D   sc = { { 0, 0 }, extent };
    VkPipelineViewportStateCreateInfo vps = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1, .pViewports = &vp, .scissorCount = 1, .pScissors = &sc,
    };
    VkPipelineRasterizationStateCreateInfo rs = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL, .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE, .lineWidth = 1.0f,
    };
    VkPipelineMultisampleStateCreateInfo ms = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };
    VkPipelineColorBlendAttachmentState cba = { .colorWriteMask = 0xf };
    VkPipelineColorBlendStateCreateInfo cb = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1, .pAttachments = &cba,
    };
    VkPipelineLayoutCreateInfo plci = { .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    VkPipelineLayout layout;
    must(vkCreatePipelineLayout(device, &plci, NULL, &layout), "vkCreatePipelineLayout");

    VkGraphicsPipelineCreateInfo gpci = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2, .pStages = stages,
        .pVertexInputState = &vi, .pInputAssemblyState = &ia, .pViewportState = &vps,
        .pRasterizationState = &rs, .pMultisampleState = &ms, .pColorBlendState = &cb,
        .layout = layout, .renderPass = rp, .subpass = 0,
    };
    VkPipeline pipeline;
    /* ⚠ THE FIRST PIPELINE COMPILE IS THE ONE THAT NEEDS A STACK. README section 2.8: a dEQP worker died
     * inside radv_graphics_shaders_compile with about 72 KB of frame under it, and every thread that
     * compiles a pipeline on this console has the same cliff. This happens on the main thread here,
     * which has 2 MB, so it is not a test of the interposer - it is the call that would have found it. */
    must(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &gpci, NULL, &pipeline),
         "vkCreateGraphicsPipelines");

    /* ---------------------------------------------------------------- commands */
    VkCommandPoolCreateInfo cpci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .queueFamilyIndex = qf,
    };
    VkCommandPool pool;
    must(vkCreateCommandPool(device, &cpci, NULL, &pool), "vkCreateCommandPool");

    VkCommandBuffer *cmds = calloc(nimg, sizeof(*cmds));
    VkCommandBufferAllocateInfo cbai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool, .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, .commandBufferCount = nimg,
    };
    must(vkAllocateCommandBuffers(device, &cbai, cmds), "vkAllocateCommandBuffers");

    for (uint32_t i = 0; i < nimg; i++) {
        VkCommandBufferBeginInfo bi = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(cmds[i], &bi);
        VkClearValue clear = { .color = { .float32 = { 0.05f, 0.05f, 0.12f, 1.0f } } };
        VkRenderPassBeginInfo rpbi = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = rp, .framebuffer = fbs[i],
            .renderArea = { { 0, 0 }, extent },
            .clearValueCount = 1, .pClearValues = &clear,
        };
        vkCmdBeginRenderPass(cmds[i], &rpbi, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmds[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(cmds[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(cmds[i]);
        vkEndCommandBuffer(cmds[i]);
    }

    VkSemaphoreCreateInfo semci = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkSemaphore acquired, rendered;
    must(vkCreateSemaphore(device, &semci, NULL, &acquired), "vkCreateSemaphore(acquired)");
    must(vkCreateSemaphore(device, &semci, NULL, &rendered), "vkCreateSemaphore(rendered)");

    /* ---------------------------------------------------------------- present, forever */
    say("triangle: setup complete - presenting");
    unsigned long frame = 0;
    for (;;) {
        uint32_t idx = 0;
        VkResult r = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, acquired, VK_NULL_HANDLE, &idx);
        if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR) {
            say("triangle: vkAcquireNextImageKHR -> %d at frame %lu", (int)r, frame);
            break;
        }
        VkPipelineStageFlags wait = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo si = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1, .pWaitSemaphores = &acquired, .pWaitDstStageMask = &wait,
            .commandBufferCount = 1, .pCommandBuffers = &cmds[idx],
            .signalSemaphoreCount = 1, .pSignalSemaphores = &rendered,
        };
        r = vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE);
        if (r != VK_SUCCESS) { say("triangle: vkQueueSubmit -> %d at frame %lu", (int)r, frame); break; }

        VkPresentInfoKHR pi = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1, .pWaitSemaphores = &rendered,
            .swapchainCount = 1, .pSwapchains = &swapchain, .pImageIndices = &idx,
        };
        r = vkQueuePresentKHR(queue, &pi);
        if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR) {
            say("triangle: vkQueuePresentKHR -> %d at frame %lu", (int)r, frame);
            break;
        }
        vkQueueWaitIdle(queue);

        /* Powers of two, for the same reason README section 2.8's census uses them: a line per frame
         * would put this port's own log channel in the state it warns about. */
        if ((frame & (frame - 1)) == 0)
            say("triangle: frame %lu presented", frame);
        frame++;
    }

    say("triangle: stopped after %lu frame(s) - idling; close from the PS button menu", frame);
    for (;;)
        sleep(1);
}
