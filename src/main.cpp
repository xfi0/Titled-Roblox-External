#include "memory/memory.h"
#include <thread>
#include <chrono>
#include "sdk/offsets.h"
#include "sdk/sdk.h"
#include "game/game.h"
#include "cache/cache.h"
#include "overlay/overlay.h"
#include "imgui/imgui.h"
#include "modules/visuals/ESP.h"
#include "modules/aim/aimbot.h"
#include "modules/player/movement.h"
#include <atomic>

std::unique_ptr<Overlay> OverlayInstance = std::make_unique<Overlay>();

namespace teleportHandler
{
    std::uint64_t lastDataModel = 0;
    std::uint64_t lastPlayers = 0;
    std::uint64_t lastWorkspace = 0;
    std::atomic<bool> initialized{ false };

    static bool HasTeleported()
    {
        if (!initialized.load())
        {
            lastDataModel = game::dataModel.address;
            lastPlayers = game::players.address;
            lastWorkspace = game::workSpace.address;
            initialized.store(true);
            return false;
        }

        try
        {
            std::uint64_t fakeDataModel = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::FakeDataModel::Pointer);
            std::uint64_t currentDataModel = memory->read<std::uint64_t>(fakeDataModel + Offsets::FakeDataModel::RealDataModel);

            if (currentDataModel != 0 && currentDataModel != lastDataModel)
                return true;

            Roblox::instance_t temp(currentDataModel);
            auto currentWorkspace = temp.FindFirstChildByClass("Workspace");
            auto currentPlayers = temp.FindFirstChildByClass("Players");

            if ((currentWorkspace.address != 0 && currentWorkspace.address != lastWorkspace) || (currentPlayers.address != 0 && currentPlayers.address != lastPlayers))
                return true;
        }
        catch (...)
        {
            return true;
        }

        return false;
    }

    static void Reinitialize()
    {
        try
        {
            std::uint64_t fakeDataModel = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::FakeDataModel::Pointer);
            game::dataModel = Roblox::instance_t(memory->read<std::uint64_t>(fakeDataModel + Offsets::FakeDataModel::RealDataModel));
            game::visualEngine = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::VisualEngine::Pointer);

            game::players = { game::dataModel.FindFirstChildByClass("Players") };
            game::workSpace = { game::dataModel.FindFirstChildByClass("Workspace") };
            game::camera = { game::workSpace.FindFirstChild("Camera") };

            lastDataModel = game::dataModel.address;
            lastPlayers = game::players.address;
            lastWorkspace = game::workSpace.address;
        }
        catch (...)
        {
        }
    }

    static void HandleLoop()
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (HasTeleported())
            {
                Reinitialize();
                cache::cachedLocalPlayer = cache::entity_t{};
                cache::cachedPlayers.clear();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
}

static void moduleLoop()
{
    if (aimbot::aimbotEnabled) aimbot::Run();
}

std::int32_t main()
{
    try
    {
        static const char* procName = "RobloxPlayerBeta.exe";

        if (!memory->find_process_id(procName)) {
            printf("Failed to find roblox, is it running?\n");
            std::this_thread::sleep_for(std::chrono::seconds(25));
            return 1;
        }
        if (!memory->attach_to_process(procName)) {
            printf("Failed to attach to process: %s\n", procName);
            std::this_thread::sleep_for(std::chrono::seconds(25));
            return 1;
        }
        if (!memory->find_module_address(procName)) {
            printf("Failed to find module address for process: %s\n", procName);
            std::this_thread::sleep_for(std::chrono::seconds(25));
            return 1;
        }

        std::uint64_t fakeDataModel = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::FakeDataModel::Pointer);
        game::dataModel = Roblox::instance_t(memory->read<std::uint64_t>(fakeDataModel + Offsets::FakeDataModel::RealDataModel));
        game::visualEngine = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::VisualEngine::Pointer);

        game::players = { game::dataModel.FindFirstChildByClass("Players") };
        game::workSpace = { game::dataModel.FindFirstChildByClass("Workspace") };
        game::camera = { game::workSpace.FindFirstChild("Camera") };

        std::thread(cache::Run).detach();
        std::thread(teleportHandler::HandleLoop).detach();

        if (!OverlayInstance->Initialize())
        {
            std::cout << "Failed to initialize overlay - check DirectX support" << std::endl;
            std::cin.get();
            return 1;
        }
        while (OverlayInstance->IsRunning())
        {
            OverlayInstance->BeginFrame();
            moduleLoop();
            Visuals::BoxESP::RenderLoop();
            OverlayInstance->EndFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (...)
    {
        return 1;
    }
}
