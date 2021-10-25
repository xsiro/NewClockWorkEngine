#include "ImporterTex.h"
#include "Application.h"
#include "FileSys.h"
#include "Timer.h"
#include "ResourceTex.h"
#include "options.h"

#include "Devil/include/IL/il.h"
#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")	
#pragma comment (lib, "Devil/libx86/ILU.lib")	
#pragma comment (lib, "Devil/libx86/ILUT.lib")	

void ImporterTex::Init()
{
	ilInit();
	iluInit();

	if (ilutRenderer(ILUT_OPENGL))
		LOG("DevIL initted correctly");
}

void ImporterTex::Import(char* fileBuffer, ResourceTex* texture, uint size)
{
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error = IL_NO_ERROR;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	//ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		//LOG_ERROR("%s, %d %s", texture->assetsFile.c_str(), ilGetError(), iluErrorString(error));
		//return;
	}

	ILenum file_format = GetFileFormat(texture->assetsFile.c_str());

	if (ilLoadL(file_format, fileBuffer, size) == IL_FALSE)
	{
		LOG_ERROR("Error importing texture %s - %d: %s", texture->assetsFile, ilGetError(), iluErrorString(ilGetError()));

		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		return;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		LOG_ERROR("%d: %s", error, iluErrorString(error));
	}
	else
	{
		LOG("Texture loaded successfully from: %s in %d ms", texture->assetsFile.c_str(), timer.Read());

		texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}

	ilBindImage(0);
}


void ImporterTex::UnloadTexture(uint imageID)
{
	ilDeleteImages(1, &imageID);
}

ILenum ImporterTex::GetFileFormat(const char* file)
{
	ILenum file_format = IL_TYPE_UNKNOWN;
	std::string format = FileSys::GetFileFormat(file);

	if (format == ".png")
		file_format = IL_PNG;
	else if (format == ".jpg")
		file_format = IL_JPG;
	else if (format == ".bmp")
		file_format = IL_BMP;

	return file_format;
}

void ImporterTex::ApplyImportingOptions(TextureImportingOptions importingOptions)
{
	if (importingOptions.flip)
		iluFlipImage();

	if (importingOptions.alienify)
		iluAlienify();

	if (importingOptions.blur_average)
		iluBlurAvg(10);

	if (importingOptions.blur_gaussian)
		iluBlurGaussian;

	if (importingOptions.equalize)
		iluEqualize();

	if (importingOptions.negativity)
		iluNegative();

	if (importingOptions.noise)
		iluNoisify(importingOptions.noise_tolerance);

	if (importingOptions.pixelize)
		iluPixelize(importingOptions.pixelize_size);

	if (importingOptions.sharpening)
		iluSharpen(importingOptions.sharpening_factor, importingOptions.sharpening_iterations);

	iluGammaCorrect(importingOptions.gamma_correction);

	bool pixelize = false;
	bool sharpening = false;
	float contrast = 1.0f;
}