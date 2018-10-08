## What is this? 
This is an example program for Mbed OS that sends [MessagePack](https://msgpack.org/) encoded data to a [FLuentD](https://www.fluentd.org/) server. 

## Why would I use this? 
MessagePack encoding uses less space than JSON, this means less data used to transmit the same amount of information, which means power savings on radio transmission, money savings on data rates, and time savings on amount of time the MCU must be awake. 

## How to use
Setup a fluentd server, get your treasure data API key and then run the following. 

### FluentD Setup
Use the included example_fluentd.conf fil to configure your instance. This will setup two tags, 
- `debug.*` - this will print out anything to the serial terminal of the fluentd instance
- `td.fluentd_database.*` - this will forward anything under this tag to your Treasure Data account. Make sure to replace the api-key field in the config file with your key from TreasureData. 

### build embedded app
Run the example code on your device. You can either [import to the online compiler](http://os.mbed.com/compiler/?import=https%3A%2F%2Fgithub.com%2FBlackstoneEngineering%2Fmbed-os-example-fluentlogger) or use mbed CLI as below to clone it locally, compile and load it to the board. 

Make sure to change the ssid and password in `main.cpp` to match your local network. 

```shell
$ mbed import https://github.com/BlackstoneEngineering/mbed-os-example-fluentlogger 
$ mbed compile --target auto --toolchain GCC_ARM --flash --sterm
```

### How to set TreasureData database / table
The database is determined by the second field in the tag of your embedded code.
For example, sending data to a tag called `td.mydatabase.mytable` will log the data to the database called `mydatabase` in the table `mytable`. 

Feel free to modify the example config file to try this out. 

## Liscense
Apache 2.0, as always.