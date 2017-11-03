#include "WorldObject.h"



void WorldObject::draw(Camera& camera, vec3& lightLocation )
{
	update();
	mesh->render(camera, lightLocation);
}

void WorldObject::update()
{
	if (hasGravity) 
	{
		if (directionOfTravel.y > 0)
		{
			speedOfTravel -= 0.001;
		}
		worldLocation.y += speedOfTravel*directionOfTravel.y;
	}
	mesh->worldPos = worldLocation;
	mesh->worldRot = worldRotation;
	mesh->worldScale = worldScale;
	if (worldLocation.y < -5.0f)
	{
		directionOfTravel = vec3(0.0f, 1.0f, 0.0);
	}
}

void WorldObject::init(Mesh& meshAd)
{
	hasGravity = true;
	directionOfTravel = vec3(0.0f, -1.0f, 0.0f);
	mesh = &meshAd;
}
