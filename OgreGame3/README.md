# OgreGame3
This project is a RTS game which demos the A* path finding algorithm.  
Do note that some TODOs in *OgreGame2* is implemented here.
```
Basic controls
==============

0     - Show grid and visualize the path found by A*
SPACE - Reset camera position
SHIFT - Add/remove selection
RMB   - Move to a target position or follow a target object
LMB   - Select object
H     - Hide fps and Ogre logo
```

## Dependencies
1. OGRE3D 1.9
2. Bullet Physics 2.82

## Building
Currently it only supports building with Visual Studio 2012 and above.
* Remember to set *OGRE_HOME* and *BULLET_HOME* before opening the solution.
* If using a newer platform toolset that is greater than v110 (VS2012), OGRE3D and Bullet may need to be compiled from source.

## What's implemented
1. Seek steering behaviour
2. Path finding
3. Path following
4. RTS game style camera
5. Select multiple entities with a selection box
6. CTRL key to add or remove entities to selection
7. Display selected circle under selected object
8. Seek steering rotation
9. RTS camera zoom

## TODOs
1. Path following, try the following:
    * Use radius check instead of distance
2. Seek steering smooth rotation
3. Better unique random generator for placement of obstacles and robots
4. Add path using SHIFT key (like WC3's waypoint)

## References
1. Warcraft III
    * How it does box selection
2. http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Intermediate+Tutorial+1&structure=Tutorials
3. http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Intermediate+Tutorial+3&structure=Tutorials
4. http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Intermediate+Tutorial+4&structure=Tutorials
5. Programming Game AI by Example (Seek steering behaviour)
6. http://answers.unity3d.com/questions/467614/what-is-the-source-code-of-quaternionlookrotation.html
7. http://www.ogre3d.org/tikiwiki/Quaternion+and+Rotation+Primer
