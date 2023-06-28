#include "model.h"
#include <SDL3/SDL.h>
#include <stdexcept>

model::model(const char* path)
{
    load_model(path);
}

std::vector<mesh>& model::get_meshes()
{
    return meshes;
}

figure* model::get_figure()
{
    figure* fig = new figure();
    for (auto mesh : meshes)
    {
        fig->add_figure(mesh);
    }
    return fig;
}

void model::load_model(const char* path)
{
    Assimp::Importer import;
    // const aiScene*   scene =
    //     import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    membuff*       file = load_file_to_memory(path);
    const aiScene* scene =
        import.ReadFileFromMemory(static_cast<void*>(file->ptr.get()),
                                  file->size,
                                  aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {
        throw std::runtime_error("ERROR::ASSIMP::" +
                                 std::string(import.GetErrorString()));
    }

    process_node(scene->mRootNode, scene);

    //delete file;
}

void model::process_node(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

mesh model::process_mesh(aiMesh* m, const aiScene* scene)
{
    std::vector<vertex3d_textured> vertixes;
    std::vector<uint16_t>          indexes;
    // vector<texture>      textures;

    for (unsigned int i = 0; i < m->mNumVertices; i++)
    {
        vertex3d_textured v;
        if (m->mTextureCoords[0])
        {
            v = vertex3d_textured(
                vertex3d(
                    m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z),
                vector2d(m->mTextureCoords[0][i].x, m->mTextureCoords[0][i].y));
        }
        else
        {
            v = vertex3d_textured(vertex3d(m->mVertices[i].x,
                                           m->mVertices[i].y,
                                           m->mVertices[i].z),
                                  vector2d(0, 0));
        }
        // if (m->mNormals)
        // {
        v.normal.x = m->mNormals[i].x;
        v.normal.y = m->mNormals[i].y;
        v.normal.z = m->mNormals[i].z;
        // }

        vertixes.push_back(v);
    }

    for (unsigned int i = 0; i < m->mNumFaces; i++)
    {
        aiFace face = m->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indexes.push_back(static_cast<uint32_t>(face.mIndices[j]));
    }

    // if (m->mMaterialIndex >= 0)
    // {
    // }

    return mesh(vertixes, indexes);
}
