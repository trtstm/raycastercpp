#include <iostream>

constexpr double PI = 3.14159265358979323846;

class Angle {
public:
  enum class Quadrant {FIRST, FIRST_SECOND, SECOND, SECOND_THIRD, THIRD, THIRD_FOURTH, FOURTH, FOURTH_FIRST};
  
  constexpr Angle(double rad) : angle(Angle::capRad(rad)) {};
  
  constexpr static Angle rad(double rad)
  {
    return Angle(rad);
  }
  
  constexpr static Angle deg(double deg)
  {
    return Angle(Angle::toRad(deg));
  }

  static constexpr double toRad(double deg)
  {
    return deg * PI / 180.0;
  }
  
  static constexpr double toDeg(double rad)
  {
    return rad * 180.0 / PI;
  }

  static constexpr double capRad(double rad)
  {
    return (rad >= 0 && rad < 2 * PI) ? rad : (rad < 0 ? Angle::capRad(rad + 2 * PI) : Angle::capRad(rad - 2 * PI));
  }

  static double capDeg(double deg)
  {
    return Angle::toDeg(Angle::capRad(Angle::toRad(deg)));
  }

  Quadrant quadrant() const
  {
    auto a = Angle::toDeg(angle);
    if(a > 0 && a < 90) {
      return Quadrant::FIRST;
    }

    if(a > 90 && a < 180) {
      return Quadrant::SECOND;
    }

    if(a > 180 && a < 270) {
      return Quadrant::THIRD;
    }

    if(a > 270 && a < 360) {
      return Quadrant::FOURTH;
    }

    if(a == 0) {
      return Quadrant::FOURTH_FIRST;
    }
    
    if(a == 90) {
      return Quadrant::FIRST_SECOND;
    }
    
    if(a == 180) {
      return Quadrant::SECOND_THIRD;
    }
    
    if(a == 270) {
      return Quadrant::THIRD_FOURTH;
    }

  }

  
  constexpr double getRad() const
  {
    return angle;
  }

  constexpr double getDeg() const
  {
    return Angle::toDeg(angle);
  }
  
  constexpr Angle operator+(const Angle& o)
  {
    return Angle(angle + o.angle);
  }
  
  constexpr Angle operator-(const Angle& o)
  {
    return Angle(angle - o.angle);
  }
  
  Angle& operator+=(const Angle& o)
  {
    angle += o.angle;
    cap();
    return *this;
  }
  
  Angle& operator-=(const Angle& o)
  {
    angle -= o.angle;
    cap();
    return *this;
  }
  
  Angle& operator*=(double m)
  {
    angle *= m;
    cap();
    return *this;
  }
  
  Angle& operator/=(double m)
  {
    angle /= m;
    cap();
    return *this;
  }

  friend std::ostream& operator<<(std::ostream&, const Angle&);
  
private:
  void cap()
  {
    angle = Angle::capRad(angle);
  }
  
  double angle;
};

constexpr Angle operator-(const Angle& angle)
{
  return Angle::rad(-angle.getRad());
}

constexpr Angle operator*(const Angle& angle, double m)
{
  return Angle::rad(angle.getRad() * m);
}

constexpr Angle operator*(double m, const Angle& angle)
{
  return angle * m;
}

constexpr Angle operator/(const Angle& angle, double m)
{
  return Angle::rad(angle.getRad() / m);
}

constexpr Angle operator/(double m, const Angle& angle)
{
  return angle / m;
}

std::ostream& operator<<(std::ostream& os, const Angle& angle)
{
  os << Angle::toDeg(angle.angle) << " degrees";
  return os;
}

constexpr Angle operator "" _deg(long double deg)
{
  return Angle::deg(deg);
}

constexpr Angle operator "" _rad(long double rad)
{
  return Angle::rad(rad);
}

constexpr Angle operator "" _deg(unsigned long long deg)
{
  return Angle::deg(deg);
}

constexpr Angle operator "" _rad(unsigned long long rad)
{
  return Angle::rad(rad);
}
