AcademicProject_game
====================
STM32 (ARM Cortex-M4)

Update
This is a past project that I have done in my embedded system class in my Junior year. There are 
some missing files for the game such as image files and sound tracks. Also it will only work in a specific discovery board (LCD attached) 
with ARM Cortex-M4. I upload this project here for my own reference. Also if people ever search for some example, I hope 
this repository will offer some helps and welcome to use any code from here.

Below is the the work plan when I was creating the game and a breif discrption of it. The game was created within 3 days due to class schedule
delay. It was challenging but it was fun as well. Since the system was using C, there were many places with nasty code, it will be a lot easier 
with object oriented language. 






This game is about shooting zombies. You are able the aim anywhere in the LCD by
using the joystick on the nunchuk. The leds will be your ammos, you are able to reload.
After a while, there will be a boss pop up.
And you need to shoot the boss in the right timing in order to kill him.
After you killed the boss, you won the game.



Work Plan
1.Convert and test all the pictures and sound files and put them into the SD cards.

2.Create a function to control the aiming dot by using nunchuk.

3.Start by printing the background, and create zombies in the ramdom place.

4.Came up with a method that keep track of each zombie, so that when i shoot them , 
the programm will know which one to clear. There could be multiple zombies on the screen
at the same time(up to 1000).

6.Create some funcions to keep track of the player's health. Player has 3 health portions.
If the screen has 3 or more zombies at the same time, player start losing health.

7.Put in the sound for shooting, reloading and the boss.

8.Create some functions to keep track of the scores.

9.Create a boss fighting mode.

10.Inside the boss mode, create a target bar under the boss, a red dot will keep moving.
In order to hit the boss, you need to press the nunchuk when the red dot is in the middle.

11.If you miss the boss, you will lose health.





Risks
1. In order to clear each zombies, i need to keep track of the x and y coord of each zombies,
otherwise the programm wouldn't know which one to clear. I created a ox array and a oy array.
When the zombies is genrated, I push in the x and y coord into those arrays.

2.My game is highly depending on the speed of the printing bmp. I created a function to 
push a line each time instead of every pixel. It is inside of the f3d_lcd_sd_.c
it is print_line function.

3.The aiming dot on the screen could go out of range if you are not trying to restraint it.
I set up a range so they won't move out of the screen.

4.For general, this game is using a lot of global variables and they are usually being passed by reference.
Also, functions are usually being used in inside of another function.
So is is really easy to mess up the entire programm by pass in an incorrect argument.



I wish I could have more time for this game. So that i could add in more features in it. 
Also, I ran out of time to set up a menu, my functions were set up to take in a variable so that 
the diffcult could be changed. But I don't have enough time to set up a menu for that.