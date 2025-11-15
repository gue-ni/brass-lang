# Brass Lang

![build workflow status](https://github.com/gue-ni/brass-lang/actions/workflows/build.yml/badge.svg)

_Brass_ is a statically-typed, interpreted programming language.

## Example

````
class Square {
  w: int;
  h: int;
}

var sq = Square();

sq.width  = 2;
sq.height = 3;

fn area(s: Square) : int {
  return s.width * s.height;
}

print area(sq);
```
