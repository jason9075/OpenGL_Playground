#include "RayTracing.hpp"

#include <OPPCH.h>

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction) noexcept
    : origin(origin), direction(glm::normalize(direction)) {}

glm::vec3 Ray::at(float t) const noexcept { return origin + t * direction; }

Interval::Interval(float min, float max) noexcept {
  if (min > max) {
    minValue = max;
    maxValue = min;
    return;
  }
  minValue = min;
  maxValue = max;
}

float Interval::clamp(float x) const noexcept { return glm::clamp(x, minValue, maxValue); }

Interval Interval::expand(float delta) const noexcept {
  auto padding = delta * 0.5f;
  return Interval(minValue - padding, maxValue + padding);
}

AABB::AABB() noexcept : x(Interval(0, 0)), y(Interval(0, 0)), z(Interval(0, 0)) {}

AABB::AABB(const Interval &x, const Interval &y, const Interval &z) noexcept : x(x), y(y), z(z) {}

const Interval &AABB::axisInterval(int axis) const {
  switch (axis) {
    case 0:
      return x;
    case 1:
      return y;
    case 2:
      return z;
    default:
      throw std::out_of_range("AABB axis out of range");
  }
}

bool AABB::hit(const Ray &ray, Interval tInterval) const noexcept {
  const glm::vec3 &origin = ray.origin;
  const glm::vec3 &direction = ray.direction;

  for (int axis = 0; axis < 3; axis++) {
    const float invD = 1.0f / direction[axis];
    const Interval &ax = axisInterval(axis);
    float t0 = (ax.minValue - origin[axis]) * invD;
    float t1 = (ax.maxValue - origin[axis]) * invD;

    if (invD < 0.0f) {
      std::swap(t0, t1);
    }

    tInterval.minValue = t0 > tInterval.minValue ? t0 : tInterval.minValue;
    tInterval.maxValue = t1 < tInterval.maxValue ? t1 : tInterval.maxValue;

    if (tInterval.minValue >= tInterval.maxValue) {
      return false;
    }
  }

  return true;
}

HitRecord::HitRecord(float t, const glm::vec3 &p, const glm::vec3 &n) : t(t), p(p), normal(n) {}

Sphere::Sphere(const glm::vec3 &center, float radius) : center(center), radius(radius) {}
bool Sphere::hit(const Ray &ray, Interval tInterval, HitRecord &record) const {
  glm::vec3 oc = ray.origin - center;
  float a = glm::dot(ray.direction, ray.direction);
  float b = 2.0f * glm::dot(oc, ray.direction);
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return false;
  }

  float sqrtD = glm::sqrt(discriminant);
  float t = (-b - sqrtD) / (2.0f * a);
  if (t < tInterval.minValue || t > tInterval.maxValue) {
    t = (-b + sqrtD) / (2.0f * a);
    if (t < tInterval.minValue || t > tInterval.maxValue) {
      return false;
    }
  }

  record.t = t;
  record.p = ray.at(t);
  record.normal = (record.p - center) / radius;
  return true;
}

AABB Sphere::boundingBox() const {
  return AABB(Interval(center.x - radius, center.x + radius), Interval(center.y - radius, center.y + radius),
              Interval(center.z - radius, center.z + radius));
}

// HitTableList::HitTableList() : hitTables(std::vector<HitTable *>()) {}
// HitTableList::HitTableList(std::vector<HitTable *> hitTables) : hitTables(hitTables) {}
void HitTableList::add(HitTable *hitTable) { hitTables.push_back(hitTable); }
bool HitTableList::hit(const Ray &ray, Interval tInterval, HitRecord &record) const {
  HitRecord tempRecord;
  bool hitAnything = false;
  float closest = tInterval.maxValue;

  for (const auto &hitTable : hitTables) {
    if (hitTable->hit(ray, tInterval, tempRecord)) {
      hitAnything = true;
      if (tempRecord.t < closest) {
        closest = tempRecord.t;
        record = tempRecord;
      }
    }
  }

  return hitAnything;
}
