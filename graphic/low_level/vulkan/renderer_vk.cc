#include "low_level/vulkan/renderer_vk.h"
#include "engine/engine.h"
#include "errors/error.h"

namespace ixr {
namespace engine {
namespace vk {
using namespace core;

VulkanRenderer::VulkanRenderer(engine::Env *e) : env_(e), vk_env_({}) {
  VkApplicationInfo app_info;
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pEngineName = "ixRengine";
  app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.pApplicationName = "ixRengine::App";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo ins_info;
  VkInstanceCreateFlags flag = 0;
  std::vector<const char *> vk_extension_names{
#ifdef _DEBUG
      VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
  };
  std::vector<const char *> vk_layer_names{
#ifdef _DEBUG
      "VK_LAYER_LUNARG_standard_validation",
#endif
  };
  ins_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  ins_info.pNext = nullptr;
  ins_info.pApplicationInfo = &app_info;
  ins_info.flags = flag;
  ins_info.enabledExtensionCount =
      static_cast<uint32_t>(vk_extension_names.size());
  ins_info.ppEnabledExtensionNames = vk_extension_names.data();
  ins_info.enabledLayerCount = static_cast<uint32_t>(vk_layer_names.size());
  ins_info.ppEnabledLayerNames = vk_layer_names.data();

  VkResult res = vkCreateInstance(&ins_info, nullptr, &vk_env_.ins);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fail to create vkInstance");
#ifdef _DEBUG
  createDebugCallback();
#endif
}

VulkanRenderer::~VulkanRenderer() {
#ifdef _DEBUG
  auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
      vk_env_.ins, "vkDestroyDebugReportCallbackEXT");
  if (func != nullptr) {
    func(vk_env_.ins, debug_report_fn_, nullptr);
  }
#endif
  vkDestroyDevice(vk_env_.dev, nullptr);
  vkDestroyInstance(vk_env_.ins, nullptr);
}

void VulkanRenderer::CreateDevice(uint32_t id) {
  uint32_t dev_count;
  VkPhysicalDevice dev[16];
  VkResult res = vkEnumeratePhysicalDevices(vk_env_.ins, &dev_count, dev);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fail to vkEnumeratePhysicalDevices");
  VkPhysicalDeviceProperties prop;
  VkPhysicalDeviceFeatures feature;
  vk_env_.phy_dev = dev[0];  // by default use the 1st GPU
  for (auto i = 0U; i < dev_count; i++) {
    vkGetPhysicalDeviceProperties(dev[i], &prop);
    vkGetPhysicalDeviceFeatures(dev[i], &feature);
    if (prop.vendorID == id) {
      vk_env_.phy_dev = dev[i];
      vid_ = AdapterVendorId(id);
      break;
    }
  }
  vkGetPhysicalDeviceProperties(vk_env_.phy_dev, &prop);
  vid_ = AdapterVendorId(prop.vendorID);

  uint32_t queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(vk_env_.phy_dev, &queue_family_count,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(vk_env_.phy_dev, &queue_family_count,
                                           queue_families.data());

  VkDeviceQueueCreateInfo queue_info;
  VkDeviceQueueCreateFlags qflag = 0;
  float queue_priority = 1.0f;
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.pNext = nullptr;
  queue_info.queueFamilyIndex = 0;
  for (auto &queue : queue_families) {
    if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT && queue.queueCount > 0) {
      break;
    }
    queue_info.queueFamilyIndex++;
  }
  queue_info.flags = qflag;
  queue_info.queueCount = 1;
  queue_info.pQueuePriorities = &queue_priority;

  VkDeviceCreateInfo device_info;
  VkDeviceCreateFlags dev_flag = 0;
  std::vector<const char *> vk_extension_names{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };
  std::vector<const char *> vk_layer_names{
#ifdef _DEBUG
      "VK_LAYER_LUNARG_standard_validation",
#endif
  };
  vkGetPhysicalDeviceFeatures(vk_env_.phy_dev, &feature);
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.pNext = nullptr;
  device_info.flags = dev_flag;
  device_info.pEnabledFeatures = &feature;
  device_info.pQueueCreateInfos = &queue_info;
  device_info.queueCreateInfoCount = 1;
  device_info.enabledLayerCount = static_cast<uint32_t>(vk_layer_names.size());
  device_info.ppEnabledLayerNames = vk_layer_names.data();
  device_info.enabledExtensionCount =
      static_cast<uint32_t>(vk_extension_names.size());
  device_info.ppEnabledExtensionNames = vk_extension_names.data();
  res = vkCreateDevice(vk_env_.phy_dev, &device_info, nullptr, &vk_env_.dev);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fail to vkCreateDevice");
  vkGetDeviceQueue(vk_env_.dev, queue_info.queueFamilyIndex, 0, vk_env_.queue);

  env_->ChangeEnv({&vk_env_, vid_, GRAPHIC_API_VULKAN});
}

void VulkanRenderer::SetInputLayout(const VertexElements &v,
                                    Shader *vertex_shader) {}

void VulkanRenderer::SetVertexBySlot(const SlotVertex &slots) {}

void VulkanRenderer::SetVertexAndIndex(GpuBuffer *vertice, uint32_t stride,
                                       GpuBuffer *indice) {}

void VulkanRenderer::SetShader(Shader *shader) {}

void VulkanRenderer::SetSampler() {}

void VulkanRenderer::SetRasterizer(uint32_t rasterizer_desc) {}

void VulkanRenderer::SetViewport(float w, float h, float x, float y, float min,
                                 float max) {}

void VulkanRenderer::ClearRenderTarget(Texture *tex, const float color[4]) {}

void VulkanRenderer::ClearDepthStencil(Texture *depth) {}

void VulkanRenderer::SetRenderTargets(const RenderTargets &rt, Texture *depth) {
}

void VulkanRenderer::SetShaderResources(Texture *tex) {}

void VulkanRenderer::SetBlendState(const float (&factor)[4], uint32_t mask) {}

void VulkanRenderer::SetModelViewPerspectiveProjection(float mvp[16]) {}

void VulkanRenderer::Draw(int32_t count, int32_t offset, int32_t base) {}

void VulkanRenderer::SetBarrier() {}

void VulkanRenderer::SyncBarrier() {}

std::vector<VkLayerProperties> VulkanRenderer::enumLayers() {
  uint32_t prop_count;
  VkResult res = vkEnumerateInstanceLayerProperties(&prop_count, nullptr);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fatal, Vulkan SDK installed?");
  std::vector<VkLayerProperties> properties(prop_count);
  res = vkEnumerateInstanceLayerProperties(&prop_count, properties.data());
  THROW_IF_FAIL(res != VK_SUCCESS, "Fatal, Vulkan SDK installed?");
  return properties;
}

std::vector<VkExtensionProperties> VulkanRenderer::enumInstanceExtensions() {
  uint32_t prop_count;
  VkResult res =
      vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, nullptr);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fatal, Vulkan SDK installed?");
  std::vector<VkExtensionProperties> properties(prop_count);
  res = vkEnumerateInstanceExtensionProperties(nullptr, &prop_count,
                                               properties.data());
  THROW_IF_FAIL(res != VK_SUCCESS, "Fatal, Vulkan SDK installed?");
  return properties;
}

std::vector<VkExtensionProperties> VulkanRenderer::enumDeviceExtensions() {
  uint32_t prop_count;
  VkResult res = vkEnumerateDeviceExtensionProperties(vk_env_.phy_dev, nullptr,
                                                      &prop_count, nullptr);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fatal, Vulkan SDK installed?");
  std::vector<VkExtensionProperties> properties(prop_count);
  res = vkEnumerateDeviceExtensionProperties(vk_env_.phy_dev, nullptr,
                                             &prop_count, properties.data());
  THROW_IF_FAIL(res != VK_SUCCESS, "Fatal, Vulkan SDK installed?");
  return properties;
}

#ifdef _DEBUG
VkResult VulkanRenderer::createDebugCallback() {
  VkDebugReportCallbackCreateInfoEXT debug_report_info = {};
  debug_report_info.sType =
      VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debug_report_info.flags =
      VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  debug_report_info.pfnCallback = DebugCallback;
  auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
      vk_env_.ins, "vkCreateDebugReportCallbackEXT");
  return func(vk_env_.ins, &debug_report_info, nullptr, &debug_report_fn_);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
    uint64_t obj, size_t location, int32_t code, const char *layerPrefix,
    const char *msg, void *userData) {
  printf("%s\n", msg);
  return 0;
}
#endif
}  // namespace vk
}  // namespace engine
}  // namespace ixr
