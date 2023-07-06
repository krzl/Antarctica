#pragma once
#include "GameObjects/GameObject.h"

class QuadtreeTester : public GameObject
{
public:

	QuadtreeTester();

protected:

	void Tick(float deltaTime) override;

public:

	DEFINE_CLASS()
};


CREATE_CLASS(QuadtreeTester)
