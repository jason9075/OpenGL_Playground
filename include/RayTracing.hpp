#pragma once

class Ray {
 public:
  glm::vec3 origin{}, direction{};
  Ray(const glm::vec3 &o, const glm::vec3 &d) noexcept;
  [[nodiscard]] glm::vec3 at(float t) const noexcept;
};

class Interval {
 public:
  float minValue{}, maxValue{};
  Interval(float min, float max) noexcept;
  [[nodiscard]] float clamp(float x) const noexcept;
  [[nodiscard]] Interval expand(float delta) const noexcept;
};

class AABB {
 public:
  Interval x, y, z;

  AABB() noexcept;
  AABB(const Interval &x, const Interval &y, const Interval &z) noexcept;

  enum class Axis : int { X = 0, Y = 1, Z = 2 };
  [[nodiscard]] const Interval &axisInterval(int axis) const;
  [[nodiscard]] bool hit(const Ray &ray, Interval tInterval) const noexcept;
};

class HitRecord {
 public:
  float t{0.f};
  glm::vec3 p{0.f}, normal{0.f};
  HitRecord() noexcept = default;
  HitRecord(float t, const glm::vec3 &p, const glm::vec3 &n);
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
