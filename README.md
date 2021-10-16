# C++/C# Interoperability - Space Invaders

## What is interoperability?
the capability of two different programming languages to natively interact as part of the same system and operate on the same kind of data structures. In this case it's unmanaged C++ data being marshalled into managed C# data using PInvoke.

### PInvoke?
Explicit Platform Invoke (or PInvoke) is, in broad strokes, a feature that allows managed code to call unmanaged "native" code. explicit PInvoke uses the DllImport attribute.
Pros for using explicit PInvoke:
 -Not platform specific.
 -No real need for an intermediate library (you can directly call the managed 
functions)
Cons for using explicit PInvoke:
 -Has a more marshalling overhead causing lesser performance than implicit invoking, this does not matter in many cases.
 
## Why?
Sometimes, we want code that has been written and compiled in different programming languages to communicate with each other. The Unity engine, for example, is written in C++, but the editor is mainly written in C#.
Unity exposes a .NET API so that not every programmer is required to write their game in C++, but in C# instead.
The space invaders logic is simple, yet it taught me much about PInvoke interoperability.

## The Project!
Having the space invaders logic in C++, how do we make it work in C# Unity? By using PInvoke to marshal our unmanaged data to managed data of course!

### How?
C++ is a direct descendant of C that retains almost all of C as a subset, hence we can use a C wrapper as "bridge" to PInvoke our code into C#!
This prevents the need for name mangling by simply declaring we want to export our methods as C methods.

The drawback? C does not support classes, thus classes object aren't so simple to marshal to managed code.

To declare a C method export:
```cpp
extern "C" __declspec extern(dllexport) void MethodName(int parameter)
```

To PInvoke said method in managed C# code:
```c#
[DllImport(NameOfCPPLibrary, CallingConvention = CallingConvention.Cdecl)] //The default CallingConvention for C# is stdcall, of C it's Cdecl, thus we tell C# which one to use
public static extern void MethodName(int parameter); //If you want a different methodname, clarify with the DllImport attribute using "EntryPoint = "WantedNameForMethod""
```

### The C Wrapper
```cpp
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
	if(!pGame->GetGameObjectContainer(type).empty() && pgame->GetGameObjectContainer(type)[0])
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
```

