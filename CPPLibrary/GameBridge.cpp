#include "stdafx.h"
#include "GameLogic.h"
#include <oaidl.h>
#include <comdef.h>
#include <comutil.h>
#include <string>
//--------- PREPROCESSOR ---------//
#ifdef CPPLIBRARY_EXPORTS
#define CPPLIBRARY_API __declspec(dllexport)
#else
#define CPPLIBRARY_API __declspec(dllimport)
#endif



extern "C" _declspec(dllexport) CppLibrary::GameManager * __stdcall CreateGameManager()
{
	return new CppLibrary::GameManager();
}

extern "C" _declspec(dllexport) void __stdcall UpdateGameManager(CppLibrary::GameManager * gameManager, const float deltaTime)
{
	gameManager->Update(deltaTime);
}

//passing game obj containers
extern "C" __declspec(dllexport) void __stdcall GetGameObjectContainerCpp(CppLibrary::GameManager * pGame, CppLibrary::ObjectContainerType type, CppLibrary::GameObject* const*& pFirstElement, unsigned long* pLength)
{
	////use gamemanager to get the container with corresponding "type", set the pFirstElement ptr reference to the address of 1st element in container
	if(!pGame->GetGameObjectContainer(type).empty())
		pFirstElement = &pGame->GetGameObjectContainer(type)[0];
	////length of container
	*pLength = unsigned long(pGame->GetGameObjectContainer(type).size());
}
extern "C" __declspec(dllexport) int __stdcall GetSizeOfGameObject()
{
	return sizeof(CppLibrary::GameObject);
}
extern "C" __declspec(dllexport) void __stdcall GetPlayerObjCpp(CppLibrary::GameManager * pGame, CppLibrary::GameObject * &pPlayer)
{
	*pPlayer = *pGame->GetPlayer();
}

extern "C" __declspec(dllexport) void __stdcall SetPlayerNameCpp(CppLibrary::GameManager * pGame, char* name)
{
	pGame->SetName(name);
}

extern "C" __declspec(dllexport) BSTR __stdcall GetPlayerNameCpp(CppLibrary::GameManager * pGame)
{
	return _com_util::ConvertStringToBSTR(pGame->GetName());
}

extern "C" __declspec(dllexport) void __stdcall MovePlayerObjectCpp(CppLibrary::GameManager * pGame, CppLibrary::Vector2 direction, float deltaTime)
{
	pGame->MovePlayer(direction, deltaTime);
}

extern "C" __declspec(dllexport) void SpawnProjectileObjectCpp(CppLibrary::GameManager * pGame, CppLibrary::Vector2 pos)
{
	pGame->SpawnProjectile(pos);
}

extern "C" __declspec(dllexport) void SetShootDelegateCpp(CppLibrary::GameManager * pGame, CppLibrary::SoundCallback pFnc)
{
	pGame->SetShootCallback(pFnc);
}

extern "C" __declspec(dllexport) void SetKillDelegateCpp(CppLibrary::GameManager * pGame, CppLibrary::SoundCallback pFnc)
{
	pGame->SetKillCallback(pFnc);
}