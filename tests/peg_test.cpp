#include <doctest/doctest.h>
#include <t9/peg.h>

#include <sstream>
#include <stack>

using namespace t9::peg;

TEST_CASE("PEG match char") {
  using Exp = Char<'A'>;
  CHECK(is_match<Exp>("A"));
  CHECK_FALSE(is_match<Exp>("B"));
}

TEST_CASE("PEG match text") {
  using Exp = Text<'H', 'e', 'l', 'l', 'o'>;
  CHECK_FALSE(is_match<Exp>(""));
  CHECK_FALSE(is_match<Exp>("H"));
  CHECK_FALSE(is_match<Exp>("He"));
  CHECK_FALSE(is_match<Exp>("Hel"));
  CHECK_FALSE(is_match<Exp>("Hell"));
  CHECK(is_match<Exp>("Hello"));
  CHECK_FALSE(is_match<Exp>("Hello, World"));
}

namespace {

struct CalcContext {
  enum class Op {
    Add,
    Sub,
    Mul,
    Div,
  };
  std::stack<int> ints;
  std::stack<Op> ops;

  int pop_answer() {
    int answer = ints.top();
    ints.pop();
    return answer;
  }
};

struct PushInt {
  static void on_match(std::string_view str, CalcContext& ctx) {
    std::string s(str);
    int i;
    std::istringstream iss(s);
    iss >> i;
    ctx.ints.push(i);
  }
};
struct PushOp {
  static void on_match(std::string_view str, CalcContext& ctx) {
    if (str == "+") {
      ctx.ops.push(CalcContext::Op::Add);
    } else if (str == "-") {
      ctx.ops.push(CalcContext::Op::Sub);
    } else if (str == "*") {
      ctx.ops.push(CalcContext::Op::Mul);
    } else if (str == "/") {
      ctx.ops.push(CalcContext::Op::Div);
    }
  }
};
struct Calc {
  static void on_match(std::string_view, CalcContext& ctx) {
    auto op = ctx.ops.top();
    ctx.ops.pop();

    auto rhs = ctx.ints.top();
    ctx.ints.pop();
    auto lhs = ctx.ints.top();
    ctx.ints.pop();

    switch (op) {
      case CalcContext::Op::Add:
        ctx.ints.push(lhs + rhs);
        break;
      case CalcContext::Op::Sub:
        ctx.ints.push(lhs - rhs);
        break;
      case CalcContext::Op::Mul:
        ctx.ints.push(lhs * rhs);
        break;
      case CalcContext::Op::Div:
        ctx.ints.push(lhs / rhs);
        break;
    }
  }
};

struct Value;
using Number  // [0-9]+
    = Action<Plus<Digit>, PushInt>;
using ProductOp  // ('*' / '/')
    = Action<Or<Char<'*'>, Char<'/'>>, PushOp>;
using SumOp  // ('+' / '-')
    = Action<Or<Char<'+'>, Char<'-'>>, PushOp>;
using Product  // Value (ProductOp Value)*
    = Seq<Value, Star<Action<Seq<ProductOp, Value>, Calc>>>;
using Sum  // Product (SumOp Product)*
    = Seq<Product, Star<Action<Seq<SumOp, Product>, Calc>>>;
struct Value  // Number / '(' Sum ')'
    : Or<Number, Seq<Char<'('>, Sum, Char<')'>>> {};

}  // namespace

TEST_CASE("PEG calc") {
  using Exp = Sum;

  CalcContext ctx;

  CHECK(parse<Exp>("1", ctx));
  CHECK_EQ(1, ctx.pop_answer());

  CHECK(parse<Exp>("1+2", ctx));
  CHECK_EQ(3, ctx.pop_answer());

  CHECK(parse<Exp>("3*(1+2)", ctx));
  CHECK_EQ(9, ctx.pop_answer());

  CHECK(parse<Exp>("5*2*3+21/3-12*3", ctx));
  CHECK_EQ(1, ctx.pop_answer());

  CHECK_FALSE(parse<Exp>("2+-", ctx));
}