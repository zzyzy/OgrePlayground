# OgreGame2
This project is a small and simple FPS game where the player can shoot projectiles to hit objects in a room/scene. The player can also equip various weapons such as a handgun, shotgun, or a sniper rifle. This project uses Bullet Physics as the main physics engine for the desired physics effects.

## Dependencies
1. OGRE3D 1.9
2. Bullet Physics 2.82

## Building
Currently it only supports building with Visual Studio 2012 and above.
* Remember to set *OGRE_HOME* and *BULLET_HOME* before opening the solution.
* If using a newer platform toolset that is greater than v110 (VS2012), OGRE3D and Bullet may need to be compiled from source.

## What's implemented
* A physics environment (a single room)
* Dynamic objects (crates)
* Different weapons available (implemented with a state machine):
    1. Ball shooter
        * Charges and releases a ball with a variable amount of force depending on the time elapsed to charge the ball before releasing.
    2. Handgun
        * Projects a ray from the camera (with random coordinates from 0.4 to 0.6 in normal coordinates) and applies an impulse to whatever object that the ray hits.
    3. Shotgun
        * Same as handgun, but shoots multiple rays in a single time.
    4. Machine gun
        * Same as handgun, but continuously shoots a single ray until the mouse is released.
    5. Sniper rifle
        * Same as handgun, but has a significantly larger impulse.
        * It also has two levels of zoom which just alters the FOV of the main camera.
    6. Binocular
        * It allows the camera to zoom continuously as the mouse scrolls.
    7. Grenade launcher
        * Same as ball shooter, but the projectile explodes after a delay, which then creates a spherical ghost object and applies impulse to every object that overlaps with the ghost object.

## TODOs
* Setup collision filtering
    * Separate collision detection between the environment and dynamic objects (this is to avoid the character from colliding with ghost objects).
* Code refactoring
    * Currently the physics context is passed around everywhere to any objects that needs it. I think it's kind of ugly to do this, it's like a dependency web or something. I need to try and see if I can come up with a more elegant solution like layering.
* Animation for weapons, probably some gun sprites can be placed and swapped for each frame.
* Particles for grenade and gun impact points.
* Scene loading definition file
    * Construct a scene based on a formatted plain text file (this may be able to share between projects, will be moved to *OgreShared* once its done).
* Look for a way to get the current state of the state machine
    * Currently the state machine just returns a hard-coded text for the current state.
    * I was thinking if I were to use enums for this problem, then I need to add new enums for every new state that I may add in the future.
    * I'd like to avoid that if possible and consider it as the worst case solution.
* Binocular should be able to zoom using mouse scrolling.
* Import new model for grenade projectile.
* For zooming in both the binocular and sniper rifle, it would probably be better if the camera is moved closer instead of adjusting the FOV.
* Use a better UI toolkit or code my own.
* Define a *GameContext* class that comprises of *GraphicsContext* (OGRE3D) and *PhysicsContext* (Bullet).
* *FPSController* should move using Bullet. I think it has something to do with kinematics character controller. May need to take a look.
* Better way of resource loading
    * Currently it just appends the *OGRE_HOME* directory before every resource path.
* Add a *PoolManager* class for managing objects in a pool.
* Separate implementation from definition for weapons.

## References
1. http://gameprogrammingpatterns.com
2. http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Main_Page
3. http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Tutorials
4. Counter Strike 1.6
    * I open this game frequently to make it as similar as possible LOL.