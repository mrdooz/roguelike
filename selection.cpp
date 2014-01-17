#include "selection.hpp"
#include "entity.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
SelectionEvent::SelectionEvent()
    : _tile(nullptr),
    _entity(nullptr)
{
}

//-----------------------------------------------------------------------------
SelectionEvent::SelectionEvent(const Tile* tile)
    : _tile(tile)
    , _entity(nullptr)
{
}

//-----------------------------------------------------------------------------
SelectionEvent::SelectionEvent(const Tile* tile, const intrusive_ptr<Entity>& entity)
    : _tile(tile)
    , _entity(entity)
{
}
