# scrimbloInput
a simple keyboard input display for linux using X11 and SDL3<br>
<br>
also check out [scrimbloTimer](https://github.com/kittrz9/scrimbloTimer)<br>

## building
to build you should just be able to run `./build.sh` in this directory, it will download a version of SDL3 and compile it. however it will require you to have cmake installed to build SDL3.<br>

## usage
to use it you need to have a configuration file created along with an image containing all the keys in a line with the pressed version of the image below them, see `scrimbloInput.conf` and `keys.png` as an example<br>
<br>
you should then run it from the command line like `./scrimbloInput (config file path)`, the image should also be in the same path if it is relative in the config file<br>
