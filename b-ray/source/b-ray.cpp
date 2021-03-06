#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include "ray.h"
#include "hitable.h"
#include "sphere.h"
#include "hitable_list.h"
#include <random>
#include "camera.h"
#include <thread>
#include "material.h"


//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STBI_MSC_SECURE_CRT
//#include <stb_image_write.h>

using uchar = unsigned char;
using namespace bray;
using namespace glm;
using namespace std;



vec3 Color(const Ray& r, hitable* world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.0f, FLT_MAX, rec))
	{
		Ray scattered;
		vec3 attenuation;
		if(depth < 10 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * Color(scattered, world, depth + 1);
		}
		return vec3(0,0,0);		

		// vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		// return 0.5f * (rec.normal + vec3(1, 1, 1));
		// return 0.5f * Color(Ray(rec.p, target - rec.p), world);
	}
	// Return sky
	vec3 unit_direction = glm::normalize(r.Direction());
	float t = 0.5f * (unit_direction.y + 1.0f);
	return mix(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), t);
}

/*
int main(int argc, char* argv[])
{
	int nx = 2400;
	int ny = 1200;
	int ns = 200;

	uchar* data = new unsigned char[nx * ny * 3];
	uchar* p = data;

	hitable* list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f);
	list[1] = new sphere(vec3(0, -100.5f, -1), 100);
	hitable* world = new hitable_list(list, 2);

	default_random_engine gen;
	uniform_real_distribution<float> random(0.0f, 1.0f);
	camera cam;

	auto fun = [&]()
	{
		printf("Tracing from thread.");

		for (int j = ny - 1; j >= 0; j--)
		{
			for (int i = 0; i < nx; i++)
			{
				vec3 color(0, 0, 0);
				for (int s = 0; s < ns; s++)
				{
					float u = float(i + random(gen)) / float(nx);
					float v = float(j + random(gen)) / float(ny);
					Ray r = cam.get_ray(u, v);
					color += Color(r, world);
				}
				color /= float(ns);
				//color = vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				*p++ = uchar(255.99f * color.r);
				*p++ = uchar(255.99f * color.g);
				*p++ = uchar(255.99f * color.b);
			}
		}
	};

	thread thr(fun);

	printf("Main goes on");

	thr.join();
	
	stbi_write_png("image3.png", nx, ny, 3, data, 3*nx);

	delete[] data;
}
*/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <glm/detail/type_int.hpp>

void set_imgui_theme()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImGui::GetStyle().FramePadding = ImVec2(4.0f, 2.0f);
	ImGui::GetStyle().ItemSpacing = ImVec2(8.0f, 2.0f);
	ImGui::GetStyle().WindowRounding = 2.0f;
	ImGui::GetStyle().ChildRounding = 2.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;
	ImGui::GetStyle().GrabRounding = 1.0f;
	ImGui::GetStyle().WindowBorderSize = 1.0f;
	ImGui::GetStyle().FrameBorderSize = 1.0f;
	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.44f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.71f, 0.70f, 0.70f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.63f, 0.63f, 0.63f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.43f, 0.44f, 0.46f, 0.78f);
	colors[ImGuiCol_Button] = ImVec4(0.61f, 0.61f, 0.62f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.57f, 0.57f, 0.57f, 0.52f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.61f, 0.63f, 0.64f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.64f, 0.64f, 0.65f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.58f, 0.58f, 0.59f, 0.55f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.52f, 0.52f, 0.52f, 0.55f);
	colors[ImGuiCol_Separator] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
	//colors[ImGuiCol_b] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	//colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	//colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.71f, 0.72f, 0.73f, 0.57f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.16f, 0.16f, 0.17f, 0.95f);
}

struct pixel_render_data
{
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
	int samples = 0;
};

struct rgb_color
{
	uchar r = 0;
	uchar g = 0;
	uchar b = 0;
};

const int width = 600;
const int height = 300;
const int samples = 1000;
unsigned int texture = 0;
rgb_color image_data[width * height];
pixel_render_data image_pixels[width * height];
bool stop = false;

static void create_image()
{
	if (texture)
		glDeleteTextures(1, &texture);

	glGenTextures(1, &texture);											// Gen    

	glBindTexture(GL_TEXTURE_2D, texture);                                 // Bind

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    // Minmization
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);    // Magnification

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(
		GL_TEXTURE_2D,						// What (target)
		0,									// Mip-map level
		GL_RGB8,							// Internal format
		width,								// Width
		height,								// Height
		0,									// Border
		GL_RGB,								// Format (how to use)
		GL_UNSIGNED_BYTE,					// Type   (how to intepret)
		image_data);						// Data	
}

static void update_image()
{
	glBindTexture(GL_TEXTURE_2D, texture);  // Bind
	glTexImage2D(
		GL_TEXTURE_2D,						// What (target)
		0,									// Mip-map level
		GL_RGB8,							// Internal format
		width,								// Width
		height,								// Height
		0,									// Border
		GL_RGB,								// Format (how to use)
		GL_UNSIGNED_BYTE,					// Type   (how to intepret)
		image_data);						// Data
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "B-ray", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		// LOG("Failed to initialize OpenGL context");
		// ASSERT(false);
	}


	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	set_imgui_theme();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("external/imgui/extra_fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("external/imgui/extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);

	create_image();

	hitable* list[4];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
	list[1] = new sphere(vec3(0, -100.5f, -1), 100, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f), 0.3f));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5f, new dielectric(1.5f));
	hitable* world = new hitable_list(list, 4);

	default_random_engine gen;
	uniform_real_distribution<float> random(0.0f, 1.0f);
	camera cam;

	auto fun = [&]()
	{	
		for (int s = 0; s < samples; s++)
		{
			for (int y = height - 1; y >= 0; y--)
			{
				for (int x = 0; x < width; x++)
				{
					auto& pixel = image_pixels[x + y * width];

					float u = float(x + random(gen)) / float(width);
					float v = float(y + random(gen)) / float(height);
					Ray r = cam.get_ray(u, v);

					auto color = Color(r, world, 3);
					pixel.color += color;
					pixel.samples++;

					color = pixel.color * (1.0f / pixel.samples);
					color = vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
					int idx = x + (height - y - 1) * width;
					image_data[idx].r = uchar(255.99f * color.r);
					image_data[idx].g = uchar(255.99f * color.g);
					image_data[idx].b = uchar(255.99f * color.b);

					if (stop)
						return;
				}			
			}
		}

		/*
		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				vec3 color(0, 0, 0);

				for (int s = 0; s < samples; s++)
				{					
					float u = float(x + random(gen)) / float(width);
					float v = float(y + random(gen)) / float(height);
					Ray r = cam.get_ray(u, v);

					color += Color(r, world, 0);
												
					if (stop)
						return;
				}

				color *= (1.0f / samples);
				color = vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				int idx = x + (height - y - 1) * width;
				image_data[idx].r = uchar(255.99f * color.r);
				image_data[idx].g = uchar(255.99f * color.g);
				image_data[idx].b = uchar(255.99f * color.b);
			}
		}
		*/
	};

	thread thr(fun);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		update_image();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			static bool open = true;

			ImGui::Begin("Render View", &open, ImVec2(width + 30, height + 30) , -1, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);

			// Menu
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Main menu bar", NULL, &show_demo_window);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Examples"))
				{
					ImGui::MenuItem("Main menu bar", NULL, &show_demo_window);
					ImGui::MenuItem("Console", NULL, &show_demo_window);
					ImGui::MenuItem("Log", NULL, &show_demo_window);
					ImGui::MenuItem("Simple layout", NULL, &show_demo_window);
					ImGui::MenuItem("Property editor", NULL, &show_demo_window);
					ImGui::MenuItem("Long text display", NULL, &show_demo_window);
					ImGui::MenuItem("Auto-resizing window", NULL, &show_demo_window);
					ImGui::MenuItem("Constrained-resizing window", NULL, &show_demo_window);
					ImGui::MenuItem("Simple overlay", NULL, &show_demo_window);
					ImGui::MenuItem("Manipulating window titles", NULL, &show_demo_window);
					ImGui::MenuItem("Custom rendering", NULL, &show_demo_window);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help"))
				{
					ImGui::MenuItem("Metrics", NULL, &show_demo_window);
					ImGui::MenuItem("Style Editor", NULL, &show_demo_window);
					ImGui::MenuItem("About Dear ImGui", NULL, &show_demo_window);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			static float val;

			// Menu
			//if (ImGui::BeginMenuBar())
			{
				ImGui::Button("A");				
				ImGui::SameLine();				
				ImGui::Button("B");
				ImGui::SameLine();
				ImGui::Button("C");
				ImGui::SameLine();
				ImGui::Separator();
				ImGui::SameLine();
				ImGui::SliderFloat("Exposure", &val, 0.0f, 100.0f);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Exposure");
				ImGui::SameLine();
				ImGui::Separator();
				ImGui::SameLine();
				ImGui::SliderFloat("Gamma", &val, 0.0f, 100.0f);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Gamma");
				ImGui::Separator();
				//ImGui::EndMenuBar();
			}

			//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			
			ImTextureID id = (void*)(glm::uint*)texture;		
			ImGui::Image(id, ImVec2(width, height));

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	stop = true;

	thr.join();

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


