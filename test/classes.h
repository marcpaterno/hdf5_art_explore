#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "compare_assns_t.hh"

template class art::Wrapper<art::Assns<gdtest::A, gdtest::B, void> >;
template class art::Wrapper<art::Assns<gdtest::A, gdtest::B, gdtest::D> >;
template class art::Wrapper<art::Assns<gdtest::B, gdtest::A, void> >;
template class art::Wrapper<art::Assns<gdtest::B, gdtest::A, gdtest::D> >;
