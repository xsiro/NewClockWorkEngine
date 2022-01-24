#include <string>
#include <vector>
struct aiMesh;
struct aiNode;
struct aiScene;
class GameObject;
class ResourceMesh;
class Resource;
class ResourceMaterial;
class ModuleMaterial;
class ModuleTransform;
class ModuleCamera;
class AudioSource;
class AudioListener;
struct JSON_Object;
class Config;
class Component;

namespace Importer
{
	bool InitializeDevIL();

	namespace Model
	{
		//Take a saved buffer in memory and loads it into a model resource, also imports dependencies if needed (aka meshes / textures)
		bool ImportModel(const char* Buffer, unsigned int Length, const char* relativePath, Resource* res);
		bool LoadModel(const char* libPath, GameObject* root, bool onlyBase = false);//revise if needed
		unsigned int SaveModel(GameObject* root, Resource* ret);//returns buffer size; it cals upon SavePHO on its own
	}

	namespace Mesh
	{
		void ImportRMesh(aiMesh* fbxMesh, ResourceMesh& meshToFill);
		bool LoadMesh(char* Buffer, unsigned int Length, Resource& meshToFill);
		unsigned int SaveMesh(Resource& aux, char** buffer);//returns buffer 
	}

	namespace Texture
	{
		//path required for display in the inspector. froma ssets to lib
		bool ImportImage(const char* Buffer, unsigned int Length, Resource& textureToFill);


		//from lib to engine
		bool LoadNewImage(const char* libPath, Resource& textureToFill);

		//Used in GameObject serialization. Never call this "raw". from engine to lib
		void SaveComponentMaterial(Config& config, Component* Mat);

		//Testing own file format. from engine to lib
		unsigned int SaveTexture(Resource& texture);

	}

	namespace Camera
	{
		//Import camera will exist when we load Cameras from Fbx, NOT NOW
		//Own file format
		unsigned int SaveCamera(ModuleCamera* aux, char* buffer);
		//Used in GameObject serialization. Never call this "raw"
		void SaveComponentCamera(Config& config, Component* camera);
		//Own file format
		bool LoadCameraFromPho(char* buffer, unsigned int Lenght);

	}

	namespace Audio
	{
		void SaveComponentAudioListener(Config& config, Component* audioListener);
		void SaveComponentAudioSource(Config& config, Component* audioSource);
		void SaveComponentReverbZone(Config& config, Component* reverbZone);
	}


	namespace Controller
	{
		void SaveComponentController(Config& config, Component* controller);
	}


	//Dont use this directly
	unsigned int LoadPureImageGL(const char* path);


	//returns size of buffer. Caution! this does not create file, it only fills buffer. Create file usinsg SavePHO from FliseSystem.
	unsigned int SerializeScene(GameObject* root, char** TrueBuffer);

	//takes a buffer from a JSON and creates all the GO hierarchy starting from root
	void LoadScene(char* buffer, GameObject* sceneRoot);

	//Dont use this directly: it's called from SerializeScene
	void SerializeGameObject(Config& config, GameObject* gameObject);
	//Fill a vector with a GO and the childs tree
	void SeekChildrenRecurvisely(GameObject* root, std::vector<GameObject*>& vectorToFill);
	//Dont use this directly: it's called from SerializeScene
	void SaveComponentRaw(Config& config, Component* component);


	//Importing Components once we have a loaded buffer
	//void ImportResourceMeshFromBuffer(char* buffer, unsigned int buffersize, Resource& meshToFill);
}