#pragma once

#include "DataObject.hpp"

std::unique_ptr<Mesh> createCuboidMesh(float width = 1.0f, float height = 1.0f, float depth = 1.0f,
                                       glm::vec3 pos = glm::vec3(0.0f), glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));

std::unique_ptr<Mesh> createCubeMesh(float scale = 1.0f, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));

std::unique_ptr<Mesh> createPlaneMesh(const float scale = 1.0f, const glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f),
                                      const glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f),
                                      const glm::vec3 center = glm::vec3(0.0f));

std::unique_ptr<Mesh> createFrameMesh(float width, float height, float depth, float thickness, glm::vec3 position,
                                      glm::vec3 color);

std::unique_ptr<Mesh> createArrowMesh(float shaftWidth, float shaftHeight, float headWidth, float headHeight,
                                      float thickness, glm::vec3 position = glm::vec3(0.0f),
                                      glm::vec3 color = glm::vec3(1.0f));
