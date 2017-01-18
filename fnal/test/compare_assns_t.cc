#include "compare.hh"
#include "compare_assns_t.hh"
#include "canvas/Persistency/Common/Ptr.h"

#include <cassert>
#include <vector>

#include "TError.h"

using namespace art;

int main() {
  using namespace gdtest;
  using namespace std::string_literals;

  Assns<A, B> assns1;
  Assns<A, B, D> assnsD1;
  std::vector<A> va { { 1 }, { 2 }, { 3 }, { 5 }, { 7 },
                      { 11 }, { 13 }, { 17 }, { 23 }, { 29 },
                      { 31 }, { 37 }, { 41 } };
  std::vector<B> vb { { 2.2 }, { 4.2 }, { 6.2 }, { 8.2 }, { 10.2 },
                      { 12.2 }, { 14.2 }, { 16.2 }, { 18.2 }, { 20.2 },
                      { 22.2 }, { 24.2 }, { 26.2 } };
  std::vector<D> vd { { "a"s }, { "b"s }, { "c"s }, { "d"s }, { "e"s }, { "f"s }, { "g"s } };

  for (auto i = 0UL, sz = va.size(), szd = vd.size(); i != sz; ++i) {
    Ptr<A> p1({ static_cast<unsigned short>(i / 3 + 1), 1 }, &va[i], i);
    Ptr<B> p2({ static_cast<unsigned short>(i / 3 + 11), 1 }, &vb[i], i);
    assns1.addSingle(p1, p2);
    assnsD1.addSingle(p1, p2, vd[i % szd]);
  }
  auto assns2 = assns1;
  auto assnsD2 = assnsD1;
  // Equality works.
  assert(assns1 == assns2);
  assert(assnsD1 == assnsD2);
  // Inequality due to size.
  assns2.addSingle({}, {});
  assert(assns1 != assns2);
  assnsD2.addSingle({}, {}, {});
  assert(assnsD1 != assnsD2);
  // Inequality due to member value.
  assns2 = assns1;
  assert(assns1 == assns2);
  const_cast<Ptr<A>&>(assns2[assns2.size() -1].first) = Ptr<A> { };
  assert(assns1 != assns2);
  assnsD2 = assnsD1;
  assert(assnsD1 == assnsD2);
  const_cast<Ptr<A>&>(assnsD2[assnsD2.size() -1].first) = Ptr<A> { };
  assert(assnsD1 != assnsD2);
  assnsD2 = assnsD1;
  assert(assnsD1 == assnsD2);
  const_cast<D &>(assnsD2.data(assnsD2.size() - 1)) = {};
}
