#include <randar/render/color.hpp>

randar::Color::Color()
: randar::Color::Color(1.0f, 1.0f, 1.0f)
{

}

randar::Color::Color(float rInit, float gInit, float bInit, float aInit)
: r(rInit),
  g(gInit),
  b(bInit),
  a(aInit)
{

}
