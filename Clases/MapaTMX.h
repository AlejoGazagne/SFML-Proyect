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

using namespace std;

class MapaTMX {
private:
    tmx::Map map;
    sf::Texture textura;
    stack<sf::Vector2<float> *> enemigos;
    LinkedList<sf::Sprite *> sprites;
    LinkedList<sf::Rect<float> *> list;
    sf::Vector2u tile_size;
    uint32_t columns;
    Personaje *player;


public:

    explicit MapaTMX(const string &archivo, sf::Texture &pl_tx) {
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
                    //cout << objects[j].getName() << " ";
                    //cout << objects[j].getPosition().x << " " << objects[j].getPosition().y <<endl;

                    if(objects[j].getName() == "Pared"){
                        //sf::Rect<int> paredes(objects[j].getAABB().left, objects[j].getAABB().top, objects[j].getAABB().width, objects[j].getAABB().height);
                        list.push_front(new sf::Rect<float> (objects[j].getAABB().left, objects[j].getAABB().top, objects[j].getAABB().width, objects[j].getAABB().height));
                    }
                    if(objects[j].getName() == "Enemy"){
                        enemigos.push(new sf::Vector2<float> (objects[j].getPosition().x, objects[j].getPosition().y));
                    }

                    if (objects[j].getName() == "player") {
                        player = new Personaje(objects[j].getPosition().x, objects[j].getPosition().y,pl_tx);
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

    const stack<sf::Vector2<float> *> &getEnemigos() const {
        return enemigos;
    }

    Personaje *getPlayer() {
        return player;
    }
};

#endif //TESTSFML_MAPATMX_H