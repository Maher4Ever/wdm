# Windows Directory Monitor (WDM)

Windows Directory Monitor (WDM) is a library which can be used to monitor directories for changes.
It's mostly implemented in C and uses the Win32 API for a better performance.

**Note:** This is still a work in progress, so it's not advisable to use
it yet in anything (unless you are testing it, which is very much appreciated :)).

TODO:

- Fix all the TODO's in the source.
- Add options to the `watch` method.
- ~~Provide info about the change in the callback.~~
- ~~Convert \ to / in paths.~~
- ~~Don't allow directories to be watched while the monitor is running.~~
- ~~Check if the passed direcoty exists.~~
- ~~Convert passed directories to absolute paths.~~

## Installation

Add this line to your application's Gemfile:

    gem 'wdm'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install wdm

## Compiling the extension for developers

Download the source, then run the following:

	$ rake compile

To get debug messages, you need to enable them in the `global.h` file:

	#define WDM_DEBUG_ENABLED TRUE // This is disabled by default

## Usage

TODO: Write usage instructions here

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

## Author

[Maher Sallam](https://github.com/Maher4Ever)