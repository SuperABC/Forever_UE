#include "trivial.h"

#include "map/room.h"
#include "map/building.h"
#include "map/zone.h"
#include "map/block.h"


float ProjectOntoLine(float px, float py, float ax, float ay, float bx, float by) {
	float dx = bx - ax, dy = by - ay;
	float lenSq = dx * dx + dy * dy;
	if (lenSq <= 0.f) return 0.f;
	float t = ((px - ax) * dx + (py - ay) * dy) / lenSq;
	if (t < 0.f) t = 0.f;
	if (t > 1.f) t = 1.f;
	return t;
}

Node* ResolveNavigationNode(Room* room, Building* building, Zone* zone, Block* block) {
	if (room) return room->GetNavigationNode();
	if (building) {
		for (auto corner : building->GetBoundary().corners) {
			if (corner) return corner;
		}
	}
	if (zone) {
		for (auto corner : zone->GetBoundary().corners) {
			if (corner) return corner;
		}
	}
	if (block) {
		for (auto corner : block->GetBoundary().corners) {
			if (corner) return corner;
		}
	}
	return nullptr;
}
