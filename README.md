![Game Banner](./assets/banner.png)

# 🏇🏿 COGS Pixel Race

Recreation of the [horse race tests](https://tvtropes.org/pmwiki/pmwiki.php/WebOriginal/HorseRaceTests)!

## Developing

- Open up `game` in Godot
- Download the `godot-cpp` submodule using

```bash
git submodule update --recursive
```

- Build the C++ `GDExtension` code using

```bash
scons
```

> [!NOTE]
> 
> The custom `GDExtension` must be built for every type of platform you'd like to export to, as well as the target, which is either debug or release.
>
> The difference between debug and release targets occurs when you check the "Export With Debug" option when you export the project. If this option is checked, then the game will be in debug mode, and contain extra information and logging useful for debugging your exported game. If it's unchecked, then the game will strip out any debug information, making the game smaller. However, this makes the game harder to debug if it crashes.
> 
> By default, running `scons` will build a debug build for your computer's platform.
> You can specify the platform using `platform=<platform>` flag, and you can specify the target using the `target=<target>` flag.
>
> **Ex.** the following command builds the `GDExtension` for windows release builds.
> ```bash
> scons platform=windows target=template_release
> ```
> **Ex.** the following command builds the `GDExtension` for windows debug builds.
> ```bash
> scons platform=windows target=template_debug
> ```
> You can find more information on the [buildsystem page](https://docs.godotengine.org/en/stable/contributing/development/compiling/introduction_to_the_buildsystem.html) and the [gdextension example page](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html) on the Godot documentation.

## Creating Maps
              
To create a custom map, create a new inherited scene of the `base_map.tscn` by right clicking on the `base_map.tscn` and selecting "New Inherited Scene". Then, place the newly created scene inside the `prefabs/maps` folder, and then add to the list of maps in the RaceSettings node in `main.tscn`.

## Creating Contestants

There are two types of contestants

- Sprite contestant
    - Creates a complete contestant using just a 32x32 image
    - The name of the sprite is based on the image's file name
    - The icon of the sprite is the image
    - The color of the sprite is the most frequent color in the image
- Custom contestant
    - Creates a contestant from a scene, allowing for greater flexibility
    - You can manually define the contestant's color, name, and icon

To create a sprite contestant, import the image into `assets/sprites/contestants`, and then add the image to the list of sprite contestants in the RaceSettings node in `main.tscn`.

To create a custom contestant, create a new inherited scene of `base_pixel_contestant.tscn` by right clicking on `base_pixel_contestant.tscn` and selecting "New Inherited Scene". Create the new scene inside the `prefabs/contestants` folder, and then add it to the list of custom contestants in the RaceSettings node in `main.tscn`.