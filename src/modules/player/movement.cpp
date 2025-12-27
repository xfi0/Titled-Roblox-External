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
	printf("\n Saved Position %f %f %f\n", cache::cachedLocalPlayer.position.x, cache::cachedLocalPlayer.position.y, cache::cachedLocalPlayer.position.z);
}
void movement::SpinBot() {

}
struct cframe final
{

};
void movement::NoClip() // probably ahh but i tested 3 other things and it wouldnt work, neither does this
{
	try {
		
	}
	catch (...){

	}
}
