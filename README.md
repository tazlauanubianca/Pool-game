# Pool game

The purpose of the project is to create a classic "8 Ball Pool" game with 2 players, which uses 7 red balls, 7 yellow balls, a black ball and a white ball. Example: https://www.youtube.com/watch?v=6uzf1nXZW6E

Stadii
1. Ball breaking
They will be arranged in a triangle consisting of the 15 balls in the third third of the table, and the white ball is located in the first third of the table.

The beginning player will be able to move the white ball in the first third of the table. After moving the ball where you want with the "WASD" keys, press "SPACE".

At first, the cue is very close to the white ball. The cue will have a retraction animation (in the cue direction) when the "Left Mouse Button" is pressed. This animation will be made in vertex shader. After the cue has reached a certain distance from the white ball, it returns close to the white ball and the animation resumes. The intensity of the shot will be directly proportional to the distance between the cue and the white ball. When you release the left mouse button, the shot is executed. Also, with "Right Mouse Button" you can move the cue around the white ball, the camera tracking the white ball and the cue as in third person mode. The direction of the blow is given by the direction of the bush. After the shot, the white ball will have a certain speed with the direction of the cue and the magnitude proportional to the distance between the cue to the white ball at the time of the release of the left mouse button. When the white ball collides with the other balls, they will have to behave as in real life. The movement of the white ball is in the XOZ plane.

2. Choosing the color of each player

The color a player plays is chosen when the player enters the first yellow or red ball into his pocket. After that, the respective player must insert all the balls of the respective color in the pocket, and the other player must introduce the balls of the other color.

3. The game until the black ball

Players must put all the balls in their pocket. A ball is inserted into the pocket when it gets so close to a pocket that we can draw that conclusion. After that, they can enter the black ball. Whoever introduces the first black ball is the winner. If a foul is committed, the other player can place the white ball anywhere on the table with the WASD keys (as long as no other balls intersect). The fault is of two types: if a player, at one stroke, does not touch his own balls for the first time or if the white ball enters his pocket. After placing the white ball on the table with a top-down view, press SPACE to return to the third person room and the game continues.
	
The game was implemented in Lab 7 and can be run from the main file. The main file is now configured to run the game.

Players will play in turn, trying to introduce the balls color will be set for each player after the first ball has been introduced.
