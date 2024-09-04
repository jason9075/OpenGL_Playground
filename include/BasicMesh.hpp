#pragma once

#include "DataObject.hpp"

std::unique_ptr<Mesh> createCuboidMesh(float width = 1.0f, float height = 1.0f, float depth = 1.0f, float x = 0.0f,
                                       float y = 0.0f, float z = 0.0f, float r = 0.5f, float g = 0.5f, float b = 0.5f);

std::unique_ptr<Mesh> createCubeMesh(float scale = 1.0f);

std::unique_ptr<Mesh> createPlaneMesh(const float scale = 1.0f, const glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f));

std::unique_ptr<Mesh> createFrameMesh();
