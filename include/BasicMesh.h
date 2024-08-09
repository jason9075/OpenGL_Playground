#pragma once

#include "DataObject.h"

std::unique_ptr<Mesh> createCubeMesh(float scale = 1.0f);

std::unique_ptr<Mesh> createPlaneMesh();
