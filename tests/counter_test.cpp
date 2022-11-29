#include <doctest/doctest.h>
#include <t9/counter.h>

TEST_CASE("Counter::get") {
  t9::Counter counter;
  CHECK(counter.count() == 0);
  CHECK(counter.count() == 0);
}

TEST_CASE("Counter::inc") {
  t9::Counter counter;
  CHECK(counter.count() == 0);
  CHECK(counter.inc() == 1);
  CHECK(counter.count() == 1);
  CHECK(counter.inc() == 2);
  CHECK(counter.count() == 2);
}