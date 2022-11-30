#pragma once
#include <cassert>
#include <string>

namespace t9::peg {

/// @cond private
namespace peg_ {

// Nil.
struct Nil {};

// Seq.
template <typename T, typename... Ts>
struct Seq {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    return T::is_match(iter, ctx) && Seq<Ts...>::is_match(iter, ctx);
  }
};
template <>
struct Seq<Nil> {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter&, Ctx&) {
    return true;
  }
};

// Or.
template <typename T, typename... Ts>
struct Or {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    auto copy = iter;
    if (T::is_match(iter, ctx)) return true;
    iter = copy;
    return Or<Ts...>::is_match(iter, ctx);
  }
};
template <>
struct Or<Nil> {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter&, Ctx&) {
    return false;
  }
};

}  // namespace peg_
/// @endcond

/// @brief イテレータ
struct Iterator {
  const char* begin = nullptr;
  const char* end = nullptr;

  const char* next() {
    if (begin == end) return nullptr;
    return begin++;
  }
  char front() const {
    assert(!is_empty());
    return *begin;
  }
  bool is_empty() const { return begin == end; }
};

/// @brief アクション
template <typename Exp, typename T>
struct Action {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    auto copy = iter;
    if (!Exp::is_match(iter, ctx)) return false;
    std::string_view str(copy.begin, iter.begin - copy.begin);
    T::on_match(str, ctx);
    return true;
  }
};

/// @brief 合致判定
template <typename Exp>
inline bool is_match(std::string_view str) {
  Iterator iter{str.data(), str.data() + str.length()};
  peg_::Nil ctx;
  return Exp::is_match(iter, ctx) && iter.is_empty();
}

/// @brief パース
template <typename Exp, typename Ctx>
inline bool parse(std::string_view str, Ctx& ctx) {
  Iterator iter{str.data(), str.data() + str.length()};
  return Exp::is_match(iter, ctx) && iter.is_empty();
}

/// @brief 文字合致 [C]
template <char C>
struct Char {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx&) {
    if (iter.is_empty()) return false;
    if (iter.front() != C) return false;
    iter.next();
    return true;
  }
};

/// @brief 文字合致 [BeginC, EndC]
template <char BeginC, char EndC>
struct CharRange {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx&) {
    if (iter.is_empty()) return false;
    if (iter.front() < BeginC || EndC < iter.front()) return false;
    iter.next();
    return true;
  }
};

/// @brief 並び
template <typename... Exp>
struct Seq {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    auto copy = iter;
    if (peg_::Seq<Exp..., peg_::Nil>::is_match(iter, ctx)) return true;
    iter = copy;
    return false;
  }
};

/// @brief 選択
template <typename... Exp>
struct Or {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    return peg_::Or<Exp..., peg_::Nil>::is_match(iter, ctx);
  }
};

/// @brief 0個以上
template <typename Exp>
struct Star {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    while (Exp::is_match(iter, ctx)) {
    }
    return true;
  }
};

/// @brief 1個以上
template <typename Exp>
struct Plus {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    if (!Exp::is_match(iter, ctx)) return false;
    while (Exp::is_match(iter, ctx)) {
    }
    return true;
  }
};

/// @brief 省略可能
template <typename Exp>
struct Opt {
  template <typename Iter, typename Ctx>
  static bool is_match(Iter& iter, Ctx& ctx) {
    Exp::is_match(iter, ctx);
    return true;
  }
};

template <char... C>
using Text = Seq<Char<C>...>;
using Blank = Or<Char<' '>, Char<'\t'>>;
using Eol = Or<Char<'\r'>, Char<'\n'>>;
using Space = Or<Blank, Eol>;
using Digit = CharRange<'0', '9'>;
using Lower = CharRange<'a', 'z'>;
using Upper = CharRange<'A', 'Z'>;
using Alpha = Or<Lower, Upper>;
using Alnum = Or<Alpha, Digit>;

}  // namespace t9::peg