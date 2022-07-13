# GegLevelGenerator

Geg Level Generator project is a demo plugin for UE 4.27 which aim to generate 
new maps from a simple txt.

## How to use
It can be used via:
- Commandlet
- like simple UE Plugin

## Requiremets and Notice
1. **This plugin cannot be used in production. At least it could be necessary a license because to open the file dialogue window is called a class which is in the Developer headers.**

2. The txt file must be formatted as a square or rectangle with all positions 
filled and must contain these types of values: 0, 2, 3 (like the following example).

```
3,3,3,3,3,3,3,3,3,3,3
3,0,0,2,0,3,0,0,0,0,3
3,0,0,2,0,3,0,0,0,0,3
3,0,0,2,0,3,0,0,0,0,3
3,3,3,3,2,2,2,3,3,3,3
3,0,0,0,0,0,0,2,0,0,3
3,2,2,2,2,2,2,2,0,0,3
3,0,0,0,0,0,0,2,0,0,3
3,3,3,3,3,3,3,3,3,3,3
```

LEGEND:
 - 0 -> Floor Tile
 - 2 -> Breakable Wall
- 3 -> Unbreakable Wall


### Commandlet Use
To use it via commandlet it's necessary:
1. add the plugin to UE project (obviously)
2. go to UE installed path
3. execute the command line:
```
UE4Editor-Cmd.exe {CustomProjectPath with the uproject file} GegLevelGeneratorCommandlet {Absolute path where is the txt map file}
```
It will create a new map with the name **Level_{GUID}** in the path:
```
/Game/GegLevelGenerator/Maps/Level_{GUID}/
```
**NOTE:**
The generated Map will have the default assets and, for the moment, no one material.

### Plugin Use
If you want to customize materials or assets, it's possible to use it via UE Editor
like a simple plugin. To do this:
1. Select the "Level Generator" in Window > Developer Tools > Miscellaneous to open the plugin's UI
2. Customize assets as you prefer (for materials it's possible to choose both Material and Material Instance)
3. Load txt file from button "Load Tiled File"
4. Generate Level by clicking on the same button

The new map will be generated with the same logic in the same path as in Commandlet Use Paragraph explaination.

**NOTE:**
if some asset is not specified, the Plugin will use the relative default object.


**Plugin Use Example**

Open the Level Generator Menu

![LevelGeneratorMenu](https://user-images.githubusercontent.com/18500631/176996185-8264862f-a048-4d70-83b9-d5d9cb2fe61b.jpg)

> The Menu exposes the following parameters:
...


After clicked on Load Button and then on Generate Button, your new level will be found in this path:

![NewMapCreatedPath](https://user-images.githubusercontent.com/18500631/176996193-bf71890b-cdd3-4cae-822a-529a6907bd4c.jpg)


Finally, opened the level, it is possible to see the new Level Map Asset:

![FinalExampleOfMap](https://user-images.githubusercontent.com/18500631/176996170-19a4528d-cd5c-4260-a0b1-7121be342bc8.jpg)


### Note
This project is licensed under the terms of the MIT license.
