#include "movement.h"
#include "cache/cache.h"
#include "game/game.h"
#include "sdk/sdk.h"
#include "sdk/offsets.h"

void movement::Teleport(math::vector3 Position)
{
	//auto primitive = memory->read<uintptr_t>(cache::cachedLocalPlayer.humanoid.address + Offsets::BasePart::Primitive);
	//memory->write<math::vector3>(primitive + Offsets::BasePart::Position, Position);
	////printf("\n Primative: 0x%llx\n", primitive + Offsets::BasePart::Position);
	////printf("\n Teleported To %f %f %f\n", Position.x, Position.y, Position.z);
}
void movement::SavePosition() {

}

void movement::NoClip() // probably ahh but i tested 3 other things and it wouldnt work, neither does this
{
	/*try {
		memory->write<bool>(cache::cachedLocalPlayer.humanoid.address + Offsets::PrimitiveFlags::CanCollide, false);
		memory->write<bool>(cache::cachedLocalPlayer.leftArm.address + Offsets::PrimitiveFlags::CanCollide, false);
		memory->write<bool>(cache::cachedLocalPlayer.leftLeg.address + Offsets::PrimitiveFlags::CanCollide, false);
		memory->write<bool>(cache::cachedLocalPlayer.rightArm.address + Offsets::PrimitiveFlags::CanCollide, false);
		memory->write<bool>(cache::cachedLocalPlayer.leftArm.address + Offsets::PrimitiveFlags::CanCollide, false);
		memory->write<bool>(cache::cachedLocalPlayer.head.address + Offsets::PrimitiveFlags::CanCollide, false);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	catch (...){

	}*/
}
