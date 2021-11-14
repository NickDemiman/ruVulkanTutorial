#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <optional>

#define WND_WIDTH = 800
#define WND_HEIGHT = 600

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#pragma region ������� ��� �������� DebugMessengger
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void setupDebugMessenger(bool enableValidationLayers, VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger)
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
#pragma endregion


class HelloTiangleApplication
{
	private:
	GLFWwindow* _windowHandler;
	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debugMessenger;
	const std::vector<const char*> _validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	
	#pragma region ������� ��� ����������������� ����

	private:
		void createInstance()
		{

			VkApplicationInfo appInfo = fillAppInfo();
			auto exts = getRequiredExtensions();
			VkInstanceCreateInfo createInfo = fillInstInfo(&appInfo, &exts);
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			if (enableValidationLayers && checkValidationLayerSupport())
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
				createInfo.ppEnabledLayerNames = _validationLayers.data();

				populateDebugMessengerCreateInfo(debugCreateInfo);
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
			}
			else
			{
				createInfo.enabledLayerCount = 0;

				createInfo.pNext = nullptr;
			}

			if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create instance!");
			}
		}

		void initVulkan()
		{
			createInstance();
			setupDebugMessenger(enableValidationLayers, _instance, &_debugMessenger);
			pickPhysicalDevice();
		}

		void mainLoop()
		{
			while (!glfwWindowShouldClose(_windowHandler))
			{
				glfwPollEvents();
			}

		}

		void cleanup()
		{
			if (enableValidationLayers)
			{
				DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
			}

			vkDestroyInstance(_instance, nullptr);

			glfwDestroyWindow(_windowHandler);

			glfwTerminate();
		}

	void initWindow(uint32_t width = 800, uint32_t height = 600)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // ��������� OpenCL, ��� ��� ���������� GLFW ���������� ����� ��� ���
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	  // ������� �������� ���� �������������. 1 - ����� �������� ��������, 2 - ����.

		_windowHandler = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr); // �������� ���� ������
	}

	bool checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : _validationLayers) {
			bool layerFound = false;
			

			for (const auto& layerProperties : availableLayers) {
				//std::cout << layerProperties.layerName << std::endl;
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

public:

	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	#pragma endregion

	#pragma region ��� ������� ��� �������� ��������
	private:
		std::vector<const char*> getRequiredExtensions()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

			if (enableValidationLayers)
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			return extensions;
		}

		VkApplicationInfo fillAppInfo()
		{
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Hello Triangle";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "No Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_2;

			return appInfo;
		}

		VkInstanceCreateInfo fillInstInfo(VkApplicationInfo* appInfo, std::vector<const char*>* extensions)
		{
			VkInstanceCreateInfo createInfo{};

			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = appInfo;

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			if (enableValidationLayers && checkValidationLayerSupport())
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
				createInfo.ppEnabledLayerNames = _validationLayers.data();

				populateDebugMessengerCreateInfo(debugCreateInfo);
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
				createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions->size());
				createInfo.ppEnabledExtensionNames = extensions->data();
			}
			else
			{
				createInfo.enabledLayerCount = 0;

				createInfo.pNext = nullptr;
			}

			return createInfo;
		}
	#pragma endregion

	#pragma region ��� �������
	private:
		void pickPhysicalDevice()
		{
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			uint32_t deviceCount = 0;

			vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);


			if (deviceCount == 0) 
			{
				throw std::runtime_error("failed to find GPUs with Vulkan support!");
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

			std::multimap<int, VkPhysicalDevice> candidates;

			for (const auto& device : devices) 
			{
				if (isDeviceSuitable(device, &deviceProperties, &deviceFeatures))
				{
					int score = rateDeviceSuitability(device, deviceProperties, deviceFeatures);
					candidates.insert(std::make_pair(score, device));
				}
			}

			// Check if the best candidate is suitable at all
			if (candidates.rbegin()->first > 0) {
				physicalDevice = candidates.rbegin()->second;
			}
			else 
			{
				throw std::runtime_error("failed to find a suitable GPU!");
			}
		}

		int rateDeviceSuitability(VkPhysicalDevice device, VkPhysicalDeviceProperties deviceProperties, VkPhysicalDeviceFeatures deviceFeatures)
		{
			int score = 0;

			// Discrete GPUs have a significant performance advantage
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
			{
				score += 1000;
			}

			// Maximum possible size of textures affects graphics quality
			score += deviceProperties.limits.maxImageDimension2D;

			// Application can't function without geometry shaders
			if (!deviceFeatures.geometryShader) 
			{
				return 0;
			}

			return score;
		}

		bool isDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceProperties* deviceProperties, VkPhysicalDeviceFeatures* deviceFeatures)
		{
			vkGetPhysicalDeviceProperties(device, deviceProperties);
			vkGetPhysicalDeviceFeatures(device, deviceFeatures);

			QueueFamilyIndices indices = findQueueFamilies(device);	

			return deviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
				deviceFeatures->geometryShader&&
				indices.isComplete();
		}
	#pragma endregion

	#pragma region ��������� ��������
	private:
		struct QueueFamilyIndices 
		{
			std::optional<uint32_t> graphicsFamily;

			bool isComplete() {
				return graphicsFamily.has_value();
			}
		};

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) 
		{
			QueueFamilyIndices indices;
			
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies) 
			{
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
				{
					indices.graphicsFamily = i;
				}

				i++;
			}

			return indices;
		}

	#pragma endregion

};

