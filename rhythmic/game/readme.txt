Rhythmic is a rhythm game that behaves similar to Guitar Hero or Rock Band. The end goal of
this project is to be able to have a good playing rhythm game with the ability to modify
the way it looks. 

Features can be requested in the feature-requests channel on the discord server
https://discord.gg/CR54Pdx


Please report bugs and glitches.

If you can, please also include steps to perform the bug (it helps me get rid of it faster)

-------------------------------------------------------------------------------------------

 - Rhythmic supports
	- 4 lane drums with and without cymbals
	- 5 lane drums (with cymbals as a visual)
	- 5 fret guitar/bass
	
 - Rhythmic only supports both .chart and .mid files and will look for a song.ini
		If Rhythmic doesn't find one, it'll ignore it. If the chart is a .chart, Rhythmic
	  will generate a simple song.ini
	  
-------------------------------------------------------------------------------------------

Rhythmic allows users to customize bindings. To access this, make sure you are at the
main menu and double click. This will pull up an overlay where you can add and remove
bindings to your liking. 

There are three tabs to choose from. Bindings, Save Binding, and Load Binding

The Bindings tab allows you to modify the bindings

The Save Binding tab allows you to save your binding
	- Click on empty box, and give it a name.
	- Click on the Bind Controller button if you want the binding file to be tied
	   to your controller (only works with controllers)
		*Will be highlighted blue if it's enabled*
	- Then click on Save... to save the binding

The Load Binding tab will show all available bindings
	Just click on one of the available bindings to load it in


If the start button on your controller isn't signing you in, there is a button in the
binding overlay that allows you to press any button to sign in.

If this button isn't working, it's likely that SDL isn't recognizing your controller


Default keyboard bindings:

	F		- pad 1 / accept
	G		- pad 2 / go back
	H		- pad 3
	J		- pad 4
	K		- pad 5
	space		- kick / open notes (only when No Strum is enabled)
	enter		- pause
	escape		- pause
	left ctrl	- whammy
	left shift	- activate starpower

Controllers
	
	Controllers can be tied to a binding save. This makes it easier to use your
	controller and not have to load the binding anytime you want to use your
	controller. 

	If a controller isn't tied to any binding save, the game will use default
	bindings for controllers. The default binding should work with X-Plorer
	guitars. 

