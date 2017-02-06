#ifndef RANDAR_RESOURCE_RESOURCE_REPOSITORY_HPP
#define RANDAR_RESOURCE_RESOURCE_REPOSITORY_HPP

#include <cstdint>
#include <randar/Filesystem/Directory.hpp>
#include <randar/Project/Importer.hpp>
#include <randar/Utility/Map.hpp>

namespace randar
{
    class Texture;
    class Model;

    class ResourceRepository
    {
        /**
         * The location of this repository on disk.
         */
        Directory directory;

        /**
         * Resources in this repository.
         */
        std::map<uint32_t, Model*> models;
        std::map<uint32_t, Texture*> textures;

    public:
        /**
         * Destructor.
         */
        ~ResourceRepository();

        /**
         * Frees this repository from memory.
         */
        void clear();

        /**
         * Loads a repository from disk.
         *
         * An exception is thrown upon error.
         */
        void load(const Directory& directory);

        /**
         * Saves this repository to disk.
         *
         * An exception is thrown upon error.
         */
        void save();
        void save(const Directory& directory);

        /**
         * Retrieves and adds a model.
         */
        Model* getModel(uint32_t id);
        uint32_t addModel(Model* model);

        /**
         * Retrieves and adds a texture.
         */
        Texture* getTexture(uint32_t id);
        uint32_t addTexture(Texture* texture);

        /**
         * Exporting methods.
         */
        void exportRaTexture(uint32_t textureId);
    };
}

#endif
