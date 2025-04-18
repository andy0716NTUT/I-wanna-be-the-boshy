#ifndef IMGUI_HPP
#define IMGUI_HPP

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <SDL.h>
#include <GL/glew.h>
#include <App.hpp>
// 僅聲明函數
void InitImGui(SDL_Window* window, SDL_GLContext gl_context);
void RenderImGui(App& app);
void CleanupImGui();

#endif // IMGUI_HPP