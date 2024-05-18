# TAL CardReader

This is a plugin that allows you to use CardIO and SmartCard readers with [TaikoArcadeLoader](https://github.com/BroGamer4256/TaikoArcadeLoader)

# Acknowledgments

This is a plugin destined to be used with [BroGamer4256](https://github.com/BroGamer4256)'s TaikoArcadeLoader. More specifically with esuo1198's [fork](https://github.com/esuo1198/TaikoArcadeLoader)

CardIO and SmartCard implementation taken from [Spicetools](https://github.com/spicetools/spicetools) and adapted with the help of [CrazyRedMachine](https://github.com/CrazyRedMachine) (Huge thanks!)

Initial project setup done with the help of [Stepland](https://github.com/Stepland)

# How to use

Copy cardreader.dll and it's config file to your TAL's plugin folder (\Executable\Release\plugins\). You should be able to see the plugin initializing in the game's console.

*There are two versions of this plugin :*

* For JPN or ASIA dumps, use the [CARD Insert](https://gitea.farewell.dev/AkaiiKitsune/tal-cardreader/releases/download/CARD-Fix/cardreader-card.zip) build.
* If you're playing on CHN instead, use the [QR Insert](https://gitea.farewell.dev/AkaiiKitsune/tal-cardreader/releases/download/QR-Fix/cardreader-qr.zip) build.

If there's an error when you log-in, make sure you're using the right version for your game dump and that you're using an updated version of esuo1198's TAL fork (Commit [3920cc2](https://github.com/esuo1198/TaikoArcadeLoader/commit/3920cc2) and above !).

# Settings

- using_smartcard (default : false)
  * _Reader type defaults to CardIO, if you want to use a SmartCard reader, set this to true._

- read_cooldown (default : 20)
  * _Cooldown between each read attempt, a low value (below 15) can lead to game crashes, a high value (above 500) will break SmartCard readers._

# Compiling

To build this, you'll need two things :

- [Meson 1.1.0](https://mesonbuild.com)
- [Build Tools pour Visual Studio 2022](https://visualstudio.microsoft.com/fr/downloads/)

Once you've edited your build64.bat file to point to your local installation of the VS2022 build tools, you should be good to go.
