# TAL CardIO

This is a plugin that allows you to use USB HID card readers (cardio) with [TaikoArcadeLoader](https://github.com/BroGamer4256/TaikoArcadeLoader)

# Acknowledgments

This is a plugin destined to be used with [BroGamer4256](https://github.com/BroGamer4256)'s TaikoArcadeLoader.

CardIO implementation taken from [Spicetools](https://github.com/spicetools/spicetools) and adapted with the help of [CrazyRedMachine](https://github.com/CrazyRedMachine) (Huge thanks!)

Initial project setup done with the help of [Stepland](https://github.com/Stepland)

# How to use

To build this, you'll need two things :

- [Meson 1.1.0](https://mesonbuild.com)
- [Build Tools pour Visual Studio 2022](https://visualstudio.microsoft.com/fr/downloads/)

Once you've edited your build64.bat file to point to your local installation of the VS2022 build tools, you should be good to go.

Copy the cardio.dll to your TAL's plugin folder. You should be able to see the plugin initializing in the game's console.
