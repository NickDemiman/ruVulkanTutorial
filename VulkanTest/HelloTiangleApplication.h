#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define WND_WIDTH = 800
#define WND_HEIGHT = 600

class HelloTiangleApplication
{
private:
	GLFWwindow *_windowHandler;

	void initWindow(uint32_t width = 800, uint32_t height = 600)
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // отключаем OpenCL, так как библиотека GLFW изначально юзает эту АПИ
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	  // сделаем габариты окна неизменяемыми. 1 - какой параметр изменяем, 2 - ключ.

		_windowHandler = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr); // создадим наше окошко
	}


	void createInstance()
	{

	}

	void initVulkan()
	{
		
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

