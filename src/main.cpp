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

std::unique_ptr<Overlay> OverlayInstance = std::make_unique<Overlay>();
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
		game::camera = { game::dataModel.FindFirstChildByClass("Workspace").FindFirstChild("Camera") };

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
			Visuals::BoxESP::RenderLoop();
			OverlayInstance->EndFrame();
		}

		while (true) {
			for (cache::entity_t& entity : cache::cachedPlayers) {
				printf("\n%s Is in the cache\n", entity.name.c_str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	catch (...) {

	}
}