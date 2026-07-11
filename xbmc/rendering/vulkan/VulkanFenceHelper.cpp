/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanFenceHelper.h"

#include <cassert>
#include <utility>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanFenceHelper::CVulkanFenceHelper(CVulkanDeviceQueue* deviceQueue) : m_deviceQueue(deviceQueue)
{
}

CVulkanFenceHelper::~CVulkanFenceHelper()
{
}

void CVulkanFenceHelper::Destroy()
{
  PerformImmediateCleanup();
}

VkResult CVulkanFenceHelper::GetFence(VkFence* fence)
{
  VkFenceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };
  return vkCreateFence(m_deviceQueue->GetVulkanDevice(), &createInfo, nullptr, fence);
}

void CVulkanFenceHelper::PerformImmediateCleanup()
{
  //if (cleanup_tasks_.empty() && tasks_pending_fence_.empty())
  //  return;

  //// We want to run all tasks immediately, so just use vkQueueWaitIdle which
  //// ensures that all fences have passed.
  //// Even if exclusively using callbacks, the callbacks use WeakPtr and will
  //// not keep this class alive, so it's important to wait / run all cleanup
  //// immediately.
  //VkResult result = vkQueueWaitIdle(m_deviceQueue->GetVulkanQueue());
  //// Wait can only fail for three reasons - device loss, host OOM, device OOM.
  //// If we hit an OOM, treat this as a crash. There isn't a great way to
  //// recover from this.
  //assert(result == VK_SUCCESS || result == VK_ERROR_DEVICE_LOST);
  //bool device_lost = result == VK_ERROR_DEVICE_LOST;

  //// We're going to destroy all fences below, so we should consider them as
  //// passed.
  //current_generation_ = next_generation_ - 1;

  //// Run all cleanup tasks. Create a temporary vector of tasks to run to avoid
  //// reentrancy issues.
  //std::vector<CleanupTask> tasks_to_run;
  //while (!cleanup_tasks_.empty())
  //{
  //  auto& tasks_for_fence = cleanup_tasks_.front();
  //  vkDestroyFence(m_deviceQueue->GetVulkanDevice(), tasks_for_fence.fence, nullptr);
  //  tasks_to_run.insert(tasks_to_run.end(), std::make_move_iterator(tasks_for_fence.tasks.begin()),
  //                      std::make_move_iterator(tasks_for_fence.tasks.end()));
  //  cleanup_tasks_.pop_front();
  //}
  //tasks_to_run.insert(tasks_to_run.end(), std::make_move_iterator(tasks_pending_fence_.begin()),
  //                    std::make_move_iterator(tasks_pending_fence_.end()));
  //tasks_pending_fence_.clear();
  //for (auto& task : tasks_to_run)
  //  std::move(task).Run(m_deviceQueue.get(), device_lost);
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
