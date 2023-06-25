#include "objects/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class model
{
public:
    model(const char* path);

    std::vector<mesh>& get_meshes();
    figure*            get_figure();

private:
    std::vector<mesh> meshes;

    void load_model(const char* path);
    void process_node(aiNode* node, const aiScene* scene);
    mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    // std::vector<Texture> load_material_textures(aiMaterial*   mat,
    //                                             aiTextureType type,
    //                                             string        typeName);
};