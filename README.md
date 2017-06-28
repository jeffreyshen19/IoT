# Internet of Things

Codebase storing projects written for the Intel Edison platform as part of an externship with Dr. Peter Reiher


## Table of Contents
Each project is stored in an individual folder:

* **Alarm**: Monitors ambient sound level to detect disturbances. If it hears one, it sets off an LED, buzzer, and sends a message to the server. It can accept commands sent over an SSL connection. 
* **Archive**: Old code that doesn't work but is there for reference.
* **Chat**: A chat client that enables 3 Intel edisons to communicate: 2 clients and one server.
* **Light Sensor**: Entering a password to a server using SSL and a light sensor.
* **Temperature Network**: A network of Intel Edisons monitoring temperature data and transmitting that to a central server. Each individual Edison also displays some data over an LCD screen
* **Weather Station**: One Intel Edison sending weather information to another one, communicating over SSL

## Built With

* [Mraa](https://iotdk.intel.com/docs/master/mraa/)
* [UPM](https://github.com/intel-iot-devkit/upm)
* [OpenSSL](https://www.openssl.org)

## Authors

* **Ryan Goggins**
* **Jeffrey Shen**
* **Peize He**

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

