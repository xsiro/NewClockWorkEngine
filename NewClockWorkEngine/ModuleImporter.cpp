#include "Globals.h"
#include "Application.h"
#include "timer.h"
#include "ModuleImporter.h"
#include "ModuleMaterial.h"
#include "ModuleMesh.h"
#include "ModuleTransform.h"
#include "GameObject.h"
#include "FileSystem.h" 
#include "ModuleSceneIntro.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "MathGeoLib/src/MathGeoLib.h"
#include "Devil/Include/IL/ilut.h"
#include "Devil/Include/IL/ilu.h"
#include "Devil/Include/IL/il.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma comment (lib, "Devil/libx86/ILU.lib")
#pragma comment (lib, "Devil/libx86/ILUT.lib")
#pragma comment (lib, "Assimp/libx86/assimp.lib")

std::vector<ResourceMesh*> Importer::MeshImporter::Import(const char* file)
{
    Timer* timeImporting = new Timer();

    const aiScene* scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
    std::vector<ResourceMesh*> ret;

    if (scene != nullptr && scene->HasMeshes())
    {
       
        for (int i = 0; i < scene->mNumMeshes; i++)
        {
            ResourceMesh* newMesh = new ResourceMesh();

            newMesh->buffersSize[ResourceMesh::vertex] = scene->mMeshes[i]->mNumVertices;
            newMesh->vertices = new float[newMesh->buffersSize[ResourceMesh::vertex] * 3];
            memcpy(newMesh->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * newMesh->buffersSize[ResourceMesh::vertex] * 3);
            LOG("New mesh with %d vertices", newMesh->buffersSize[ResourceMesh::vertex]);

            if (scene->mMeshes[i]->HasFaces())
            {
                newMesh->buffersSize[ResourceMesh::index] = scene->mMeshes[i]->mNumFaces * 3;
                newMesh->indices = new uint[newMesh->buffersSize[ResourceMesh::index]];
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
                newMesh->buffersSize[ResourceMesh::normal] = scene->mMeshes[i]->mNumVertices;
                newMesh->normals = new float[newMesh->buffersSize[ResourceMesh::normal] * 3];
                memcpy(newMesh->normals, scene->mMeshes[i]->mNormals, sizeof(float) * newMesh->buffersSize[ResourceMesh::normal] * 3);
            }


            if (scene->mMeshes[i]->HasTextureCoords(0))
            {
                newMesh->buffersSize[ResourceMesh::texture] = scene->mMeshes[i]->mNumVertices;
                newMesh->textureCoords = new float[scene->mMeshes[i]->mNumVertices * 2];

                for (int j = 0; j < newMesh->buffersSize[ResourceMesh::texture]; j++)
                {
                    newMesh->textureCoords[j * 2] = scene->mMeshes[i]->mTextureCoords[0][j].x;
                    newMesh->textureCoords[j * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][j].y;
                }
            }

            newMesh->aabb.SetNegativeInfinity();
            newMesh->aabb.Enclose((float3*)newMesh->vertices, newMesh->buffersSize[ResourceMesh::vertex]);

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

    delete timeImporting;

    return ret;
}

void Importer::MeshImporter::LoadNodeMesh(const aiScene* scene, const aiNode* node, std::vector<ResourceMesh*>& meshes)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        ResourceMesh* newMesh = new ResourceMesh();

        newMesh->buffersSize[ResourceMesh::vertex] = scene->mMeshes[node->mMeshes[i]]->mNumVertices;
        newMesh->vertices = new float[newMesh->buffersSize[ResourceMesh::vertex] * 3];
        memcpy(newMesh->vertices, scene->mMeshes[node->mMeshes[i]]->mVertices, sizeof(float) * newMesh->buffersSize[ResourceMesh::vertex] * 3);
        LOG("New mesh with %d vertices", newMesh->buffersSize[ResourceMesh::vertex]);

        if (scene->mMeshes[node->mMeshes[i]]->HasFaces())
        {
            newMesh->buffersSize[ResourceMesh::index] = scene->mMeshes[node->mMeshes[i]]->mNumFaces * 3;
            newMesh->indices = new uint[newMesh->buffersSize[ResourceMesh::index]];
            for (uint f = 0; f < scene->mMeshes[node->mMeshes[i]]->mNumFaces; ++f)
            {
                if (scene->mMeshes[node->mMeshes[i]]->mFaces[f].mNumIndices != 3)
                {
                    LOG("WARNING, geometery face with != 3 indices!");
                   
                }
                else
                {
                    memcpy(&newMesh->indices[f * 3], scene->mMeshes[node->mMeshes[i]]->mFaces[f].mIndices, 3 * sizeof(uint));
                }
            }
        }

        if (scene->mMeshes[node->mMeshes[i]]->HasNormals())
        {
            newMesh->buffersSize[ResourceMesh::normal] = scene->mMeshes[node->mMeshes[i]]->mNumVertices;
            newMesh->normals = new float[newMesh->buffersSize[ResourceMesh::normal] * 3];
            memcpy(newMesh->normals, scene->mMeshes[node->mMeshes[i]]->mNormals, sizeof(float) * newMesh->buffersSize[Mesh::normal] * 3);
        }

        if (scene->mMeshes[node->mMeshes[i]]->HasTextureCoords(0))
        {
            newMesh->buffersSize[ResourceMesh::texture] = scene->mMeshes[node->mMeshes[i]]->mNumVertices;
            newMesh->textureCoords = new float[scene->mMeshes[node->mMeshes[i]]->mNumVertices * 2];

            for (int j = 0; j < newMesh->buffersSize[ResourceMesh::texture]; j++)
            {
                newMesh->textureCoords[j * 2] = scene->mMeshes[node->mMeshes[i]]->mTextureCoords[0][j].x;
                newMesh->textureCoords[j * 2 + 1] = scene->mMeshes[node->mMeshes[i]]->mTextureCoords[0][j].y;
            }
        }

        App->renderer3D->GenerateBuffers(newMesh); 

        meshes.push_back(newMesh);
    }
}

uint64 Importer::MeshImporter::Save(const ResourceMesh mesh)
{
    Timer* timeSaving = new Timer();

    uint sizes[4] = { mesh.buffersSize[ResourceMesh::vertex] ,mesh.buffersSize[ResourceMesh::index], mesh.buffersSize[Mesh::normal], mesh.buffersSize[Mesh::texture] };

    uint size = sizeof(sizes) + sizeof(uint) * mesh.buffersSize[ResourceMesh::index] + sizeof(float) * mesh.buffersSize[Mesh::vertex] * 3
        + sizeof(float) * mesh.buffersSize[ResourceMesh::normal] * 3 + sizeof(float) * mesh.buffersSize[ResourceMesh::texture] * 2;

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
    bytes = sizeof(uint) * mesh.buffersSize[ResourceMesh::vertex] * 3;
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;

    //normals
    bytes = sizeof(uint) * mesh.buffersSize[ResourceMesh::normal] * 3;
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;

    //TextureCoords
    bytes = sizeof(uint) * mesh.buffersSize[ResourceMesh::texture] * 2;
    memcpy(cursor, mesh.indices, bytes);
    cursor += bytes;

    int id = rand() % 50;
    char tmp[5];
    sprintf(tmp, "%d", id);
    std::string fileName = "Library/Meshes/";
    fileName += tmp;

    LOG("Saved mesh in id: %s", fileName.c_str());

    App->filesystem->Save(fileName.c_str(), fileBuffer, size);

    LOG("Time spent saving Mesh: %d ms", timeSaving->Read());

    return id;
}

void Importer::MeshImporter::Load(const char* fileBuffer, ResourceMesh* mesh)
{
    Timer* timeLoading = new Timer();

    const char* cursor = fileBuffer;

    uint sizes[4];
    uint bytes = sizeof(sizes);
    memcpy(sizes, cursor, bytes);
    cursor += bytes;

    mesh->buffersSize[ResourceMesh::index] = sizes[0];
    mesh->buffersSize[ResourceMesh::vertex] = sizes[1];
    mesh->buffersSize[ResourceMesh::normal] = sizes[2];
    mesh->buffersSize[ResourceMesh::texture] = sizes[3];

    //Indices
    bytes = sizeof(uint) * mesh->buffersSize[ResourceMesh::index];
    mesh->indices = new uint[mesh->buffersSize[ResourceMesh::index]];
    memcpy(mesh->indices, cursor, bytes);
    cursor += bytes;

    //Vertices
    bytes = sizeof(float) * mesh->buffersSize[ResourceMesh::vertex] * 3;
    mesh->vertices = new float[mesh->buffersSize[ResourceMesh::vertex] * 3];
    memcpy(mesh->vertices, cursor, bytes);
    cursor += bytes;

    //Normals
    bytes = sizeof(float) * mesh->buffersSize[ResourceMesh::normal] * 3;
    mesh->normals = new float[mesh->buffersSize[ResourceMesh::normal] * 3];
    memcpy(mesh->normals, cursor, bytes);
    cursor += bytes;

    //Texture Coords
    bytes = sizeof(float) * mesh->buffersSize[ResourceMesh::texture] * 2;
    mesh->textureCoords = new float[mesh->buffersSize[ResourceMesh::texture] * 2];
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

ResourceMaterial* Importer::TextureImp::Import(const char* path)
{
    ResourceMaterial* newMaterial = new ResourceMaterial();
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
            newMaterial->SetId(CreateTexture(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT)));
            newMaterial->SetHeight(ilGetInteger(IL_IMAGE_HEIGHT));
            newMaterial->SetWidth(ilGetInteger(IL_IMAGE_WIDTH));
            newMaterial->SetPath(path);

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



    return newMaterial;
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

void Importer::SceneImporter::Import(const char* file)
{
    char* buffer;
    uint byteSize = App->filesystem->Load(file, &buffer);

    if (byteSize > 0)
    {
        const aiScene* scene = aiImportFileFromMemory(buffer, byteSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
        const aiNode* rootNode = scene->mRootNode;

        ProcessAiNode(scene, rootNode, App->scene_intro->rootObject, file);

        LOG("Finished importing: %s", file);
    }
    else
    {
        LOG("(ERROR) Could not load .fbx file");
    }
}

void Importer::SceneImporter::ProcessAiNode(const aiScene* scene, const aiNode* node, GameObject* parentObject, const char* file) 
{
    GameObject* newGameObject;

    if (node->mParent == NULL)
    {
        std::string name;
        App->filesystem->SplitFilePath(file, nullptr, &name);
        newGameObject = new GameObject(App->scene_intro->rootObject, name.c_str());
    }
    else
    {
        aiString nodeName = node->mName;
        newGameObject = new GameObject(parentObject, node->mName.C_Str());
    }

    LoadTransform(node, newGameObject);

    if (node->mNumMeshes > 0)
    {
        LoadMeshes(scene, node, newGameObject);
    }

    LoadMaterial(scene, node, newGameObject, file);

    parentObject->children.push_back(newGameObject);
    App->scene_intro->game_objects.push_back(newGameObject);

    for (int i = 0; i < node->mNumChildren; i++)
    {
        ProcessAiNode(scene, node->mChildren[i], newGameObject, file);
    }
}

const aiNode* Importer::SceneImporter::LoadTransform(const aiNode* node, GameObject* newGameObject)
{
    aiVector3D position = { 0,0,0 };
    aiVector3D scale = { 0,0,0 };
    aiQuaternion rotation;

    aiVector3D _position = { 0,0,0 };
    aiVector3D _scale = { 0,0,0 };
    aiQuaternion _rotation = { 0,0,0,0 };

    while (strstr(node->mName.C_Str(), "_$AssimpFbx$_") != nullptr && node->mNumChildren == 1)
    {
        node->mTransformation.Decompose(_scale, _rotation, _position);

        position.x += _position.x;
        position.y += _position.y;
        position.z += _position.z;

        scale.x *= _scale.x;
        scale.y *= _scale.y;
        scale.z *= _scale.z;

        rotation = rotation * _rotation;

        node = node->mChildren[0];

        newGameObject->SetName(node->mName.C_Str());
    }

    node->mTransformation.Decompose(_scale, _rotation, _position);

    scale.x += _scale.x;
    scale.y += _scale.y;
    scale.z += _scale.z;

    rotation = rotation * _rotation;
    position += _position;

    float3 p = { position.x, position.y, position.z };
    float3 s = { scale.x, scale.y, scale.z };
    Quat r = { rotation.x,rotation.y,rotation.z,rotation.w };

    newGameObject->transform->SetLocalTransform(p, s, r);

    return node;
}

void Importer::SceneImporter::LoadMeshes(const aiScene* scene, const aiNode* node, GameObject* newGameObject)
{
    std::vector<ResourceMesh*> meshes;

    Importer::MeshImporter::LoadNodeMesh(scene, node, meshes);

    for (int i = 0; i < meshes.size(); i++)
    {
        newGameObject->AddComponent(new ModuleMesh(newGameObject, "", meshes[i]));
    }
}
void Importer::SceneImporter::LoadMaterial(const aiScene* scene, const aiNode* node, GameObject* newGameObject, const char* file)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        ResourceMaterial* material;
        uint index = scene->mMeshes[node->mMeshes[i]]->mMaterialIndex;
        aiString path;

        if (index >= 0)
        {
            if (scene->mMaterials[index]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                std::string fileName, extension;
                App->filesystem->SplitFilePath(path.C_Str(), nullptr, &fileName, &extension);

                fileName += "." + extension;
                fileName = "Assets" + fileName;

                LOG("Adding texture to %s", newGameObject->GetName());

                material = Importer::TextureImp::Import(fileName.c_str());

                newGameObject->AddComponent(new ModuleMaterial(newGameObject, fileName.c_str(), material));
            }
            else
            {
                LOG("(ERROR) Failed loading node texture: %s in node %s", path.C_Str(), node->mName.C_Str());
            }
        }

    }
}