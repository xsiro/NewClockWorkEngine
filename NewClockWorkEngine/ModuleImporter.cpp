#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleMaterial.h"
#include "ModuleMesh.h"
#include "ModuleTransform.h"
#include "GameObject.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "Devil/Include/IL/ilut.h"
#include "Devil/Include/IL/ilu.h"
#include "Devil/Include/IL/il.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment (lib, "Devil/libx86/ILU.lib")
#pragma comment (lib, "Devil/libx86/ILUT.lib")
#pragma comment (lib, "Assimp/libx86/assimp.lib")



std::vector<Mesh*> Importer::MeshImporter::Import(const char* file)
{
    //aiImportFileFromMemory
    const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
    std::vector<Mesh*> ret;

    if (scene != nullptr && scene->HasMeshes())
    {
        // Use scene->mNumMeshes to iterate on scene->mMeshes array
        for (int i = 0; i < scene->mNumMeshes; i++)
        {
            Mesh* newMesh = new Mesh();

            newMesh->buffersSize[Mesh::vertex] = scene->mMeshes[i]->mNumVertices;
            newMesh->vertices = new float[newMesh->buffersSize[Mesh::vertex] * 3];
            memcpy(newMesh->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * newMesh->buffersSize[Mesh::vertex] * 3);
            LOG("New mesh with %d vertices", newMesh->buffersSize[Mesh::vertex]);

            if (scene->mMeshes[i]->HasFaces())
            {
                newMesh->buffersSize[Mesh::index] = scene->mMeshes[i]->mNumFaces * 3;
                newMesh->indices = new uint[newMesh->buffersSize[Mesh::index]];
                for (uint f = 0; f < scene->mMeshes[i]->mNumFaces; ++f)
                {
                    if (scene->mMeshes[i]->mFaces[f].mNumIndices != 3)
                    {
                        LOG("WARNING, geometry face with != 3 indices!");
                    }
                    else
                    {
                        memcpy(&newMesh->indices[f * 3], scene->mMeshes[i]->mFaces[f].mIndices, 3 * sizeof(uint));
                    }
                }
            }

            if (scene->mMeshes[i]->HasNormals())
            {
                newMesh->buffersSize[Mesh::normal] = scene->mMeshes[i]->mNumVertices;
                newMesh->normals = new float[newMesh->buffersSize[Mesh::normal] * 3];
                memcpy(newMesh->normals, scene->mMeshes[i]->mNormals, sizeof(float) * newMesh->buffersSize[Mesh::normal] * 3);
            }

            if (scene->mMeshes[i]->HasTextureCoords(0))
            {
                newMesh->buffersSize[Mesh::texture] = scene->mMeshes[i]->mNumVertices;
                newMesh->textureCoords = new float[scene->mMeshes[i]->mNumVertices * 2];

                for (int j = 0; j < newMesh->buffersSize[Mesh::texture]; j++)
                {
                    newMesh->textureCoords[j * 2] = scene->mMeshes[i]->mTextureCoords[0][j].x;
                    newMesh->textureCoords[j * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][j].y;
                }
            }

            newMesh->aabb.SetNegativeInfinity();
            newMesh->aabb.Enclose((float3*)newMesh->vertices, newMesh->buffersSize[Mesh::vertex]);

            App->renderer3D->GenerateBuffers(newMesh);

            newMesh->path = file;

            ret.push_back(newMesh);
        }

        aiReleaseImport(scene);
    }
    else
    {
        LOG("(ERROR) Error loading scene %s", file);
    }
    return ret;
}

void Importer::MeshImporter::Save(const Mesh mesh)
{
    uint sizes[4] = { mesh.buffersSize[Mesh::vertex] ,mesh.buffersSize[Mesh::index], mesh.buffersSize[Mesh::normal], mesh.buffersSize[Mesh::texture] };

    uint size = sizeof(sizes) + sizeof(uint) * mesh.buffersSize[Mesh::index] + sizeof(float) * mesh.buffersSize[Mesh::vertex] * 3
        + sizeof(float) * mesh.buffersSize[Mesh::normal] * 3 + sizeof(float) * mesh.buffersSize[Mesh::texture] * 2;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    uint bytes = sizeof(sizes);
    memcpy(cursor, sizes, bytes);
    cursor += bytes;

    //Indices
    bytes = sizeof(uint) * mesh.buffersSize[Mesh::index];
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;

    //vertices
    bytes = sizeof(uint) * mesh.buffersSize[Mesh::vertex] * 3;
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;

    //normals
    bytes = sizeof(uint) * mesh.buffersSize[Mesh::normal] * 3;
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;

    //TextureCoords
    bytes = sizeof(uint) * mesh.buffersSize[Mesh::texture] * 2;
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;
}

void Importer::MeshImporter::Load(const char* fileBuffer, Mesh* mesh)
{
    const char* cursor = fileBuffer;

    uint sizes[4];
    uint bytes = sizeof(sizes);
    memcpy(sizes, cursor, bytes);
    cursor += bytes;

    mesh->buffersSize[Mesh::index] = sizes[0];
    mesh->buffersSize[Mesh::vertex] = sizes[1];
    mesh->buffersSize[Mesh::normal] = sizes[2];
    mesh->buffersSize[Mesh::texture] = sizes[3];

    //Indices
    bytes = sizeof(uint) * mesh->buffersSize[Mesh::index];
    mesh->indices = new uint[mesh->buffersSize[Mesh::index]];
    memcpy(mesh->indices, cursor, bytes);
    cursor += bytes;

    //Vertices
    bytes = sizeof(float) * mesh->buffersSize[Mesh::vertex] * 3;
    mesh->vertices = new float[mesh->buffersSize[Mesh::vertex] * 3];
    memcpy(mesh->vertices, cursor, bytes);
    cursor += bytes;

    //Normals
    bytes = sizeof(float) * mesh->buffersSize[Mesh::normal] * 3;
    mesh->normals = new float[mesh->buffersSize[Mesh::normal] * 3];
    memcpy(mesh->normals, cursor, bytes);
    cursor += bytes;

    //Texture Coords
    bytes = sizeof(float) * mesh->buffersSize[Mesh::texture] * 2;
    mesh->textureCoords = new float[mesh->buffersSize[Mesh::texture] * 2];
    memcpy(mesh->normals, cursor, bytes);
    cursor += bytes;
}

void Importer::TextureImp::InitDevil()
{
    LOG("Initializing Devil")
        ilInit();
    iluInit();
    ilutInit();
    ilutRenderer(ILUT_OPENGL);
}

Material* Importer::TextureImp::Import(const char* path)
{
    Material* newTexture = new Material;
    uint i;

    ilGenImages(1, &i);
    ilBindImage(i);

    if (ilLoadImage(path))
    {
        ILinfo ImgInfo;
        iluGetImageInfo(&ImgInfo);

        if (ImgInfo.Origin == IL_ORIGIN_UPPER_LEFT)
            iluFlipImage();

        if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
        {
            newTexture->id = CreateTexture(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT));
            newTexture->height = ilGetInteger(IL_IMAGE_HEIGHT);
            newTexture->width = ilGetInteger(IL_IMAGE_WIDTH);
            newTexture->path = path;

            LOG("Successfully loaded Texture from: %s", path);
        }
        else
        {
            LOG("(ERROR)Could not convert image %s", path);
        }
    }
    else
    {
        LOG("(ERROR) Error loading Image %s", path);
    }



    return newTexture;
}

uint Importer::TextureImp::CreateTexture(const void* data, uint width, uint height, uint format)
{
    uint id;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}