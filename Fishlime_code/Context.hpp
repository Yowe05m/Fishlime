#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"

struct Context {
    sf::RenderWindow* window = nullptr;
    ResourceManager<sf::Texture>* textures = nullptr;
    ResourceManager<sf::Font>* fonts = nullptr;
};
