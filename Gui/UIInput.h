#ifndef UI_INPUT_H
#define UI_INPUT_H

#include "../../Core/Device/VKWindow.h"
#include "UIConfig.h"

namespace Gui {
    class UIInput: protected virtual Core::VKWindow {
        private:
            Log::Record* m_UIInputLog;
            const uint32_t m_instanceId = g_collectionSettings.instanceId++;

        public:
            UIInput (void) {
                m_UIInputLog = LOG_INIT (m_instanceId, g_collectionSettings.logSaveDirPath);
            }

            ~UIInput (void) {
                LOG_CLOSE (m_instanceId);
            }

        protected:
            void readyKeyCallBack               (uint32_t deviceInfoId) {
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                glfwSetKeyCallback              (deviceInfo->resource.window, keyCallBack);
            }

            void deleteKeyCallBack              (uint32_t deviceInfoId) {
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                glfwSetKeyCallback              (deviceInfo->resource.window, NULL);
            }

            void readyCursorPositionCallBack    (uint32_t deviceInfoId) {
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                glfwSetCursorPosCallback        (deviceInfo->resource.window, cursorPositionCallBack);
                glfwSetInputMode                (deviceInfo->resource.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            void deleteCursorPositionCallBack   (uint32_t deviceInfoId) {
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                glfwSetCursorPosCallback        (deviceInfo->resource.window, NULL);
                glfwSetInputMode                (deviceInfo->resource.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            void readyScrollOffsetCallBack      (uint32_t deviceInfoId) {
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                glfwSetScrollCallback           (deviceInfo->resource.window, scrollOffsetCallBack);
                glfwSetInputMode                (deviceInfo->resource.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            void deleteScrollOffsetCallBack     (uint32_t deviceInfoId) {
                auto deviceInfo = getDeviceInfo (deviceInfoId);
                glfwSetScrollCallback           (deviceInfo->resource.window, NULL);
                glfwSetInputMode                (deviceInfo->resource.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            void createKeyEventBinding (int key, const std::function <void (float)>& binding) {
                getKeyEventInfoPool()[key].meta.binding    = binding;
            }

            void createMouseEventBinding (Core::e_mouseEventType type,
                                          const std::function <void (double, double)>& binding) {
                getMouseEventInfoPool()[type].meta.binding = binding;
            }

            void deleteKeyEventInfo (int key) {
                if (getKeyEventInfoPool().find  (key) != getKeyEventInfoPool().end())
                    getKeyEventInfoPool().erase (key);
            }

            void deleteMouseEventInfo (Core::e_mouseEventType type) {
                if (getMouseEventInfoPool().find  (type) != getMouseEventInfoPool().end())
                    getMouseEventInfoPool().erase (type);
            }

            void handleKeyEvents (std::chrono::steady_clock::time_point currentTime) {
                for (auto const& [key, info]: getKeyEventInfoPool()) {
                    if (info.meta.isPressed) {
                        float deltaTime = std::chrono::duration <float, std::chrono::seconds::period>
                                          (currentTime - info.meta.captureTime).count();
                        info.meta.binding (deltaTime);
                    }
                }
            }

            void cleanUp (uint32_t deviceInfoId) {
                deleteKeyCallBack            (deviceInfoId);
                deleteCursorPositionCallBack (deviceInfoId);
                deleteScrollOffsetCallBack   (deviceInfoId);
            }
    };
}   // namespace Gui
#endif  // UI_INPUT_H