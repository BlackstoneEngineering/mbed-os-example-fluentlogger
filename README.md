## What is this? 
This is an example program for Mbed OS that sends [MessagePack](https://msgpack.org/) encoded data to a [FLuentD](https://www.fluentd.org/) server that then forwards the messages to Treasure Data. 

## Why would I use this? 
MessagePack encoding uses less space than JSON, this means less data used to transmit the same amount of information, which means power savings on radio transmission, money savings on data rates, and time savings on amount of time the MCU must be awake. 

## How to use
You have two options for using this example. You can either use a raw TCP socket to connect to the fluentd server (no security), or you can use a TLS socket to connect to the fluentd server (lots of security). The default choice is to use the TLS socket. 

In either case you will need to setup your fluentd server and get your Treasure Data API key. 

### FluentD Setup
Use the included example_fluentd.conf fil to configure your instance. This will setup two tags. You can use it by running fluentd as follows `fluentd --config ./fluentd.conf -vv`

In the config file
- `debug.*` - this will print out anything to the serial terminal of the fluentd instance
- `td.fluentd_database.test` - this will forward anything under this tag to the `test` table in the `fluentd_database` in your Treasure Data account. Make sure to replace the api-key field in the `fluentd-setup/fluentd.conf` config file with your key from TreasureData. Also make sure to create a database called `fluentd_database` in your treasure data account. 

#### Security 
The reccomended method of using this example is to use security and enable TLS. In the default config file this is done on port `24228`. You can either run the fluentd server locally on your machine (localhost) with self signed certificates, or you can run it in the cloud at a publically accessable IP with certificates signed by a proper CA. See the below subsections for instructions for both. (I will assume you are going to run it as localhost at first, so that is where I will focus the majority of my instructions)

##### Signed by CA, running in cloud

If you have valid certs from a CA replace the `fluentd.crt` and `fluentd.key` files with them. Then uncomment the lines in the `fluentd.conf` file for CA trusted certs, comment out the lines for self signed certs, and change the passphrase to match for your certificate.

```
    # cert_path ~/mbed-os-example-fluentlogger/fluentd-setup/fluentd.crt
    # private_key_path ~mbed-os-example-fluentlogger/fluentd-setup/fluentd.key
    # private_key_passphrase YOUR_PASSPHRASE
```

##### Self signed certs on localhost

If you are running the fluentd server locally on your machine for bringing up a PoC you will need to configure a few things.

1) Change the `generate_cert_common_name` parameter in `fluentd.conf` to be the IP address of the computer running the fluentd server
1) Run ` openssl req -new -x509 -sha256 -days 1095 -newkey rsa:2048 -keyout fluentd.key -out fluentd.crt` to generate new certificates. When entering the prompted values make sure to match the parameters in the `fluentd.conf` file(US, CA, Mountain View...etc). **Make sure the Common Name (CN) field is set to the IP address of the fluentd server**


### Mbed OS setup
Run the example code on your device. You can either [import to the online compiler](http://os.mbed.com/compiler/?import=https%3A%2F%2Fgithub.com%2FBlackstoneEngineering%2Fmbed-os-example-fluentlogger) or use mbed CLI as below to clone it locally, compile and load it to the board. 

```shell
$ mbed import https://github.com/BlackstoneEngineering/mbed-os-example-fluentlogger 
$ mbed compile --target auto --toolchain GCC_ARM --flash --sterm
```

#### Unsecure (TCP)
To send data to fluentd over TCP uncomment the line  below in `main.cpp`. 
```C++
// FluentLogger logger(net,"192.168.1.85",24227);  // Send unsecure data over TCP socket
```
Make sure that the port `24227` matches the setup in `fluentd.conf`.

#### Secure (TLS)
To send data  to fluentd over TLS (securely) we will need to configure a few things. 

1) run `openssl s_client -connect localhost:24228 -showcerts`, copy the certificate to `fluentd-sslcert.h`. If running the fluentd server on localhost then this certificate will change every time you restart fluentd, so you will need to re-run this command every time and recompile your embedded code. 
1) modify the call in `main.cpp` to the FluentLogger object, change the IP address to the IP address of the fluentd server, or if you are hosting it in the cloud change it to the web address where it is hosted. **It is very important that the IP address in the main.cpp file matches the IP address set in the CN field fo the fluentd server, otherwise it will not work as mbed tls uses strict CN verification**
1) Compile the code and load it onto your board. 


### Success
When everything is working you should be output on the fluentd terminal that looks like the following:

```sterm
 -0500 debug.test: ["sint",0,1,-1,-128,-32768,-2147483648]
 -0500 [trace]: #0 fluent/log.rb:281:trace: connected fluent socket addr="192.168.1.95" port=5522
 -0500 [trace]: #0 fluent/log.rb:281:trace: accepted fluent socket addr="192.168.1.95" port=5522
 -0500 debug.test: ["uint",0,1,128,255,65535,4294967295]
 -0500 [trace]: #0 fluent/log.rb:281:trace: connected fluent socket addr="192.168.1.95" port=5523
 -0500 [trace]: #0 fluent/log.rb:281:trace: accepted fluent socket addr="192.168.1.95" port=5523
 -0500 [trace]: #0 fluent/log.rb:281:trace: enqueueing all chunks in buffer instance=70248976563020
 -0500 debug.test: {"string":"Hi!","float":0.3333333432674408,"double":0.3333333333333333}
 -0500 [trace]: #0 fluent/log.rb:281:trace: connected fluent socket addr="192.168.1.95" port=5524
 -0500 [trace]: #0 fluent/log.rb:281:trace: accepted fluent socket addr="192.168.1.95" port=5524
 -0500 debug.test: {"string":"Hi!","float":0.3333333432674408,"double":0.3333333333333333}
 -0500 [trace]: #0 fluent/log.rb:281:trace: connected fluent socket addr="192.168.1.95" port=5525
 -0500 [trace]: #0 fluent/log.rb:281:trace: accepted fluent socket addr="192.168.1.95" port=5525
 -0500 [trace]: #0 fluent/log.rb:281:trace: adding metadata instance=70248976563020 metadata=#<struct Fluent::Plugin::Buffer::Metadata timekey=nil, tag="td.fluentd_database.test", variables=nil>
 -0500 [trace]: #0 fluent/log.rb:281:trace: writing events into buffer instance=70248976563020 metadata_size=1
 -0500 [debug]: #0 fluent/log.rb:302:debug: Created new chunk chunk_id="585c249fd2ebe20867267de2fde7c4bc" metadata=#<struct Fluent::Plugin::Buffer::Metadata timekey=nil, tag="td.fluentd_database.test", variables=nil>
 -0500 [trace]: #0 fluent/log.rb:281:trace: connected fluent socket addr="192.168.1.95" port=5526
 -0500 [trace]: #0 fluent/log.rb:281:trace: accepted fluent socket addr="192.168.1.95" port=5526
 -0500 debug.test: {"string":"Hi!","float":0.3333333432674408,"double":0.3333333333333333}

```

#### Configure 

### How to set TreasureData database / table
The database is determined by the second field in the tag of your embedded code.
For example, sending data to a tag called `td.mydatabase.mytable` will log the data to the database called `mydatabase` in the table `mytable`. 

Feel free to modify the example config file to try this out. 

## Debugging

For more verbose debug messages turn on the following flags in `mbed_app.json`

```
MBEDTLS_SSL_DEBUG_ALL=1

"mbed-trace.enable" : true
```

## Liscense
Apache 2.0, as always.