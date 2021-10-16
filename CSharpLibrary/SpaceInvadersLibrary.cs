using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace CSharpLibrary
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    { public float X, Y; };

    [StructLayout(LayoutKind.Sequential)]
    public class GameObject
    {
        public Vector2 Position;
        public Vector2 Size;
    }

    public enum ObjectContainerType
    {
        Alien,
        Projectile
    }

    public class SpaceInvadersLibrary : IDisposable
    {
        //Datamembers
        private readonly IntPtr _nativePtr = IntPtr.Zero;
        private bool _disposed = false;

        //Constructor - Finalizer
        public SpaceInvadersLibrary()
        {
            _nativePtr = CreateGameManager();
        }

        ~SpaceInvadersLibrary()
        { Dispose(false); }

        //IDisposable interface
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool bDisposing)
        {
            if (!this._disposed)
            {

                this.Dispose();
                this._disposed = true;
            }
        }

        //PUBLIC FUNCTIONS
        public void Update(float deltaTime)
        {
            UpdateGameManager(_nativePtr, deltaTime);
        }

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
                Marshal.PtrToStructure(ptrToObject, gameObject);
                objects.Add(gameObject);

                //x64 --> 8 bytes shift for next ptr
                ptrToPtr = (IntPtr)(ptrToPtr.ToInt64() + 8); 
            }

            return objects;
        }

        public GameObject GetPlayerObject()
        {
            GameObject gameObject = new GameObject();
            SpaceInvadersLibrary.GetPlayerObjCpp(_nativePtr, ref gameObject);

            return gameObject;
        }

        public void SetPlayerName(string name)
        {
            SetPlayerNameCpp(_nativePtr, name);
        }

        public string GetPlayerName()
        {
            return GetPlayerNameCpp(_nativePtr);
        }

        public void MovePlayerObject(Vector2 direction, float deltaTime)
        {
            MovePlayerObjectCpp(_nativePtr, direction, deltaTime);
        }

        public void SpawnProjectileObject(Vector2 position)
        {
            SpawnProjectileObjectCpp(_nativePtr, position);
        }

        public void SetShootDelegate(Delegate fnc)
        {
            SetShootDelegateCpp(_nativePtr, fnc);
        }

        public void SetKillDelegate(Delegate fnc)
        {
            SetKillDelegateCpp(_nativePtr, fnc);
        }

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
        //______________________________________________________________________________

    }
}
