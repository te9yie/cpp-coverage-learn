#include <t9/counter.h>

namespace t9 {

int Counter::inc() {
  ++count_;
  return count_;
}

}  // namespace t9