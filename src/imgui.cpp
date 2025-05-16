#include "Imgui.hpp"

#include "App.hpp"
void InitImGui(SDL_Window* window, SDL_GLContext gl_context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // 禁用 ImGui 的默認配置文件保存（可選，避免干擾）
    io.IniFilename = nullptr;

    // 設定 ImGui 的樣式，匹配圖片的顏色風格
    ImGui::StyleColorsDark(); // 從深色主題開始
    ImGuiStyle& style = ImGui::GetStyle();

    // 調整間距和填充以匹配圖片的緊湊風格
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.ScrollbarSize = 10.0f;
    style.WindowRounding = 0.0f; // 移除窗口圓角，匹配圖片的直角邊框
    style.FrameRounding = 0.0f; // 移除框架圓角
    style.GrabRounding = 0.0f; // 移除滑塊抓取部分的圓角

    // 設置顏色，匹配圖片的深色主題和藍色高亮
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 白色文字
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 非常深的背景
    style.Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 邊框顏色
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // 框架背景（如滑塊背景）
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f); // 框架懸停
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f); // 框架激活
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // 標題欄背景
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // 標題欄激活
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // 標題欄折疊
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // 藍色複選框勾選
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // 藍色滑塊抓取
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.7f, 1.0f, 1.0f); // 藍色滑塊抓取激活
    style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // 按鈕背景
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f); // 按鈕懸停
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f); // 按鈕激活
    style.Colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f); // 分隔線

    // 初始化 ImGui 的 SDL 和 OpenGL3 後端
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void RenderImGui(App& app) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug Window");
	ImGui::StyleColorsDark(); // 從深色主題開始
    ImGuiStyle& style = ImGui::GetStyle();

    // 調整間距和填充以匹配圖片的緊湊風格
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.ScrollbarSize = 10.0f;
    style.WindowRounding = 0.0f; // 移除窗口圓角，匹配圖片的直角邊框
    style.FrameRounding = 0.0f; // 移除框架圓角
    style.GrabRounding = 0.0f; // 移除滑塊抓取部分的圓角

    // 設置顏色，匹配圖片的深色主題和藍色高亮
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 白色文字
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f); // 非常深的背景
    style.Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 邊框顏色
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // 框架背景（如滑塊背景）
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f); // 框架懸停
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f); // 框架激活
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // 標題欄背景
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // 標題欄激活
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f); // 標題欄折疊
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // 藍色複選框勾選
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.5f, 1.0f, 1.0f); // 藍色滑塊抓取
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.7f, 1.0f, 1.0f); // 藍色滑塊抓取激活
    style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // 按鈕背景
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f); // 按鈕懸停
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f); // 按鈕激活
    style.Colors[ImGuiCol_Separator] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f); // 分隔線
    // 显示游戏状态信息
    ImGui::Text("Game Debug Information");
    ImGui::Separator();

    // 显示位置和瓦片信息
    ImGui::Text("%s", app.m_DebugInfo.positionInfo.c_str());
    ImGui::Text("%s", app.m_DebugInfo.tileInfo.c_str());

    ImGui::Text("%d", app.m_DebugInfo.currentX);
    ImGui::Text("%d", app.m_DebugInfo.currentY);

    // 显示阶段和计时器信息
    ImGui::Text("%s", app.m_DebugInfo.phaseInfo.c_str());
    ImGui::Text("%s", app.m_DebugInfo.switchTimerInfo.c_str());

    // 显示鼠标坐标信息
    ImGui::Text("%s", app.m_DebugInfo.mousePtsdPos.c_str());
    ImGui::Text("%s", app.m_DebugInfo.mouseGamePos.c_str());
    ImGui::Text("%s", app.m_DebugInfo.boshyPtsdPos.c_str());
    ImGui::Text("%s", app.m_DebugInfo.overlapInfo.c_str());

    ImGui::Separator();

    // 已有的控制选项
    ImGui::Text("Controls");
    ImGui::InputFloat("Gravity", &app.Gravity, 0.1f, 1.0f, "%.1f");
    ImGui::SliderFloat("Gravity", &app.Gravity, -10.0f, 10.0f, "%.1f");

    if (ImGui::Button("Gravity to 0")) {
        app.Gravity = 0.0f;
    }
    if (ImGui::Button("Gravity to -0.5f")) {
        app.Gravity = -0.5f;
    }
    ImGui::Separator();
    ImGui::Text("map tp");

    // 世界選擇
    static const char* worlds[] = {"WORLD1", "WORLD2"};
    static int currentWorld = 0;
    ImGui::Combo("World", &currentWorld, worlds, IM_ARRAYSIZE(worlds));

    // 階段選擇
    static const char* phases[] = {"1", "2", "3_1", "3_2", "4_1", "4_2", "5", "6", "7", "8", "9", "10", "11", "12"};
    static int currentPhase = 0;
    ImGui::Combo("Phase", &currentPhase, phases, IM_ARRAYSIZE(phases));

    // 傳送按鈕
    if (ImGui::Button("tp")) {
        // 獲取當前選擇的世界和階段
        std::string newWorld = worlds[currentWorld];
        std::string newPhase = phases[currentPhase];

        // 更新世界和階段ㄝ
        app.TeleportToMap(newPhase, newWorld);
        app.ReloadMapObjects();
        std::cout << "tp: World " << newWorld << ", Phase " << newPhase << std::endl;
    }
    ImGui::Separator();
    ImGui::Text("cheat");

    // 添加上帝模式選項
    if (ImGui::Checkbox("God", &app.GodMode)) {
        // 切換上帝模式的額外操作（如果需要）
    }

    if (app.GodMode) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "ac");

        // 上帝模式子選項
        ImGui::Indent();
        static bool invincible = true;
        static bool infiniteJump = true;
        static bool noClip = false;

        ImGui::Checkbox("God", &invincible);
        ImGui::Checkbox("ij", &infiniteJump);
        ImGui::Checkbox("wh", &noClip);
        ImGui::Unindent();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}