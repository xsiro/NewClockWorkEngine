#ifndef _WWISE_LIBS_H_
#define _WWISE_LIBS_H_

#include "Wwise/AK/SoundEngine/Common/AkSoundEngine.h"	  
#include "Wwise/AK/MusicEngine/Common/AkMusicEngine.h"	  


#include "Wwise/AK/SoundEngine/Common/AkMemoryMgr.h"
#include "Wwise/AK/SoundEngine/Common/AkModule.h"

#include "Wwise/AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "Wwise/AK/Tools/Common/AkPlatformFuncs.h"


#include "Wwise/AK/SpatialAudio/Common/AkSpatialAudio.h"

#ifndef AK_OPTIMIZED //AK_OPTIMIZED is defined by the precompiler only in release


#pragma comment(lib, "Wwise/Profile(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment(lib, "Wwise/Profile(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment(lib, "Wwise/Profile(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment(lib, "Wwise/Profile(StaticCRT)/lib/AkStreamMgr.lib")
#pragma comment(lib, "Wwise/Profile(StaticCRT)/lib/AkSpatialAudio.lib")

#include "Wwise/AK/Comm/AkCommunication.h"
#pragma comment(lib, "Wwise/Profile(StaticCRT)/lib/CommunicationCentral.lib")
#pragma comment( lib, "Wwise/ws2_32.LIB")

#else

#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkStreamMgr.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkSpatialAudio.lib")

#endif // !AK_OPTIMIZED

//DirectX external libs
#pragma comment(lib,"Wwise/dinput8.lib")
#pragma comment(lib,"Wwise/dsound.lib")
#pragma comment(lib,"Wwise/dxguid.lib")


// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void* AllocHook(size_t in_size);
	void FreeHook(void* in_ptr);
	void* VirtualAllocHook(void* in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect);
	void VirtualFreeHook(void* in_pMemAddress, size_t in_size, DWORD in_dwFreeType);
#endif
}






//
//#include "Wwise/include/AK/SoundEngine/Common/AkSoundEngine.h"   
//#include "Wwise/include/AK/MusicEngine/Common/AkMusicEngine.h"   
//#include "Wwise/include/AK/SoundEngine/Common/AkMemoryMgr.h"      
//#include "Wwise/include/AK/SoundEngine/Common/AkModule.h"         
//#include "Wwise/include/AK/SoundEngine/Common/AkStreamMgrModule.h"  
//#include "Wwise/include/AK/SoundEngine/Common/AkTypes.h"
//#include "Wwise/include/AK/Tools/Common/AkPlatformFuncs.h" 
//
///*	Uncomenting this define will set the library to "RELEASE" mode. Some chunks of code will be skipped
//	to increase performance, but debug will not be possible.	*/
//	//#define AK_OPTIMIZED
//
//#ifdef AK_OPTIMIZED
//
//#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkSoundEngine.lib")
//#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkMusicEngine.lib")
//#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkMemoryMgr.lib")
//#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkStreamMgr.lib")
//
//#else
//
//#include "Wwise/include/AK/Comm/AkCommunication.h"
//#pragma comment( lib, "Wwise/Profile(StaticCRT)/lib/CommunicationCentral.lib")
//#pragma comment( lib, "Wwise/ws2_32.lib")
//#pragma comment( lib, "Wwise/Profile(StaticCRT)/lib/AkSoundEngine.lib")
//#pragma comment( lib, "Wwise/Profile(StaticCRT)/lib/AkMusicEngine.lib")
//#pragma comment( lib, "Wwise/Profile(StaticCRT)/lib/AkMemoryMgr.lib")
//#pragma comment( lib, "Wwise/Profile(StaticCRT)/lib/AkStreamMgr.lib")
//
//#endif
//
//#pragma comment( lib, "Wwise/dinput8.lib") 
//#pragma comment( lib, "Wwise/dsound.lib")  
//#pragma comment( lib, "Wwise/dxguid.lib")
//
//
//namespace AK
//{
//#ifdef WIN32
//
//	void* AllocHook(size_t in_size)
//	{
//		return malloc(in_size);
//	}
//	void FreeHook(void* in_ptr)
//	{
//		free(in_ptr);
//	}
//	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
//	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
//	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
//	// you may implement it with a simple malloc().
//	void* VirtualAllocHook(void* in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect)
//	{
//		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
//	}
//	void VirtualFreeHook(void* in_pMemAddress, size_t in_size, DWORD in_dwFreeType)
//	{
//		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
//	}
//#endif // WIN32
//}


#endif
