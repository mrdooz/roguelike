#include "hotload_sprite.hpp"
#include "game.hpp"

using namespace rogue;
using namespace sf;


#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>


////////////////////////////////////////////////////////////
HotloadSprite::HotloadSprite() :
  m_lastTexture(NULL),
  m_textureRect()
{
}


////////////////////////////////////////////////////////////
HotloadSprite::HotloadSprite(const TextureHandle& texture) :
  m_lastTexture(NULL),
  m_texture(texture),
  m_textureRect()
{
  setTexture(texture);
}


////////////////////////////////////////////////////////////
HotloadSprite::HotloadSprite(const TextureHandle& texture, const IntRect& rectangle) :
  m_texture    (texture),
  m_textureRect()
{
  setTexture(texture);
  setTextureRect(rectangle);
}


////////////////////////////////////////////////////////////
void HotloadSprite::setTexture(const TextureHandle& texture, bool resetRect)
{
  m_texture = texture;
  m_lastTexture = TEXTURE_CACHE->TextureByHandle(m_texture);

  // Recompute the texture area if requested, or if there was no valid texture & rect before
  if (resetRect || (!m_lastTexture && (m_textureRect == sf::IntRect())))
    setTextureRect(IntRect(0, 0, m_lastTexture->getSize().x, m_lastTexture->getSize().y));
}


////////////////////////////////////////////////////////////
void HotloadSprite::setTextureRect(const IntRect& rectangle)
{
  if (rectangle != m_textureRect)
  {
    m_textureRect = rectangle;
    updatePositions();
    updateTexCoords();
  }
}


////////////////////////////////////////////////////////////
void HotloadSprite::setColor(const Color& color)
{
  // Update the vertices' color
  m_vertices[0].color = color;
  m_vertices[1].color = color;
  m_vertices[2].color = color;
  m_vertices[3].color = color;
}


////////////////////////////////////////////////////////////
TextureHandle HotloadSprite::getTexture() const
{
  return m_texture;
}


////////////////////////////////////////////////////////////
const IntRect& HotloadSprite::getTextureRect() const
{
  return m_textureRect;
}


////////////////////////////////////////////////////////////
const Color& HotloadSprite::getColor() const
{
  return m_vertices[0].color;
}


////////////////////////////////////////////////////////////
FloatRect HotloadSprite::getLocalBounds() const
{
  float width = static_cast<float>(std::abs(m_textureRect.width));
  float height = static_cast<float>(std::abs(m_textureRect.height));

  return FloatRect(0.f, 0.f, width, height);
}


////////////////////////////////////////////////////////////
FloatRect HotloadSprite::getGlobalBounds() const
{
  return getTransform().transformRect(getLocalBounds());
}


////////////////////////////////////////////////////////////
void HotloadSprite::draw(RenderTarget& target, RenderStates states) const
{
  // Check if the texture has been modified since the last draw call
  Texture* t = TEXTURE_CACHE->TextureByHandle(m_texture);
  if (t != m_lastTexture)
  {
    const_cast<HotloadSprite*>(this)->setTexture(m_texture);
    m_lastTexture = t;
  }

  if (m_lastTexture)
  {
    states.transform *= getTransform();
    states.texture = m_lastTexture;
    target.draw(m_vertices, 4, Quads, states);
  }
}


////////////////////////////////////////////////////////////
void HotloadSprite::updatePositions()
{
  FloatRect bounds = getLocalBounds();

  m_vertices[0].position = Vector2f(0, 0);
  m_vertices[1].position = Vector2f(0, bounds.height);
  m_vertices[2].position = Vector2f(bounds.width, bounds.height);
  m_vertices[3].position = Vector2f(bounds.width, 0);
}


////////////////////////////////////////////////////////////
void HotloadSprite::updateTexCoords()
{
  float left   = static_cast<float>(m_textureRect.left);
  float right  = left + m_textureRect.width;
  float top    = static_cast<float>(m_textureRect.top);
  float bottom = top + m_textureRect.height;

  m_vertices[0].texCoords = Vector2f(left, top);
  m_vertices[1].texCoords = Vector2f(left, bottom);
  m_vertices[2].texCoords = Vector2f(right, bottom);
  m_vertices[3].texCoords = Vector2f(right, top);
}
