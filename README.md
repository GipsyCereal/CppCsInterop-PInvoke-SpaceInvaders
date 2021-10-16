# C++/C# Interoperability - Space Invaders

## What is interoperability?
The capability of two different programming languages to natively interact as part of the same system and operate on the same kind of data structures. In this case it's unmanaged C++ data being marshalled into managed C# data using PInvoke.

### PInvoke?
Explicit Platform Invoke (or PInvoke) is, in broad strokes, a feature that allows managed code to call unmanaged "native" code by using the explicit PInvoke DllImport attribute.  
Pros for using explicit PInvoke:
 * Not platform specific.
 * No real need for an intermediate library (you can directly call the managed 
functions)  

Cons for using explicit PInvoke:
 * Has a more marshalling overhead causing lesser performance than implicit invoking, this does not matter in many cases.
 
## Why?
Sometimes, we want code that has been written and compiled in different programming languages to communicate with each other. The Unity engine, for example, is written in C++, but the editor is mainly written in C#.  

Unity exposes a .NET API so that not every programmer is required to write their game in C++, but in C# instead.
The space invaders logic is simple, yet it taught me much about PInvoke interoperability.

## The Project!
Having the space invaders logic in C++, how do we make it work in C# Unity? By using PInvoke to marshal our unmanaged data to managed data of course!

### How?
C++ is a direct descendant of C that retains almost all of C as a subset, hence we can use a C wrapper as "bridge" to PInvoke our code into C#!
This prevents the need for name mangling by simply declaring we want to export our methods as C methods.

The drawback? C does not support classes, thus classes object aren't so simple to PInvoke into managed code.

To declare a C method export:
```cpp
extern "C" __declspec extern(dllexport) void MethodName(int parameter)
```

To PInvoke said method in managed C# code:
```c#
[DllImport(NameOfCPPLibrary, CallingConvention = CallingConvention.Cdecl)] //The default CallingConvention for C# is stdcall, of C it's Cdecl, thus we tell C# which one to use
public static extern void MethodName(int parameter); //If you want a different methodname, clarify with the DllImport attribute using "EntryPoint = "WantedNameForMethod""
```

## Some code from my C Wrapper
```cpp
extern "C" _declspec(dllexport) CppLibrary::GameManager * __stdcall CreateGameManager()
{
	return new CppLibrary::GameManager();
}

extern "C" _declspec(dllexport) void __stdcall UpdateGameManager(CppLibrary::GameManager * gameManager, const float deltaTime)
{
	gameManager->Update(deltaTime);
}
```
Remember the mentioned issue around classes? The GameObject class cannot be easily PInvoked through a C function, solution == pointers!
Passing a pointer to const pointer reference parameter into our method, we can safely create an std::vector<Gameobject*> and pass a pointer to the 1st pointer in this vector, to our parameter.

Later, in C# we can ask what is on the pointer's memory address, giving us the pointer to our Gameobject data, then marshalling said unmanaged data into a formatted class in C#.
```cpp
//passing game obj containers
extern "C" __declspec(dllexport) void __stdcall GetGameObjectContainerCpp(CppLibrary::GameManager * pGame, CppLibrary::ObjectContainerType type, CppLibrary::GameObject* const*& pFirstElement, unsigned long* pLength)
{
	////use gamemanager to get the container with corresponding "type", set the pFirstElement ptr reference to the address of 1st element in container
	if(!pGame->GetGameObjectContainer(type).empty() && pgame->GetGameObjectContainer(type)[0])
		pFirstElement = &pGame->GetGameObjectContainer(type)[0];
	////length of container
	*pLength = unsigned long(pGame->GetGameObjectContainer(type).size());
}
```

Some more bridge functions
```cpp
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

```

## Some code from my C# Library
The PInvoking of my C Wrapped methods
```c#
 //BRIDGE FUNCTIONS

        //GAME MANAGER METHODS
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr CreateGameManager();
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall)]
        public static extern void UpdateGameManager(IntPtr gameManager, float deltaTime);
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.ThisCall)]
        public static extern void SpawnProjectileObjectCpp(IntPtr gameManager, Vector2 pos);
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.ThisCall)]
        public static extern void SetShootDelegateCpp(IntPtr gameManager, Delegate fnc);
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.ThisCall)]
        public static extern void SetKillDelegateCpp(IntPtr gameManager, Delegate fnc);
        //________________________________________________________________________________

        //GET CONTAINER
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall)]
        public static extern void GetGameObjectContainerCpp(IntPtr gameManager, ObjectContainerType type, out IntPtr firstElement, ref ulong Length);
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall)]
        public static extern int GetSizeOfGameObject(IntPtr gameManager);
        //_________________________________________________________________________________

        //PLAYER METHODS-------------------------------------------------------------------
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall)]
        public static extern void GetPlayerObjCpp(IntPtr gameManager, ref GameObject gameObj);
        //Move the player
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern void MovePlayerObjectCpp(IntPtr gameManager, Vector2 direction, float deltaTime);
        //Change player name --> LPString --> ptr to null terminated array of ANSI chars
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern void SetPlayerNameCpp(IntPtr gameManager, [MarshalAs(UnmanagedType.LPStr)] string name);
        //Get player name BSTR prefixed length
        [DllImport("CPPLibrary", CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.BStr)]
        public static extern string GetPlayerNameCpp(IntPtr gameManager);
```
The GetGameObjectContainer() method in C#, marshalling the Gameobject data by using specific Marshal attributes and some pointer arithmetics.
```c#
public List<GameObject> GetGameObjectContainer(ObjectContainerType type)
        {

            List<GameObject> objects = new List<GameObject>();
            IntPtr ptrToPtr = IntPtr.Zero;
            ulong length = 0;
            GetGameObjectContainerCpp(_nativePtr, type, out ptrToPtr, ref length);

            IntPtr ptrToObject = new IntPtr();
            for (ulong i = 0; i < length; i++)
            {
                GameObject gameObject = new GameObject();
                //Get the pointer to the gameobject from the const ptr reference
                ptrToObject = (IntPtr)Marshal.PtrToStructure(ptrToPtr, typeof(IntPtr));
                //Get gameobject from pointer
                Marshal.PtrToStructure(ptrToObject, gameObject); //classes can also be marshalled if it's formatted
                objects.Add(gameObject);

                //x64 --> 8 bytes shift for next ptr
                ptrToPtr = (IntPtr)(ptrToPtr.ToInt64() + 8); 
            }

            return objects;
        }
```

