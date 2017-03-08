#ifndef COMPARE_ASSNS_T_HH
#define COMPARE_ASSNS_T_HH

#include <string>

namespace gdtest {
  struct A { int a; };
  struct B { double b; };
  struct D {
    D(std::string data = { }) : d(data) { }
    std::string d;
  };
  bool operator == (D const & left, D const & right)
  {
    return left.d == right.d;
  }
  bool operator != (D const & left, D const & right)
  {
    return !(left == right);
  }
}

#endif /* COMPARE_ASSNS_T_HH */
