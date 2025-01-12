// Tilemap.h
///////////////////////////////////////////
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    bool load(  const std::filesystem::path& tileset, /* texture/tileset filepath */
                sf::Vector2u    tileSize, /* tileSize in the texture*/
                const uint16_t* tiles,    /* indexes of tiles in the texture */
                uint16_t        width,    /* map width */
                uint16_t        height    /* map height */)
    {
        // update member data
        m_tileSize = tileSize;
        m_width = width;
        // Open texture file
        if (!m_tileset.loadFromFile(tileset))
            return false;
        
        // setup vertex array
        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_vertices.resize(width * height * 6);

        for (uint16_t i = 0; i < height; ++i)
        {
            for (uint16_t j = 0; j < width; ++j)
            {

                uint16_t tileNumber = tiles[j + i * width];
                updateTile(i, j, tileNumber);
            }
        }
        return true;
    }

    /**
     * @brief 
     * 
     * @param i row
     * @param j col
     * @param tileNumber newTileIndex on the tileset
     */
    void updateTile(uint16_t i, uint16_t j, uint16_t tileNumber)
    {
        // tile's pos on tileset
        uint16_t tu = (tileNumber % (m_tileset.getSize().x / m_tileSize.x)) * m_tileSize.x;
        uint16_t tv = (tileNumber / (m_tileset.getSize().x / m_tileSize.x)) * m_tileSize.y;
        // uint16_t tu = tileNumber * m_tileSize.x;
        // uint16_t tv = 0;
        // the commented code work well if tileNumber is 100% in range of [0, tileSize - 1]
        // but the uncommented code makes it work even if any tileNubmber exceed the range

        // vertices of current tile
        sf::Vertex* triangles = &m_vertices[(j + i * m_width) * 6];

        // define current tile's vertices position
        triangles[0].position = sf::Vector2f{j * m_tileSize.x, i * m_tileSize.y}; // top-left
        triangles[1].position = sf::Vector2f{(j+1) * m_tileSize.x, i * m_tileSize.y}; // top-right
        triangles[2].position = sf::Vector2f{j * m_tileSize.x, (i+1) * m_tileSize.y}; // bottom-left
        triangles[3].position = sf::Vector2f{j * m_tileSize.x, (i+1) * m_tileSize.y}; // bottom-left
        triangles[4].position = sf::Vector2f{(j+1) * m_tileSize.x, (i+1) * m_tileSize.y}; // bottom-right
        triangles[5].position = sf::Vector2f{(j+1) * m_tileSize.x, i * m_tileSize.y}; // top-right

        // define the matching texture coordinates
        triangles[0].texCoords = sf::Vector2f{tu, tv}; // top-left
        triangles[1].texCoords = sf::Vector2f{tu + m_tileSize.x, tv}; // top-right
        triangles[2].texCoords = sf::Vector2f{tu, tv + m_tileSize.y}; // bottom-left
        triangles[3].texCoords = sf::Vector2f{tu, tv + m_tileSize.y}; // bottom-left
        triangles[4].texCoords = sf::Vector2f{tu + m_tileSize.x, tv + m_tileSize.y}; // bottom-right
        triangles[5].texCoords = sf::Vector2f{tu + m_tileSize.x, tv}; // top-right
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // apply the transform
        states.transform = getTransform();
        // apply the texture
        states.texture = &m_tileset;
        // draw the vertex array
        target.draw(m_vertices, states);
    }


    sf::VertexArray m_vertices;
    sf::Texture     m_tileset;
    sf::Vector2u    m_tileSize;
    uint16_t        m_width;
};