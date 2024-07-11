#ifndef VK_FRAME_BUFFER_H
#define VK_FRAME_BUFFER_H

#include "VKRenderPassMgr.h"

using namespace Collections;

namespace Renderer {
    class VKFrameBuffer: protected virtual VKRenderPassMgr {
        private:
            static Log::Record* m_VKFrameBufferLog;
            const size_t m_instanceId = g_collectionsId++;
            
        public:
            VKFrameBuffer (void) {
                m_VKFrameBufferLog = LOG_INIT (m_instanceId, g_pathSettings.logSaveDir); 
                LOG_ADD_CONFIG (m_instanceId, Log::ERROR, Log::TO_FILE_IMMEDIATE | Log::TO_CONSOLE); 
            }

            ~VKFrameBuffer (void) { 
                LOG_CLOSE (m_instanceId);
            }

        protected:
            /* Render passes operate in conjunction with framebuffers. Framebuffers represent a collection of specific 
             * memory attachments that a render pass instance uses. In other words, a frame buffer binds a VkImageView 
             * with an attachment, and the frame buffer together with the render pass defines the render target 
            */
            void createFrameBuffer (uint32_t renderPassInfoId, 
                                    uint32_t resourceId,
                                    const std::vector <VkImageView>& attachments) {

                auto renderPassInfo = getRenderPassInfo (renderPassInfoId);
                auto deviceInfo     = getDeviceInfo();

                VkFramebufferCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                /* Specify with which renderPass the framebuffer needs to be compatible. You can only use a framebuffer 
                 * with the render passes that it is compatible with, which roughly means that they use the same number 
                 * and type of attachments
                */
                createInfo.renderPass = renderPassInfo->resource.renderPass;
                /* The attachmentCount and pAttachments parameters specify the VkImageView objects that should be bound to
                 * the respective attachment descriptions in the render pass pAttachment array
                */
                createInfo.attachmentCount = static_cast <uint32_t> (attachments.size());
                createInfo.pAttachments    = attachments.data();
                createInfo.width           = deviceInfo->unique[resourceId].swapChain.extent.width;
                createInfo.height          = deviceInfo->unique[resourceId].swapChain.extent.height;
                createInfo.layers          = 1;

                VkFramebuffer frameBuffer;
                VkResult result = vkCreateFramebuffer (deviceInfo->shared.logDevice, 
                                                       &createInfo, 
                                                       nullptr, 
                                                       &frameBuffer);
                if (result != VK_SUCCESS) {
                    LOG_ERROR (m_VKFrameBufferLog) << "Failed to create framebuffer " 
                                                   << "[" << renderPassInfoId << "]"
                                                   << " "
                                                   << "[" << resourceId << "]"
                                                   << " "
                                                   << "[" << string_VkResult (result) << "]"
                                                   << std::endl;
                    throw std::runtime_error ("Failed to create framebuffer");
                }

                renderPassInfo->resource.framebuffers.push_back (frameBuffer);
            }

            void cleanUp (uint32_t renderPassInfoId) {
                auto renderPassInfo = getRenderPassInfo (renderPassInfoId);
                auto deviceInfo     = getDeviceInfo();
                /* Destroy the framebuffers before the image views and render pass that they are based on
                */
                for (auto const& framebuffer: renderPassInfo->resource.framebuffers)
                    vkDestroyFramebuffer (deviceInfo->shared.logDevice, framebuffer, nullptr);                
            }
    };

    Log::Record* VKFrameBuffer::m_VKFrameBufferLog;
}   // namespace Renderer
#endif  // VK_FRAME_BUFFER_H