/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanTexture.h"

#include "ServiceBroker.h"
#include "guilib/TextureFormats.h"
#include "guilib/TextureManager.h"
#include "rendering/RenderSystem.h"
#include "rendering/vulkan/VulkanRenderSystem.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/Map.h"
#include "utils/MemUtils.h"
#include "utils/log.h"

#include <memory>

std::unique_ptr<CTexture> CTexture::CreateTexture(unsigned int width,
                                                  unsigned int height,
                                                  XB_FMT format)
{
  using namespace KODI::GUILIB::GRAPHICS::VULKAN;
  return std::make_unique<CVulkanTexture>(width, height, format);
}

namespace
{

// clang-format off
constexpr auto TextureMapping = make_map<KD_TEX_FMT, VkFormat>({
    // SDR texture family
    {KD_TEX_FMT_SDR_R8, VK_FORMAT_R8_UNORM},
    {KD_TEX_FMT_SDR_RG8, VK_FORMAT_R8G8_UNORM},
    {KD_TEX_FMT_SDR_RGB8, VK_FORMAT_R8G8B8_UNORM},
    {KD_TEX_FMT_SDR_RGBA8, VK_FORMAT_R8G8B8A8_UNORM},
    {KD_TEX_FMT_SDR_BGRA8, VK_FORMAT_B8G8R8A8_UNORM},
    {KD_TEX_FMT_SDR_RGBA4, VK_FORMAT_R4G4B4A4_UNORM_PACK16},
    {KD_TEX_FMT_SDR_RGB5_A1, VK_FORMAT_A1R5G5B5_UNORM_PACK16},
    {KD_TEX_FMT_SDR_R5G6B5, VK_FORMAT_R5G6B5_UNORM_PACK16},
    // HDR texture family
    {KD_TEX_FMT_HDR_R16f, VK_FORMAT_R16_SFLOAT},
    {KD_TEX_FMT_HDR_RG16f, VK_FORMAT_R16G16_SFLOAT},
    {KD_TEX_FMT_HDR_R11F_G11F_B10F, VK_FORMAT_B10G11R11_UFLOAT_PACK32},
    {KD_TEX_FMT_HDR_RGB9_E5, VK_FORMAT_E5B9G9R9_UFLOAT_PACK32},
    {KD_TEX_FMT_HDR_RGB10_A2, VK_FORMAT_A2B10G10R10_UNORM_PACK32},
    {KD_TEX_FMT_HDR_RGBA16f, VK_FORMAT_R16G16B16A16_SFLOAT},
    // YUV texture family
    {KD_TEX_FMT_YUV_YUYV8, VK_FORMAT_G8B8G8R8_422_UNORM},
    // S3TC texture family
    {KD_TEX_FMT_S3TC_RGB8, VK_FORMAT_BC1_RGB_UNORM_BLOCK},
    {KD_TEX_FMT_S3TC_RGB8_A1, VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
    {KD_TEX_FMT_S3TC_RGB8_A4, VK_FORMAT_BC2_UNORM_BLOCK},
    {KD_TEX_FMT_S3TC_RGBA8, VK_FORMAT_BC3_UNORM_BLOCK},
    // RGTC (LATC) texture family
    {KD_TEX_FMT_RGTC_R11, VK_FORMAT_BC4_UNORM_BLOCK},
    {KD_TEX_FMT_RGTC_RG11, VK_FORMAT_BC5_UNORM_BLOCK},
    // BPTC texture family
    {KD_TEX_FMT_BPTC_RGB16F, VK_FORMAT_BC6H_UFLOAT_BLOCK},
    {KD_TEX_FMT_BPTC_RGBA8, VK_FORMAT_BC7_UNORM_BLOCK},
    // ETC1 texture family
    {KD_TEX_FMT_ETC1_RGB8, VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK},
    // ETC2 texture family
    {KD_TEX_FMT_ETC2_R11, VK_FORMAT_EAC_R11_UNORM_BLOCK},
    {KD_TEX_FMT_ETC2_RG11, VK_FORMAT_EAC_R11G11_UNORM_BLOCK},
    {KD_TEX_FMT_ETC2_RGB8, VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK},
    {KD_TEX_FMT_ETC2_RGB8_A1, VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK},
    {KD_TEX_FMT_ETC2_RGBA8, VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK},
    // ASTC LDR texture family
    // Bitrate varies from 8bpp (4x4 tile) to 0.89bpp (12x12 tile).
    {KD_TEX_FMT_ASTC_LDR_4x4, VK_FORMAT_ASTC_4x4_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_5x4, VK_FORMAT_ASTC_5x4_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_5x5, VK_FORMAT_ASTC_5x5_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_6x5, VK_FORMAT_ASTC_6x5_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_6x6, VK_FORMAT_ASTC_6x6_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_8x5, VK_FORMAT_ASTC_8x5_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_8x6, VK_FORMAT_ASTC_8x6_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_8x8, VK_FORMAT_ASTC_8x8_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_10x5, VK_FORMAT_ASTC_10x5_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_10x6, VK_FORMAT_ASTC_10x6_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_10x8, VK_FORMAT_ASTC_10x8_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_10x10, VK_FORMAT_ASTC_10x10_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_12x10, VK_FORMAT_ASTC_12x10_UNORM_BLOCK},
    {KD_TEX_FMT_ASTC_LDR_12x12, VK_FORMAT_ASTC_12x12_UNORM_BLOCK},
    // ASTC HDR texture family
    // Bitrate varies from 8bpp (4x4 tile) to 0.89bpp (12x12 tile).
    {KD_TEX_FMT_ASTC_HDR_4x4, VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_5x4, VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_5x5, VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_6x5, VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_6x6, VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_8x5, VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_8x6, VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_8x8, VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_10x5, VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_10x6, VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_10x8, VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_10x10, VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_12x10, VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK},
    {KD_TEX_FMT_ASTC_HDR_12x12, VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK},
});

constexpr auto SwizzleMap = make_map<KD_TEX_SWIZ, VkComponentMapping>({
    {KD_TEX_SWIZ_RGBA, {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A}},
    {KD_TEX_SWIZ_RGB1, {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_ONE}},
    {KD_TEX_SWIZ_RRR1, {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_ONE}},
    {KD_TEX_SWIZ_111R, {VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_R}},
    {KD_TEX_SWIZ_RRRG, {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G}},
    {KD_TEX_SWIZ_RRRR, {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R}},
    {KD_TEX_SWIZ_GGG1, {VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_ONE}},
    {KD_TEX_SWIZ_111G, {VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_ONE, VK_COMPONENT_SWIZZLE_G}},
    {KD_TEX_SWIZ_GGGA, {VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_A}},
    {KD_TEX_SWIZ_GGGG, {VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_G}},
});
// clang-format on

} // namespace

using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::GUILIB::GRAPHICS::VULKAN
{

CVulkanTexture::CVulkanTexture(unsigned int width, unsigned int height, XB_FMT format)
  : CTexture(width, height, format)
{
  using namespace KODI::RENDERING::VULKAN;
  m_renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  m_vkDevice = m_renderSystem->vkDevice();
  m_vkPhysicalDevice = m_renderSystem->vkPhysicalDevice();

  fprintf(stderr, "CTexture::CreateTexture: Creating Vulkan texture %ux%u format %d\n", width,
          height, format);
}

CVulkanTexture::~CVulkanTexture()
{
  DestroyTextureObject();
}

void CVulkanTexture::CreateTextureObject()
{
  fprintf(stderr, "----------------------------> %s\n", __func__);
}

void CVulkanTexture::DestroyTextureObject()
{
  fprintf(stderr, "----------------------------> %s\n", __func__);
  if (m_imageView != VK_NULL_HANDLE)
  {
    vkDestroyImageView(m_vkDevice, m_imageView, nullptr);
    m_imageView = VK_NULL_HANDLE;
  }
  if (m_image != VK_NULL_HANDLE)
  {
    vkDestroyImage(m_vkDevice, m_image, nullptr);
    m_image = VK_NULL_HANDLE;
  }
  if (m_sampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(m_vkDevice, m_sampler, nullptr);
    m_sampler = VK_NULL_HANDLE;
  }
  if (m_imageMemory != VK_NULL_HANDLE)
  {
    vkFreeMemory(m_vkDevice, m_imageMemory, nullptr);
    m_imageMemory = VK_NULL_HANDLE;
  }
}
/*





*/
VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool command_pool)
{
  VkCommandBufferAllocateInfo allocInfo =
      vkCommandBufferAllocateInfo(command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device, &allocInfo, &command_buffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer, &beginInfo);

  return command_buffer;
}

void EndSingleTimeCommands(VkDevice device,
                           VkCommandPool command_pool,
                           VkQueue queue,
                           VkCommandBuffer command_buffer)
{
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submitInfo = vkSubmitInfo();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffer;

  vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}
/*





*/
void CVulkanTexture::LoadToGPU()
{
  using namespace KODI::RENDERING::VULKAN::UTILS;

  const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

  if (!m_pixels)
  {
    // nothing to load - probably same image (no change)
    return;
  }

  size_t size = GetPitch() * GetRows() * sizeof(float);
  if (size == 0)
    return;

  VkBuffer buffer;

  /**
   * @brief Create a staging buffer to copy the pixel data to the GPU.
   */
  ///@{
  VkBufferCreateInfo bufferInfo = vkBufferCreateInfo();
  bufferInfo.size = static_cast<VkDeviceSize>(size);
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VK_CHECK_RESULT(vkCreateBuffer(m_vkDevice, &bufferInfo, nullptr, &buffer));

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_vkDevice, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = vkMemoryAllocateInfo();

  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      FindMemoryType(m_vkPhysicalDevice, memRequirements.memoryTypeBits,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkDeviceMemory stagingBufferMemory;
  VK_CHECK_RESULT(vkAllocateMemory(m_vkDevice, &allocInfo, nullptr, &stagingBufferMemory));
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkDevice, buffer, stagingBufferMemory, 0));

  void* data;
  vkMapMemory(m_vkDevice, stagingBufferMemory, 0, static_cast<VkDeviceSize>(size), 0, &data);
  memcpy(data, m_pixels, size);
  vkUnmapMemory(m_vkDevice, stagingBufferMemory);

  VkImageCreateInfo imageInfo = vkImageCreateInfo();
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = format;
  imageInfo.extent.width = static_cast<uint32_t>(GetWidth());
  imageInfo.extent.height = static_cast<uint32_t>(GetHeight());
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VK_CHECK_RESULT(vkCreateImage(m_vkDevice, &imageInfo, nullptr, &m_image));

  vkGetImageMemoryRequirements(m_vkDevice, m_image, &memRequirements);

  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = FindMemoryType(m_vkPhysicalDevice, memRequirements.memoryTypeBits,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkCommandPool command_pool = m_renderSystem->vkCurrentCommandPool();
  VkQueue queue = m_renderSystem->vkQueue();

  VK_CHECK_RESULT(vkAllocateMemory(m_vkDevice, &allocInfo, nullptr, &m_imageMemory));
  VK_CHECK_RESULT(vkBindImageMemory(m_vkDevice, m_image, m_imageMemory, 0));

  VkImageSubresourceRange subresource_range{
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };

  VkImageMemoryBarrier barrier = vkImageMemoryBarrier();
  barrier.image = m_image;
  barrier.subresourceRange = subresource_range;

  //@{
  barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

  VkCommandBuffer command_buffer = BeginSingleTimeCommands(m_vkDevice, command_pool);

  vkCmdPipelineBarrier(command_buffer, 0 /* TODO */, 0 /* TODO */, 0, 0, nullptr, 0, nullptr, 1,
                       &barrier);
  //@}

  //@{
  VkBufferImageCopy region = {};
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.layerCount = 1;
  region.imageExtent = {GetPitch(), GetRows(), 1};

  vkCmdCopyBufferToImage(command_buffer, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &region);
  //@}

  //@{
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  command_pool = m_renderSystem->vkCurrentCommandPool();
  vkCmdPipelineBarrier(command_buffer, 0 /* TODO */, 0 /* TODO */, 0, 0, nullptr, 0, nullptr, 1,
                       &barrier);
  EndSingleTimeCommands(m_vkDevice, command_pool, queue, command_buffer);
  //@}

  vkDestroyBuffer(m_vkDevice, buffer, nullptr);

  vkFreeMemory(m_vkDevice, stagingBufferMemory, nullptr);
  ///@}

  /**
   * @brief Create image view
   *
   * Create a sampler for the texture. The sampler defines how the texture
   * is sampled in the shader.
   *
   * Textures are not directly accessed by the shaders and are abstracted by image
   * views containing additional information and sub resource ranges
   */
  ///@{
  VkImageViewCreateInfo view = vkImageViewCreateInfo();
  view.image = m_image;
  view.viewType = VK_IMAGE_VIEW_TYPE_2D;
  if (TextureMapping.contains(m_textureFormat))
    view.format = TextureMapping.at(m_textureFormat);
  else
    view.format = VK_FORMAT_R8G8B8A8_UNORM;
  if (SwizzleMap.contains(m_textureSwizzle))
    view.components = SwizzleMap.at(m_textureSwizzle);
  // The subresource range describes the set of mip levels (and array layers) that can be accessed through this image view
  // It's possible to create multiple image views for a single image referring to different (and/or overlapping) ranges of the image
  view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view.subresourceRange.baseMipLevel = 0;
  view.subresourceRange.layerCount = 1;
  view.subresourceRange.baseArrayLayer = 0;
  view.subresourceRange.levelCount = 1;
  VK_CHECK_RESULT(vkCreateImageView(m_vkDevice, &view, nullptr, &m_imageView));
  ///@}

  /**
   * @brief Create sampler
   */
  ///@{
  VkSamplerCreateInfo samplerInfo = vkSamplerCreateInfo();
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_FALSE;
  VK_CHECK_RESULT(vkCreateSampler(m_vkDevice, &samplerInfo, nullptr, &m_sampler));
  ///@}

  if (!m_bCacheMemory)
  {
    KODI::MEMORY::AlignedFree(m_pixels);
    m_pixels = nullptr;
  }

  m_loadedToGPU = true;
}

void CVulkanTexture::SyncGPU()
{
  fprintf(stderr, "----------------------------> %s\n", __func__);
}

void CVulkanTexture::BindToUnit(unsigned int unit)
{
  fprintf(stderr, "----------------------------> %s\n", __func__);
}

bool CVulkanTexture::SupportsFormat(KD_TEX_FMT textureFormat, KD_TEX_SWIZ textureSwizzle)
{
  return true;
}

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
