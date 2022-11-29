#pragma once

namespace t9 {

class Counter {
private:
  int count_ = 0;

public:
  int inc();
  int count() const { return count_; }
};

} // namespace t9