#include "GameManagerSystem.hpp"

#include "GameManagerWorldComponent.hpp"
#include "Level.hpp"

#include <DeferredTaskSystem.hpp>
#include <TransformComponent.hpp>
#include <Physics2DColliders.hpp>
#include <RigidBody2DComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <CameraComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <LightSourceComponent.hpp>
#include "GroundComponent.hpp"
#include "ObstacleComponent.hpp"

using namespace SGJ;
using namespace Poly;


void SGJ::GameManagerSystem::LoadLevel(Poly::World* world, const Poly::String& path)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	gameMgrCmp->Levels.PushBack(new Level(path));
}

Poly::UniqueID GameManagerSystem::CreateGroundObject(Poly::World* world, const Poly::Vector& position, eTileType tileType)
{
	UniqueID ground = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ground);
	DeferredTaskSystem::AddComponentImmediate<GroundComponent>(world, ground);
	Poly::TransformComponent* groundTrans = world->GetComponent<Poly::TransformComponent>(ground);

	DeferredTaskSystem::AddComponentImmediate<Poly::Box2DColliderComponent>(world, ground, Vector(1, 1, 0));
	DeferredTaskSystem::AddComponentImmediate<Poly::RigidBody2DComponent>(world, ground, world, tileType == eTileType::RIGIDBODYGROUND ? eRigidBody2DType::DYNAMIC : eRigidBody2DType::STATIC);
	
	UniqueID mesh = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, mesh);
	Poly::TransformComponent* meshTrans = world->GetComponent<Poly::TransformComponent>(mesh);
	meshTrans->SetParent(groundTrans);
	meshTrans->SetLocalRotation(Quaternion(Vector::UNIT_X, 90_deg));
	Vector correctedSize = Vector(1, 1, 0);
	correctedSize.Z = 1.0f;
	meshTrans->SetLocalScale(correctedSize);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, mesh, "Quad.obj", eResourceSource::GAME, Color(0, 0, 0));

	groundTrans->SetLocalTranslation(position);
	return ground;
}

Poly::UniqueID GameManagerSystem::CreateObstacleObject(Poly::World* world, const Poly::Vector& position, const Poly::Vector& size, eTileType tileType)
{
	UniqueID obstacle = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, obstacle);
	DeferredTaskSystem::AddComponentImmediate<ObstacleComponent>(world, obstacle, tileType);
	Poly::TransformComponent* obstacleTrans = world->GetComponent<Poly::TransformComponent>(obstacle);

	DeferredTaskSystem::AddComponentImmediate<Poly::Box2DColliderComponent>(world, obstacle, size);
	DeferredTaskSystem::AddComponentImmediate<Poly::RigidBody2DComponent>(world, obstacle, world, eRigidBody2DType::STATIC);

	UniqueID mesh = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, mesh);
	Poly::TransformComponent* meshTrans = world->GetComponent<Poly::TransformComponent>(mesh);
	meshTrans->SetParent(obstacleTrans);
	meshTrans->SetLocalRotation(Quaternion(Vector::UNIT_X, 90_deg));
	Vector correctedSize = Vector(1, 1, 0);
	correctedSize.Z = 1.0f;
	meshTrans->SetLocalScale(correctedSize);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, mesh, "Quad.obj", eResourceSource::GAME, Color(1, 0, 0));

	obstacleTrans->SetLocalTranslation(position);
	return obstacle;
}

Poly::UniqueID GameManagerSystem::SpawnPlayer(Poly::World* world, const Poly::Vector& position)
{
	UniqueID player = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, player);
	TransformComponent* playerTrans = world->GetComponent<Poly::TransformComponent>(player);
	DeferredTaskSystem::AddComponentImmediate<Poly::Box2DColliderComponent>(world, player, Vector(1, 1, 0));
	DeferredTaskSystem::AddComponentImmediate<Poly::RigidBody2DComponent>(world, player, world, eRigidBody2DType::DYNAMIC);

	UniqueID body = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, body);
	Poly::TransformComponent* bodyTrans = world->GetComponent<Poly::TransformComponent>(body);
	bodyTrans->SetParent(playerTrans);
	bodyTrans->SetLocalRotation(Quaternion(Vector::UNIT_X, 90_deg));
	Vector correctedSize = Vector(1, 2, 0);
	correctedSize.Z = 1.0f;
	bodyTrans->SetLocalScale(correctedSize);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, body, "Quad.obj", eResourceSource::GAME, Color(0, 0, 1));

	playerTrans->SetLocalTranslation(position);
	return player;
}

void SGJ::GameManagerSystem::SpawnLevel(Poly::World* world, size_t idx)
{
	// cleanup previous level
	DespawnLevel(world);
	
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	Level* level = gameMgrCmp->Levels[idx];

	// calculate level center
	size_t meanW = 0, meanH = 0;
	size_t count = 0;
	for (size_t idx = 0; idx < level->Tiles.GetSize(); ++idx)
	{
		if (level->Tiles[idx] != SGJ::eTileType::NOTHING)
		{
			meanW += (idx % level->Width);
			meanH += (idx / level->Width);
			++count;
		}
	}
	meanW /= count;
	meanH /= count;

	// spawn level tiles
	for (int idx = 0; idx < level->Tiles.GetSize(); ++idx)
	{
		if (level->Tiles[idx] != SGJ::eTileType::NOTHING)
		{
			int posW = (idx % level->Width) - meanW;
			int posH = (idx / level->Width) - meanH;

			switch (level->Tiles[idx])
			{
			case eTileType::PLAYERSTARTPOS:
				gameMgrCmp->Player = SpawnPlayer(world, Vector(posW, -posH, 0));
				break;

			case eTileType::STATICGROUND:
			case eTileType::RIGIDBODYGROUND:
				gameMgrCmp->LevelEntities.PushBack(CreateGroundObject(world, Vector(posW, -posH, 0), level->Tiles[idx]));
				break;

			case eTileType::SPIKESBOTTOM:
			case eTileType::SPIKESTOP:
			case eTileType::SPIKESLEFT:
			case eTileType::SPIKESRIGHT:
				gameMgrCmp->LevelEntities.PushBack(CreateObstacleObject(world, Vector(posW, -posH, 0), Vector(1, 1, 0), level->Tiles[idx]));
				break;

			default:
				break;
			}
		}
	}
}

void SGJ::GameManagerSystem::DespawnLevel(Poly::World* world)
{
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	for (auto ent : gameMgrCmp->LevelEntities)
		DeferredTaskSystem::DestroyEntityImmediate(world, ent);
}

void SGJ::GameManagerSystem::PrepareNonlevelObjects(Poly::World * world)
{
	// Spawn entities
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	gameMgrCmp->Camera = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), gameMgrCmp->Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(gEngine->GetWorld(), gameMgrCmp->Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(gEngine->GetWorld(), gameMgrCmp->Camera, 10.0f, 0.003f);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = gEngine->GetWorld()->GetComponent<Poly::TransformComponent>(gameMgrCmp->Camera);
	cameraTrans->SetLocalTranslation(Vector(0, 0, 50.f));

	// Set background 
	//double Time = gEngine->GetWorld()->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime();
	//Background = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	//DeferredTaskSystem::AddComponentImmediate<Poly::BackgroundComponent>(gEngine->GetWorld(), Background, Time);
	//GameManagerComponent* gameManagerComponent = Engine->GetWorld()->GetComponent<GameManagerComponent>(GameManager);
	//gEngine->RegisterGameUpdatePhase(BackgroundSystem::BackgroundSystemSystemPhase);

	// SETUP SCENE HERE


	UniqueID id = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(gEngine->GetWorld(), id);
	Poly::TransformComponent* lightTrans = gEngine->GetWorld()->GetComponent<Poly::TransformComponent>(id);
	lightTrans->SetLocalRotation(Quaternion(Vector::UNIT_Y, 180_deg));
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightSourceComponent>(gEngine->GetWorld(), id, Color(1,0,0), 1.0f);
	gameMgrCmp->OtherEntities.PushBack(id);
}

void SGJ::GameManagerSystem::Cleanup(Poly::World* world)
{
	DespawnLevel(world);
	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	for (auto ent : gameMgrCmp->OtherEntities)
		DeferredTaskSystem::DestroyEntityImmediate(world, ent);

	DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->Player);
	DeferredTaskSystem::DestroyEntityImmediate(world, gameMgrCmp->Camera);
}