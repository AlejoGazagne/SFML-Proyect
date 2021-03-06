#ifndef TESTSFML_MAPATMX_H
#define TESTSFML_MAPATMX_H

#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/TileLayer.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <stack>
#include "LinkedList.h"
#include "enemigos.h"
#include "Vida.h"

using namespace std;

class MapaTMX {
private:
    tmx::Map map;
    sf::Texture textura;
    LinkedList<sf::Sprite *> sprites;
    LinkedList<sf::Rect<float> *> list;
    sf::Vector2u tile_size;
    uint32_t columns;
    Personaje *player;
    Enemigos *enemy;
    Coin *monei;
    Vida *vi;

public:

    explicit MapaTMX(const string &archivo, sf::Texture &pl_tx, LinkedList<Enemigos *> *enemies, queue<Coin *> *moneda) {
        if (!map.load(archivo))
            throw "Cannot open map";

        // Me guardo el tamaño de los tiles
        tile_size.x = map.getTileSize().x;
        tile_size.y = map.getTileSize().y;

        // Me traigo el Tileset que voy a usar y cargo la imagen y veo cuantas columnas tiene.
        const auto &tileSet = map.getTilesets();
        textura.loadFromFile(tileSet[0].getImagePath());
        columns = tileSet[0].getColumnCount();

        // Me traigo las capas
        const auto &layers = map.getLayers();

        for (int i = 0; i < layers.size(); ++i) { // recorro las capas

            // Capa de mosaicos
            if (layers[i]->getType() == tmx::Layer::Type::Tile) {
                const auto &tileLayer = layers[i]->getLayerAs<tmx::TileLayer>();
                const auto &tiles = tileLayer.getTiles(); // Me traigo todos los mosaicos

                int col = 0;
                int fila = 0;
                for (int j = 0; j < tiles.size(); ++j) {
                    if (tiles[j].ID != 0) {
                        sprites.push_front(new sf::Sprite(textura)); // Agrego un nuevo sprite a la lista
                        sf::Sprite *sp = sprites.get(0);
                        int tx_fila = (tiles[j].ID - 1) / columns;
                        int tx_col = (tiles[j].ID - 1) % columns;
                        sp->setTextureRect({tx_col * (int) tile_size.x, tx_fila * (int) tile_size.x, (int) tile_size.x,(int) tile_size.x});
                        sp->setPosition(col * tile_size.x, fila * tile_size.x);
                    }

                    if (++col == map.getTileCount().x) {
                        col = 0;
                        fila++;
                        std::cout << std::endl;
                    }
                }
                //capa de Objetos
            }else if (layers[i]->getType() == tmx::Layer::Type::Object) {
                const auto &objectLayer = layers[i]->getLayerAs<tmx::ObjectGroup>();
                const auto &objects = objectLayer.getObjects();
                for (int j = 0; j < objects.size(); ++j) {
                    if(objects[j].getName() == "Pared"){
                        list.push_front(new sf::Rect<float> (objects[j].getAABB().left, objects[j].getAABB().top, objects[j].getAABB().width, objects[j].getAABB().height));
                    }
                    if(objects[j].getName() == "Enemigo"){
                        auto *enemy = new Enemigos();
                        enemy->getSprite().setPosition(objects[j].getPosition().x, objects[j].getPosition().y);
                        enemy->getSprite().setOrigin((float) enemy->getTexture().getSize().x / 2,
                                                     (float) enemy->getTexture().getSize().y / 2);
                        enemies->push_front(enemy);
                    }
                    if (objects[j].getName() == "player") {
                        player = new Personaje(objects[j].getPosition().x, objects[j].getPosition().y,pl_tx);
                    }
                    if (objects[j].getName() == "Coin") {
                        auto *monei = new Coin(objects[j].getPosition().x, objects[j].getPosition().y);
                        moneda->push(monei);
                    }
                }
            }
        }
    }

    void dibujar(sf::RenderWindow &w) {
        for (int i = sprites.getSize() - 1; i >= 0; --i) {
            w.draw(*sprites.get(i));
        }
    }

    const LinkedList<sf::Rect<float> *> &getList() const {
        return list;
    }

    Personaje *getPlayer() {
        return player;
    }

};

#endif //TESTSFML_MAPATMX_H