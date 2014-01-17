#pragma once

namespace rogue
{
  struct Tile;
  class Entity;

  enum class Selection
  {
    None      = 0,
    Empty     = 1 << 0,
    Monster   = 1 << 1,
    Player    = 1 << 2,
    Any       = (int)Empty | (int)Monster | (int)Player,
  };

  struct SelectionEvent
  {
    SelectionEvent();
    SelectionEvent(const Tile* tile);
    SelectionEvent(const Tile* tile, const intrusive_ptr<Entity>& entity);
    const Tile* _tile;
    intrusive_ptr<Entity> _entity;
  };
}
