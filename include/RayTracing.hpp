#pragma once

class Ray {
 public:
  glm::vec3 origin, direction;
  Ray(const glm::vec3 &origin, const glm::vec3 &direction);
  glm::vec3 at(float t) const;
};

class Interval {
 public:
  float minValue, maxValue;
  Interval(float min, float max);
  float clamp(float x);
  Interval expand(float delta);
};

class AABB {
 public:
  Interval x, y, z;

  AABB();
  AABB(const Interval &x, const Interval &y, const Interval &z);

  const Interval &axisInterval(int axis) const;

  bool hit(const Ray &ray, Interval tInterval) const;
};

class HitRecord {
 public:
  float t;
  glm::vec3 p, normal;
  HitRecord();
  HitRecord(float t, const glm::vec3 &p, const glm::vec3 &normal);
};

class HitTable {
 public:
  virtual ~HitTable() = default;
  virtual bool hit(const Ray &ray, Interval tInterval, HitRecord &record) const = 0;
  virtual AABB boundingBox() const = 0;
};

class Sphere : public HitTable {
 public:
  glm::vec3 center;
  float radius;

  Sphere(const glm::vec3 &center, float radius);
  bool hit(const Ray &ray, Interval tInterval, HitRecord &record) const override;
  AABB boundingBox() const override;
};

class HitTableList : public HitTable {
 public:
  std::vector<HitTable *> hitTables;

  // HitTableList();
  // HitTableList(std::vector<HitTable *> hitTables);
  ~HitTableList() override;
  void add(HitTable *hitTable);
  bool hit(const Ray &ray, Interval tInterval, HitRecord &record) const override;
  AABB boundingBox() const override;
};
