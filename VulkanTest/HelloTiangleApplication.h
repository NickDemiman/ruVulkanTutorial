#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#define WND_WIDTH = 800
#define WND_HEIGHT = 600

class HelloTiangleApplication
{
private:
	GLFWwindow* _windowHandler;
	VkInstance _instance;
	//const uint32_t _WIDTH = 800;
	//const uint32_t _HEIGHT = 600;

	void initWindow(uint32_t width = 800, uint32_t height = 600)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // отключаем OpenCL, так как библиотека GLFW изначально юзает эту АПИ
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	  // сделаем габариты окна неизменяемыми. 1 - какой параметр изменяем, 2 - ключ.

		_windowHandler = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr); // создадим наше окошко
	}

	void ListAllExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "available extensions:\n";

		for (const auto& extension : extensions) {
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}

	VkApplicationInfo fillAppInfo()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		return appInfo;
	}

	VkInstanceCreateInfo fillInstInfo(VkApplicationInfo* appInfo)
	{
		VkInstanceCreateInfo createInfo{};

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		ListAllExtensions();

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;

		return createInfo;
	}

	void createInstance()
	{
		VkApplicationInfo appInfo = fillAppInfo();
		VkInstanceCreateInfo createInfo = fillInstInfo(&appInfo);
		VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	void initVulkan()
	{
		createInstance();
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
		vkDestroyInstance(_instance, nullptr);

		glfwDestroyWindow(_windowHandler);

		glfwTerminate();
	}

public:

	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

};

