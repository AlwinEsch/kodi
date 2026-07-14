/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanRenderPass.h"

#include "rendering/vulkan/VulkanUtils.h"
#include "utils/log.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

CVulkanRenderPass::CVulkanRenderPass(VkDevice logicalDevice, VkRenderPass renderPass)
  : m_renderPass(renderPass),
    m_logicalDevice(logicalDevice)
{
}

CVulkanRenderPass::~CVulkanRenderPass()
{
  if (m_renderPass != VK_NULL_HANDLE)
    vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);
}

std::unique_ptr<CVulkanRenderPass> CVulkanRenderPass::Create(VkFormat format,
                                                             VkDevice logicalDevice)
{
  VkAttachmentDescription attachment{
      .flags = 0,
      .format = format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  // We have one subpass. This subpass has one color attachment.
  // While executing this subpass, the attachment will be in attachment optimal layout.
  VkAttachmentReference color_ref = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  // We will end up with two transitions.
  // The first one happens right before we start subpass #0, where
  // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
  // The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
  // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
  VkSubpassDescription subpass{
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_ref,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };

  // Create a dependency to external events.
  // We need to wait for the WSI semaphore to signal.
  // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
  // actually wait for the semaphore, so we must also wait for that pipeline stage.
  VkSubpassDependency dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dependencyFlags = 0,
  };

  // Finally, create the renderpass.
  VkRenderPassCreateInfo rp_info{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = 1,
      .pAttachments = &attachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
  };

  VkRenderPass renderPass;
  VkResult result = vkCreateRenderPass(logicalDevice, &rp_info, nullptr, &renderPass);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create render pass, ERROR: {0}", ErrorString(result));
    return nullptr;
  }

  return std::make_unique<CVulkanRenderPass>(logicalDevice, renderPass);
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
