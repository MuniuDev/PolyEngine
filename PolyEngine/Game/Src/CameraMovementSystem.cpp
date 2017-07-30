#include "CameraMovementSystem.hpp"

#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include "Rigidbody2DComponent.hpp"
#include <PostprocessSettingsComponent.hpp>

#include "GameManagerWorldComponent.hpp"

using namespace Poly;
using namespace SGJ;

void SGJ::CameraMovementSystem::CameraMovementUpdatePhase(Poly::World* world)
{
	double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
	double time = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);

	GameManagerWorldComponent* gameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	UniqueID player = gameMgrCmp->Player;
	TransformComponent* playerTransCmp = world->GetComponent<TransformComponent>(player);
	RigidBody2DComponent* rigidbodyCmp = world->GetComponent<RigidBody2DComponent>(player);

	for (auto& tuple : world->IterateComponents<CameraComponent, TransformComponent>())
	{
		CameraComponent* cameraCmp = std::get<CameraComponent*>(tuple);
		TransformComponent* transformCmp = std::get<TransformComponent*>(tuple);

		float velocity = rigidbodyCmp->GetLinearSpeed().Length2();
		Angle TargetFov = Lerp(60_deg, 47_deg, Clamp(velocity/350.0f, 0.0f, 1.0f));
		cameraCmp->SetTargetFOV(TargetFov);
		cameraCmp->SetFOV(Lerp(cameraCmp->GetFOV(), cameraCmp->GetTargetFOV(), deltaTime));

		const float cameraHeight = 16.f;

		Vector TargetPosition;
		TargetPosition.Z = 8.0f / Tan(cameraCmp->GetFOV() / 2.0f);

		float verticalSpan = cameraHeight / 2.0f;
		float horizontalSpan = (cameraHeight * cameraCmp->GetAspect()) / 2.0f;
		float minW = gameMgrCmp->MinLevelWidth + horizontalSpan;
		float maxW = gameMgrCmp->MaxLevelWidth - horizontalSpan + 1;
		float minH = gameMgrCmp->MinLevelHeight + verticalSpan;
		float maxH = gameMgrCmp->MaxLevelHeight - verticalSpan + 1;
		TargetPosition.Y = Clamp(playerTransCmp->GetGlobalTranslation().Y, minH, maxH) + 0.5f;
		TargetPosition.X = Clamp(playerTransCmp->GetGlobalTranslation().X, minW, maxW) - 0.5f;

		// add lag to translation
		cameraCmp->SetTargetTranslation(TargetPosition);
		Vector Translation = Lerp(transformCmp->GetLocalTranslation(), cameraCmp->GetTargetTranslation(), 2.0*deltaTime);
		
		// add screen shake
		float timeSinceLastShake = time - cameraCmp->LastShakeTime;
		Vector Shake = Vector(0.0f, cameraCmp->ShakePower * -1.0f * Abs(sin(500.0f * time)), 0.0f);
		cameraCmp->ShakePower = Clamp((float)(cameraCmp->ShakePower - 0.2f * deltaTime), 0.0f, 0.1f);
		cameraCmp->ShakeOffset = Shake;
		// gConsole.LogInfo("SGJ::CameraMovementSystem::CameraMovementUpdatePhase ShakeOffset: {}", cameraCmp->ShakeOffset);
		
		// sum the camera modified translation
		transformCmp->SetLocalTranslation(Translation + cameraCmp->ShakeOffset);

		PostprocessSettingsComponent* post = cameraCmp->GetSibling<PostprocessSettingsComponent>();
		if (post != nullptr)
		{
			float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
			float intpart;
			float fractpart = std::modf(Time, &intpart);
			// post->UseCashetes = (fractpart > 0.5) ? 1 : 0;
			// gConsole.LogInfo("SGJ::CameraMovementSystem::CameraMovementUpdatePhase Time: {}, useCashetes: {}", Time, post->UseCashetes);
		}
	}

}

float SGJ::CameraMovementSystem::ElasticEaseOut(float p)
{
	return sin(-13 * 3.14 * (p + 1)) * pow(2, -10 * p) + 1;
}
