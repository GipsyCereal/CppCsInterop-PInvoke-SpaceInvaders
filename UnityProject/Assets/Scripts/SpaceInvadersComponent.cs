using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Runtime.InteropServices;
using CSharpLibrary;

public class SpaceInvadersComponent : MonoBehaviour
{
    private int Width = 480;
    private int Height = 800;
    private SpaceInvadersLibrary Game;

    public String DefaultName = "Player";
    public Mesh DefaultMesh;
    public Material AlienMaterial;
    public Material PlayerMaterial;
    public Material ProjectileMaterial;
    public AudioClip ShootingClip;
    public AudioClip KillClip;
    private AudioSource AudioSourceComponent;

    delegate void ShootDelegateType();
    private ShootDelegateType ShootDelegate;
    delegate void KillDelegateType();
    private KillDelegateType KillDelegate;

    void PlayShootingSound()
    {
        if(AudioSourceComponent)
            AudioSourceComponent.PlayOneShot(ShootingClip);
    }

    void PlayKillSound()
    {
        if (AudioSourceComponent)
            AudioSourceComponent.PlayOneShot(KillClip);
    }

    // Use this for initialization
    void Start ()
	{
        //Set resolution for build
        Screen.SetResolution(Width, Height, false);
        //Get component
	    AudioSourceComponent = GetComponent<AudioSource>();

        //Create game
        Game = new SpaceInvadersLibrary();
        //Initialize
        Game.SetPlayerName(DefaultName);
        //Link sound functions
	    ShootDelegate = PlayShootingSound;
	    KillDelegate = PlayKillSound;
        Game.SetShootDelegate(ShootDelegate);
        Game.SetKillDelegate(KillDelegate);
	}
	
	// Update is called once per frame
	void Update ()
	{
	    //********** UPDATE **********
        //Move player
	    float xMovement = Input.GetAxis("Horizontal");
	    CSharpLibrary.Vector2 playerDirection;
	    playerDirection.X = xMovement;
	    playerDirection.Y = 0.0f;
        Game.MovePlayerObject(playerDirection, Time.deltaTime);

        //Spawn Projectiles
	    if (Input.GetKeyUp(KeyCode.Space))
	        Game.SpawnProjectileObject(Game.GetPlayerObject().Position);

	    //Update game
            Game.Update(Time.deltaTime);

        //********** DRAWING **********
        //DRAW Aliens
        var aliens = Game.GetGameObjectContainer(ObjectContainerType.Alien);
	    foreach (var obj in aliens)
	    {
            Matrix4x4 scaleAlien = Matrix4x4.Scale(new Vector3(obj.Size.X, obj.Size.Y, 1.0f));
            Matrix4x4 translateAlien = Matrix4x4.Translate(new Vector3(obj.Position.X, obj.Position.Y, 0.0f));
	        Matrix4x4 finalAlien = translateAlien * scaleAlien;
	        Graphics.DrawMesh(DefaultMesh, finalAlien, AlienMaterial, 0);
        }

        //DRAW Player
	    var player = Game.GetPlayerObject();
	    Matrix4x4 scalePlayer = Matrix4x4.Scale(new Vector3(player.Size.X, player.Size.Y, 1.0f));
	    Matrix4x4 translatePlayer = Matrix4x4.Translate(new Vector3(player.Position.X, player.Position.Y, 0.0f));
	    Matrix4x4 finalPlayer = translatePlayer * scalePlayer;
        Graphics.DrawMesh(DefaultMesh, finalPlayer, PlayerMaterial, 0);

        //DRAW Projectiles
	    var projectiles = Game.GetGameObjectContainer(ObjectContainerType.Projectile);
	    foreach (var obj in projectiles)
	    {
	        Matrix4x4 scaleProjectile = Matrix4x4.Scale(new Vector3(obj.Size.X, obj.Size.Y, 1.0f));
	        Matrix4x4 translateProjectile = Matrix4x4.Translate(new Vector3(obj.Position.X, obj.Position.Y, 0.0f));
	        Matrix4x4 finalProjectile = translateProjectile * scaleProjectile;
	        Graphics.DrawMesh(DefaultMesh, finalProjectile, ProjectileMaterial, 0);
	    }
    }

    void OnGUI()
    {
        //Player name
        var player = Game.GetPlayerObject();
        var playerName = Game.GetPlayerName();
        var playerPositionScreenSpace = Camera.main.WorldToScreenPoint(new Vector3(player.Position.X, player.Position.Y, 0.0f));
        GUI.Label(new Rect(playerPositionScreenSpace.x - 100, Screen.height - (playerPositionScreenSpace.y - 10), 200, 50),
            playerName);
    }
}
