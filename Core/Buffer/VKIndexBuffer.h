#ifndef VK_INDEX_BUFFER_H
#define VK_INDEX_BUFFER_H

#include "VKBufferMgr.h"

using namespace Collections;

namespace Core {
    class VKIndexBuffer: protected virtual VKBufferMgr {
        private:
            static Log::Record* m_VKIndexBufferLog;
            const uint32_t m_instanceId = g_collectionsId++; 

        public:
            VKIndexBuffer (void) {
                m_VKIndexBufferLog = LOG_INIT (m_instanceId, g_pathSettings.logSaveDir);
            }

            ~VKIndexBuffer (void) {
                LOG_CLOSE (m_instanceId);
            }
        
        protected:
            /* An index buffer is essentially an array of pointers into the vertex buffer. It allows you to reorder the 
             * vertex data, reuse existing data for multiple vertices and thus saving memory when loading complex models
            */
            void createIndexBuffer (uint32_t bufferInfoId,
                                    uint32_t deviceInfoId,
                                    VkDeviceSize size, 
                                    const void* data) {
                                        
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                auto stagingBufferShareQueueFamilyIndices = std::vector {
                    deviceInfo->meta.transferFamilyIndex.value()
                };
                createBuffer (bufferInfoId, 
                              deviceInfoId,
                              STAGING_BUFFER,
                              size,
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                              stagingBufferShareQueueFamilyIndices);

                auto bufferInfo = getBufferInfo (bufferInfoId, STAGING_BUFFER);
                vkMapMemory (deviceInfo->resource.logDevice, 
                             bufferInfo->resource.bufferMemory, 
                             0, 
                             size, 
                             0, 
                             &bufferInfo->meta.bufferMapped);
                memcpy (bufferInfo->meta.bufferMapped, data, static_cast <size_t> (size));
                vkUnmapMemory (deviceInfo->resource.logDevice, bufferInfo->resource.bufferMemory);

                auto bufferShareQueueFamilyIndices = std::vector {
                    deviceInfo->meta.graphicsFamilyIndex.value(), 
                    deviceInfo->meta.transferFamilyIndex.value()
                };
                createBuffer (bufferInfoId, 
                              deviceInfoId,
                              INDEX_BUFFER,
                              size, 
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
                              bufferShareQueueFamilyIndices);
            }
    };

    Log::Record* VKIndexBuffer::m_VKIndexBufferLog;
}   // namespace Core
#endif  // VK_INDEX_BUFFER_H