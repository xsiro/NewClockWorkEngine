
#ifndef __MODULE_REVERBZONE__
#define __MODULE_REVERBZONE__

#include "Component.h"

class ModuleReverbZone :public Component
{
public:
	ModuleReverbZone(GameObject* owner, unsigned int ID);
	~ModuleReverbZone();
	void OnEditor();
	bool GameUpdate(float gameDT)override;
	bool GameInit()override;
	bool Update(float dt);

	void SetReverbZone(float3 dimensions);
	void UpdateReverbZoneDimension();
	void GetAABBPoints(AABB& aabb, std::vector<float3>& emptyVector);
	bool DoesReverbZoneContainPoint(float3 point) const;
	float3 GetDimensions()const;
	std::string targetBus; 
	float revValue;

private:
	float3 dimensions;
	AABB revZone;
};

#endif // !__MODULE_REVERBZONE__
