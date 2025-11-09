#include "memory/memory.h"
#include <thread>;
#include <chrono>;
#include "sdk/offsets.h"
#include "sdk/sdk.h"
#include "game/game.h"
#include "cache/cache.h"
#include "overlay/overlay.h"
#include "imgui/imgui.h"
#include "modules/visuals/ESP.h"
#include "modules/aim/aimbot.h"
#include "modules/player/movement.h"

std::unique_ptr<Overlay> OverlayInstance = std::make_unique<Overlay>();
static void moduleLoop() {
	if (aimbot::aimbotEnabled) {
		aimbot::Run();
	}
	//const math::vector3& pos = math::vector3(100.0f, 100.0f, 100.0f);
	//movement::Teleport(pos); // just to test movement module
}
static void ClearPlayers() {
	cache::cachedLocalPlayer = cache::entity_t{};
	cache::cachedPlayers = std::vector<cache::entity_t>{};
	//cache::cachedPlayersMutex = std::mutex{};
}
static void HandleTeleport() {

}
std::int32_t main() {
	try {
		static const char* procName = "RobloxPlayerBeta.exe";

		if (!memory->find_process_id(procName)) {
			printf("Failed to find roblox, is it running?\n");
			std::this_thread::sleep_for(std::chrono::seconds(5));
			return 1;
		}
		if (!memory->attach_to_process(procName)) {
			printf("Failed to attach to process: %s\n", procName);
			std::this_thread::sleep_for(std::chrono::seconds(5));
			return 1;
		}
		if (!memory->find_module_address(procName)) {
			printf("Failed to find module address for process: %s\n", procName);
			std::this_thread::sleep_for(std::chrono::seconds(5));
			return 1;
		}
		std::uint64_t fakeDataModel = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::FakeDataModel::Pointer);
		game::dataModel = Roblox::instance_t(memory->read<std::uint64_t>(fakeDataModel + Offsets::FakeDataModel::RealDataModel));
		//printf("DataModel: 0x%llx\n", game::dataModel.address);

		game::visualEngine = memory->read<std::uint64_t>(memory->get_module_address() + Offsets::VisualEngine::Pointer);
		//printf("VisualEngine: 0x%llx\n", game::visualEngine);

		game::players = { game::dataModel.FindFirstChildByClass("Players") };
		game::workSpace = { game::dataModel.FindFirstChildByClass("Workspace") };
		game::camera = { game::workSpace.FindFirstChild("Camera") };
		
		std::thread(cache::Run).detach();
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
			movement::NoClip();
			Visuals::BoxESP::RenderLoop();
			OverlayInstance->EndFrame();
		}
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	catch (...) {

	}
}
