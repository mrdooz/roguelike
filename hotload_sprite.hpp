#pragma once
#include "texture_handle.hpp"

namespace rogue
{
  // Code ripped from sf::Sprite, and comments removed from brievity

  class Texture;

  class HotloadSprite : public sf::Drawable, public sf::Transformable
  {
  public :
    HotloadSprite();
    explicit HotloadSprite(const TextureHandle& texture);
    HotloadSprite(const TextureHandle& texture, const IntRect& rectangle);
    void setTexture(const TextureHandle& texture, bool resetRect = false);
    void setTextureRect(const IntRect& rectangle);
    void setColor(const Color& color);
    TextureHandle getTexture() const;
    const IntRect& getTextureRect() const;
    const Color& getColor() const;

    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;

  private :

    virtual void draw(RenderTarget& target, sf::RenderStates states) const;
    void updatePositions();
    void updateTexCoords();

    sf::Vertex          m_vertices[4]; ///< Vertices defining the sprite's geometry
    TextureHandle       m_texture;
    sf::IntRect         m_textureRect; ///< Rectangle defining the area of the source texture to display
    mutable Texture*    m_lastTexture;
  };
}