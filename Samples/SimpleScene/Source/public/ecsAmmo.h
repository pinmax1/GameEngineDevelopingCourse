#pragma once

#include <flecs.h>
#include <Vector.h>

struct Ammo { 
	int count; 
};
struct ReloadTimer { 
	float timeLeft; 
};
struct BulletLifetime { 
	float lifetime; 
};

struct Bullet {};

struct BonusBullets {
	int count;
};

void RegisterEcsAmmoSystems(flecs::world& world);