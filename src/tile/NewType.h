
#ifndef TILE_NEWTYPE_H
#define TILE_NEWTYPE_H

namespace Tile{
  template <typename T, typename Y>
  struct NewType{
      T value;
  };

#define TILE_NEWTYPE(X,Y) \
  class NewType ## Y{}; \
  typedef Tile::NewType<X, NewType ## Y> Y;

}

#endif
