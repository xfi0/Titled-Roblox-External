#include "overlay.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../sdk/math/math.h"
#include "../sdk/sdk.h"
#include "../cache/cache.h"
#include "modules/visuals/ESP.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool g_showMenu = true;

Overlay::Overlay() {}

Overlay::~Overlay()
{
    Cleanup();
}

bool Overlay::Initialize()
{
    CreateOverlayWindow();
    InitializeDirectX();
    InitializeImGui();

    fps_timer = std::chrono::high_resolution_clock::now();
    last_frame_time = fps_timer;
    frame_count = 0;
    current_fps = 0.0f;
    SetLayeredWindowAttributes(window_handle, RGB(0, 0, 0), 0, LWA_COLORKEY);

    return true;
}

void Overlay::CreateOverlayWindow()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "Minus";
    RegisterClassEx(&wc);

    window_handle = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        "Minus", "Titled External",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    SetLayeredWindowAttributes(window_handle, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(window_handle, SW_SHOW);
    UpdateWindow(window_handle);

    if (!g_showMenu) {
        LONG_PTR ex = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
        ex |= WS_EX_TRANSPARENT;
        SetWindowLongPtr(window_handle, GWL_EXSTYLE, ex);
        SetWindowPos(window_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

void Overlay::InitializeDirectX()
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
    scd.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 0;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = window_handle;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED, NULL, 0, D3D11_SDK_VERSION, &scd, &swapchain, &device, &featureLevel, &context);

    if (FAILED(hr)) {
        return;
    }

    IDXGIDevice* dxgiDevice = nullptr;
    if (SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)) && dxgiDevice)
    {
        IDXGIAdapter* dxgiAdapter = nullptr;
        if (SUCCEEDED(dxgiDevice->GetAdapter(&dxgiAdapter)) && dxgiAdapter)
        {
            IDXGIFactory* dxgiFactory = nullptr;
            if (SUCCEEDED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)) && dxgiFactory)
            {
                dxgiFactory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER);
                dxgiFactory->Release();
            }
            dxgiAdapter->Release();
        }
        dxgiDevice->Release();
    }

    ID3D11Texture2D* back_buffer = nullptr;
    if (SUCCEEDED(swapchain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))) && back_buffer)
    {
        device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
        back_buffer->Release();
    }
}

void Overlay::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.AntiAliasedLines = false;
    style.AntiAliasedFill = false;

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window_handle);
    ImGui_ImplDX11_Init(device, context);
}

void Overlay::BeginFrame()
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    frame_count++;
    float elapsed = duration_cast<duration<float>>(now - fps_timer).count();
    if (elapsed >= 1.0f) {
        current_fps = frame_count / elapsed;
        frame_count = 0;
        fps_timer = now;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (context && render_target_view)
        context->OMSetRenderTargets(1, &render_target_view, NULL);
}

void Overlay::EndFrame()
{
    DrawWatermark();

    ImGui::Render();

    static const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (context && render_target_view)
        context->ClearRenderTargetView(render_target_view, clear_color);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (swapchain)
    {
        swapchain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
    }
}

void Overlay::DrawText(math::vector2 position, const char* text, unsigned int color)
{
    ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x, position.y), color, text);
}

void Overlay::DrawWatermark()
{
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        g_showMenu = !g_showMenu;

        LONG_PTR ex = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
        if (g_showMenu)
            ex &= ~WS_EX_TRANSPARENT;
        else
            ex |= WS_EX_TRANSPARENT;
        SetWindowLongPtr(window_handle, GWL_EXSTYLE, ex);

        SetLayeredWindowAttributes(window_handle, RGB(0, 0, 0), 0, LWA_COLORKEY);
        SetWindowPos(window_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        char buf[64];
        snprintf(buf, sizeof(buf), "Titled | %.0f FPS", GetFPS());
        ImVec2 textSize = ImGui::CalcTextSize(buf);
        ImVec2 pos = ImVec2(displaySize.x - textSize.x - 10.0f, 10.0f);
        ImU32 outline = IM_COL32(0, 0, 0, 200);
        ImU32 white = IM_COL32(255, 255, 255, 220);
        dl->AddText(ImVec2(pos.x - 1, pos.y - 1), outline, buf);
        dl->AddText(ImVec2(pos.x + 1, pos.y - 1), outline, buf);
        dl->AddText(ImVec2(pos.x - 1, pos.y + 1), outline, buf);
        dl->AddText(ImVec2(pos.x + 1, pos.y + 1), outline, buf);
        dl->AddText(pos, white, buf);
    }

    if (!g_showMenu)
        return;

    ImGui::SetNextWindowSize(ImVec2(420, 220), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.85f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Titled", nullptr, flags))
    {
        ImGui::Text("Player");
        ImGui::Separator();

        static float JumpPower = 50.0f;
        if (ImGui::SliderFloat("Jump Power", &JumpPower, 0.0f, 100.0f))
        {
            cache::cachedLocalPlayer.humanoid.SetJumpPower(JumpPower);
        }
        static float WalkSpeed = 50.0f;
        if (ImGui::SliderFloat("Walk Speed", &WalkSpeed, 0.0f, 100.0f))
        {
            cache::cachedLocalPlayer.humanoid.SetWalkSpeed(JumpPower);
        }
        ImGui::Checkbox("Box ESP", &Visuals::BoxESP::boxESPEnabled);
        ImGui::Checkbox("Name ESP", &Visuals::NameESP::nameESPEnabled);

        ImGui::SameLine();
        if (ImGui::Button("Close Menu"))
        {
            g_showMenu = false;
            LONG_PTR ex = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
            ex |= WS_EX_TRANSPARENT;
            SetWindowLongPtr(window_handle, GWL_EXSTYLE, ex);
            SetLayeredWindowAttributes(window_handle, RGB(0, 0, 0), 0, LWA_COLORKEY);
            SetWindowPos(window_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }

        ImGui::End();
    }
}

bool Overlay::ShouldExit()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) return true;
    }
    return false;
}

bool Overlay::IsRunning()
{
    return !ShouldExit();
}

LRESULT CALLBACK Overlay::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch (msg) {
    case WM_LBUTTONDOWN:
        if (g_showMenu) {
            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse) {
                ReleaseCapture();
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                return 0;
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Overlay::Cleanup()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (render_target_view) { render_target_view->Release(); render_target_view = nullptr; }
    if (swapchain) { swapchain->Release(); swapchain = nullptr; }
    if (context) { context->Release(); context = nullptr; }
    if (device) { device->Release(); device = nullptr; }

    if (window_handle) { DestroyWindow(window_handle); window_handle = nullptr; }
}