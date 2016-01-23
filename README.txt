This repository contains four unrelated, incomplete projects from varying times within the past four years.

SHIPWRECK:
	
	(WASD to move, Q and E to spin, hold LSHIFT to use jetpack and follow tooltips in top-left of screen for everything else) Partially-complete application of a custom physics engine able to detect and respond to collisions, simulate rope physics, and simulate gas flow in an environment with dozens of travelling, spinning entities of varying size and mass. The game also allows one to connect said entities together with joints to build larger constructs, and additionally, one can build simplified electrical systems requiring the transmission and transformation of virtual power. This application was going to be a simple game loop where the player starts in the middle of a decrepit space station or shipwreck in space, and has to survive asteroid collisions while protecting a system of powered "beacons" for ten uninterrupted minutes so that the "rescue team" can save the player, ending the game. The version in this repo has asteroid showers disabled (as it became difficult to test changes during development while being pelted with asteroids). 

FLEET:

	(LMB to draw and place cards, hold RMB and drag to move view, mousewheel to scale view) Computer trading-card game of my own design, built after falling in love with the "Gwent" mini-game in CD Projekt Red's "Witcher 3: Wild Hunt." I wanted to try out various trading-card game mechanics I had ideas for, such as in-match deck building by timed drawing, geometrical advantages in placement, and an infinite gameboard. I also was interested in trying my hand at properly programming a simple architecture for multiplayer (I'm a big fan of LAN play, something that doesn't get included as much anymore in many titles) and that feature is functional in this version. However, the single player mode doesn't have any AI to play against, and despite 50+ unique cards programmed into the game, you'll notice there aren't exactly 50+ unique artworks for the card faces.   (Click on glowing rectangles in-match to place cards, click cards at top of screen when not in cooldown mode to add to your hand)

ISO_TEST:

	(Use WASD to scroll camera, Q and E to rotate view) Graphics excercise. Used OpenGL to upgrade SFML to display four different isometric views of a 3D game map, while still using the base 2D sprite class provided by SFML. Developed the ability to identify selected building faces from cursor position, and also wrote an accompanying script in Python to rapidly convert a 3D blender model into the set of necessary image resources for building the "3D" model in the isometric view.

KEEP_QUIET:

	('a' and 'd' to move, spacebar to jump, return key to enter a ship, WASD to accelerate ship, spacebar to eject out of ship, 'g' to toggle ship's antigravity thrusters) Simple sidescroller frame using a combination of unrestricted physical entities and grid-locked tiles for terrain. Implemented collision between entities and between entities and terrain, and developed a method to make the sidescrolling "level" appear infinite - that is, if you run far enough in one direction, you arrive back at your starting point in a visually seamless manner. The graphics layers parallax scroll on both axes. I was recently working on this project as a form of city-building strategy game, but after re-evaluating the design document I decided to axe the city-building part and focus only on the "flying ships around to defend your base" element.


Any one of these projects may be tested with the "debug.exe" application in said project's folder.