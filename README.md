# NewClockWorkEngine
 
![Group Logo](Group_Logo_and_Icon/Logo_BW_Basic.png)
 
NewClockWorkEngine is a game engine developed by three UPC students going through Video Games Bachelor's Degree.

This delivery includes an Audio Module using Wwise.


## Members:

### Alex Lopez
* [Github Profile](https://github.com/AlexLA99)

### Victor Nisa
* [Github Profile](https://github.com/victornisa)

### Daniel Ruiz
* [Github Profile](https://github.com/xsiro)


## Controls:

### Mouse and Camera controls
- M1: Select mode
	- Shift: Add object to selection
	- Ctrl: Remove object from selection
- M2: Enter fps mode
	- Mouse: Rotate view
	- WASD: Camera movement
	- E: Move up
	- R: Move down
	- Shift: Move faster
- M3: Pan view
- F: Focus selected object
- Scroll wheel: Zoom
- Alt + M1: Orbital view

### Menu and interaction controls
- Del: Deletes selected object
- Esc: Exit menu
- Q: Hide Gizmos
- W: Move
- E: Rotate
- R: Resize

## Features
- Wwise implementation, allowing spatial audio and special effects
- Audio Listener and Source
- Support for multiple listeners, controlled by the MainListener flag
- Added a reverb effect that can be applied to a specific zone
- Pitch variation

## Instructions
- Create an audio listener (and set it to current listener)
- Create an audio source component
- Set its event name to one of the events inside our soundbank
	- For example, "Play_Laugh"
- Then, you should be able to hear it
- Using the engine's play button, you can check the spatial audio
