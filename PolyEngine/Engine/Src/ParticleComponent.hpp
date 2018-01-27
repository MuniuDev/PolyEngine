#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
	public:
		ParticleComponent(int size);
		virtual ~ParticleComponent();

		ParticleEmitter* Emitter;

	private:
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}