# GodotYEngine
Collection of helpful classes, variables and methods for making games in the godot engine, as a C++ Godot Module (targetting 4.2+)


## Inspector Buttons

Thanks to [Valla-Chan for the inspiration](https://github.com/Valla-Chan/Godot-Valla-ExportHelper/tree/main) (and GDScript code I ended up using as base). I believe it isn't possible to add Buttons like this to any script using just GDScript without making the script you're adding the buttons to a *@tool*, so I recreated this implementation in this module working around that limitation.

### Usage:
This will create a button in the inspector that will call the button function.
```gdscript
@export var _btn_button:int

func button():
	pass
```
Essentially you make an export variable that starts with `_btn_yourfunctionname`.

![Example gif](http://yarncatgames.com/images/Example_Inspector_button.gif)


## Licensed under MIT https://www.google.com/search?q=mit+license&rlz=1CDGOYI_enCA736CA736&oq=mit+license&gs_lcrp=EgZjaHJvbWUyBggAEEUYOTIKCAEQLhixAxiABDIHCAIQABiABDIKCAMQABixAxiABDIKCAQQABixAxiABDIHCAUQABiABDIKCAYQABixAxiABDIHCAcQABiABDIKCAgQABixAxiABNIBCDE3NjNqMGo5qAITsAIB4gMEGAEgXw&hl=en-US&sourceid=chrome-mobile&ie=UTF-8