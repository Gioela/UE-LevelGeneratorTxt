# GegLevelGenerator

Geg Level Generator project is a demo plugin for UE 4.27 which aim to generate 
new maps from a simple txt.

Example of a txt map is here:
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

Open the Level Generator Menu

![LevelGeneratorMenu](https://user-images.githubusercontent.com/18500631/176996185-8264862f-a048-4d70-83b9-d5d9cb2fe61b.jpg)

> The Menu exposes the following parameters:


After clicked on Load Button and then on Generate Button, your new level will be found in this path:

![NewMapCreatedPath](https://user-images.githubusercontent.com/18500631/176996193-bf71890b-cdd3-4cae-822a-529a6907bd4c.jpg)


Finally, opened the level, it is possible to see the new Level Map Asset:
![FinalExampleOfMap](https://user-images.githubusercontent.com/18500631/176996170-19a4528d-cd5c-4260-a0b1-7121be342bc8.jpg)


### Note
This project is licensed under the terms of the MIT license.
