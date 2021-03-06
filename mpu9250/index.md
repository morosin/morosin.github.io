---
title: MPU-9250 Tutorial
---
<script type="text/javascript" async
    src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML">
</script>

## Before we Start
_Take a look at these other, harder-to-read documentations:_
* [Datasheet](https://www.invensense.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf)
* [Register Map](https://stanford.edu/class/ee267/misc/MPU-9255-Register-Map.pdf)
### Topics you should be familiar with:
* SPI
* Basic numeric integration (not necessary in practice, just in theory)

## Introduction
### What is it?
The MPU9250 is a 9-axis accelerometer. This means that it has 3 axes of movement for:
* an accelerometer
* a gyroscope
* a magnetometer (compass)

The gyroscope axes are similar to this, because I had trouble picturing them.

<img src="https://raw.githubusercontent.com/SeeedDocument/Grove-3-Axis_Digital_Gyro/master/img/Gyro_Reference_1.jpg" style="width: 200px;"/>

### Modes of Operation
MPU9250 usage is either __poll-driven__ or __interrupt-driven__. I'll be going
over poll-driven I/O first because it makes it easier to make a simple program.
For both models, data may be retrieved over either I<sub>2</sub>C or SPI. I
haven't experimented with I<sub>2</sub>C operation, so for now I'll stick to
SPI. 

### Recommendations
For initial testing, it may be easier to use a UART transmitting function to send
readings to a more friendly display.

## Usage
### General
#### Circut
Setting up the MPU9250 for SPI communication can be a bit tricky because the pinout
is labeled for I<sub>2</sub>C communication. Here is a translation of the pinout
labels:

* `NCS` stands for "negative chip select," which is equivalent to the SPI `CSN` pin.
* `SCL` - "serial clock," replaces `SCK`.
* `AD0` - serial output, replaces `MISO`.
* `SDA` - serial input, replaces `MOSI`.

Aside from `VCC` and `GND`, you shouldn't need to use any more connections. 

#### Register IO
Registers are addressed by a value from 0 to 128, as described in the register
map linked above. The most significant bit of the transacted byte determines 
read or write mode, and the least significant 7 bits determine the register 
address. For example, to read the high bit of the gyroscope X register:
```c
select(); // Select the chip by dropping to LOW the NCS pin
transfer(GYRO_XOUT_H | 0x80); // Transfer the register address with the MSB set high
uint8_t val = transfer(0x00); // Retrieve the value with a second transfer.
deselect(); // Pull the NCS pin HIGH
```
As you can see, the read operation is completed in two SPI transfers. While it is
not necessary that the second transfer send the bit `0x00`, behavior is undefined
if it is anything else. __Remember that the two transactions must be contained by
ONE select/deselect operation and not two separate ones.__

An example write operation should be fairly unsurprising:
```c
select();
transfer(GYRO_CONFIG & 0x7F);
transfer(GYRO_CONFIG_1000_DPS);
deselect();
```
This example would set the gyroscope config to a 1000 DPS resolution? What does this
mean? I'll explain later.

Confused? `0x7F` is the same as `0b01111111` and `0x80` is the same as `0b1000000`,
so we apply each as a mask to control the most significant bit.

Most of the sensor values you'll be wanting to read are going to be `int16_t`s - 
signed 16-bit integers. Because SPI only allows for the transmission of one byte at
a time, the MPU9250 exposes two 8-bit registers for each of these values. Below is a 
pre-prepared header defining the register values you'll find useful:

```c
#ifndef ACCEL_MPU9250REGS_H
#define ACCEL_MPU9250REGS_H

// Configuration
#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02
#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG_A 0x1C
#define ACCEL_CONFIG_B 0x1D

// Gyro config resolution settings.
#define GYRO_CONFIG_250_DPS 0
#define GYRO_CONFIG_500_DPS 0b00001000
#define GYRO_CONFIG_1000_DPS 0b00010000
#define GYRO_CONFIG_2000_DPS 0b00011000

// Sensor registers
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

// Allows setting thresholds
#define XA_OFFSET_H 0x77
#define XA_OFFSET_L 0x78
#define YA_OFFSET_H 0x7A
#define YA_OFFSET_L 0x7B
#define ZA_OFFSET_H 0x7D
#define ZA_OFFSET_L 0x7E

#define PWR_MGMT_1 0x6B
#define PWM_MGMT_2 0x6C

// Identifies the device - mostly for I2C
#define WHOAMI 0x75

#define CONTROL_NOP 0x00 // Transmitted as the second byte when reading data.

#endif //ACCEL_MPU9250REGS_H
```

You'll notice that each of the sensor registers have a `_H` or an `_L` suffix. This
denotes either the high or the low byte of the 16-bit actual value respectively.
In order to compose the two registers into one, readable 16-bit value, we would:
```c
int16_t high_byte, low_byte;
select();
transfer(ACCEL_XOUT_H | 0x80);
high_byte = transfer(CONTROL_NOP);
deselect(); // You probably don't have to do this (question mark)?
select();
transfer(ACCEL_XOUT_L | 0x80);
low_byte = transfer(CONTROL_NOP);
deselect();
int16_t x_accel = (high_byte << 8) | low_byte;
```

### Gyroscope
The values recorded by the gyroscope are units of _rotational velocity_. The actual
units, however, are determined by the resolution set in the `GYRO_CONFIG` bit. For
any given DPS resolution `r`, the conversion scale is given by
$$\frac{r}{32768.0}$$. Remember that this number will be less than one, so you must 
either operate on scaled numbers or use floating-point arithmetic. It is Barry's 
commandment that you don't use floating-point arithmetic, but I offer a solution:
if the device receiving the readings is a computer rather than an 8-bit
microcontroller, move all the floating point arithmetic over there and only use the
AVR for sending. Anyways, the actual rotational velocity, in _degrees per second_, is
equal to the product of the conversion scale and the 16-bit value read from the
sensor. For example, to find the actual X rotational velocity at 250 DPS given the
sensor value of `gyro_x`, we would use:
```c
float velocity = gyro_x * (250.0f / 32768.0f);
```
Now, you're probably wondering what you can do with this velocity measurement. The
answer is, "not much."

<img src="https://raw.githubusercontent.com/morosin/morosin.github.io/master/images/nord2.jpg" width="100"/>

Yeah, I get that look a lot. For most applications, you'll probably want to be 
tracking an angle, not a rotational velocity. Luckily for you, that's where the
calculus comes in.

<img src="https://raw.githubusercontent.com/morosin/morosin.github.io/master/images/nord2.jpg" width="300"/>

Sorry I said anything.

From basic physics we know that the position of an object $$p$$ is given with respect
to time $$t$$ by the function $$p(t) = \int \! v(t) \mathrm{d}t$$ where $$v(t)$$ is
the object's velocity. Given the object's rotational velocity $$\frac{\mathrm{d}\theta}{\mathrm{d}t}$$,
we can determine the object's angular displacement from $$t_0$$ to $$t_1$$ with 
$$\Delta \theta = \int_{t_0}^{t_1} \! \frac{\mathrm{d}\theta}{\mathrm{d}t} \mathrm{d}t$$. Because
our sampling interval is relatively constant, we can use a numeric method to approximate
the angular displacement:
$$\Delta \theta \approx \frac{\mathrm{d}\theta}{\mathrm{d}t} \Delta t$$
Therefore, the object's angle $$\theta$$ is approximately equal to the sum of __every__
angular displacement, given by the product of each rotational velocity sampled and the
interval between each sample.

A few things to watch out for:

* Dropping a single velocity sample will make your angular measurement unusable. 

### Error Correction
When using any sort of sensor like this, especially if it's cheaper (ordered on eBay, 
from China), you'll have to do some things to correct for noise in readings. When
working with a gyroscope, this noise is called 'drift' and can be measured in the 
angular displacement reported over a unit time where the sensor was motionless (i.e.
$$\frac{1}{t_1 - t_0} \Delta \theta$$). You'll most likely need several approaches to
effectively compensate for sensor noise. These sorts of filters should be applied to
the raw data read from the sensor to minimize buffering of data within the microcontroller's
limited memory.

#### Determining Drift/Accuracy
Before you use any sensor for anything, you need to know exactly how accurate the sensor
is. Luckily, testing accuracy is relatively easy. 
- Find a way to report values from the sensor. This is most easily achieved via a UART
connection. Be cautious, however, that the time spent on a UART transaction doesn't result
in a significant backlog of actual sensor readings, because this will influence your
calculations. 
- Instruct your AVR to report the value of the sensor at a fixed interval and with each
transmission send some sort of value based on time. It's okay to use the 16-bit timer
for this, but when reviewing your data you have to be able to account for timer overflows.
- Set the sensor down in a stationary environment.
- Record the sensor's values over a significant amount of time (10 minutes or more).
- Relate the time elapsed from whichever value you transmitted to seconds.
- Determine the total, absolute positional change based across that time period.

#### Highpass Filter
_Highpass_ and _lowpass_ filters are common, simple filters with conveniently self-explanatory
names. A highpass filter of $$h$$ will only allow sensor values higher than $$h$$ to pass, with
a lowpass filter operating on a similar concept. Because all of the sensors on the MPU9250
operate using signed values, it is important that the highpass filter affects values for which
their absolute value is less than the threshold. For example, a highpass filter of 45 could be
applied with:
```c
#define highpass(threshold, val) (abs(val) > (threshold) ? val : 0)
```
Remember that you should zero values that don't pass the filter, not remove them from calculations
altogether. 

_How do I figure out what value to use as my threshold?_ you ask. Well, for example, the MPU9250
gyroscope operates at a relatively consistent level of background noise - nonzero values are
reported when the gyro is stationary. Unfortunately, this value not only varies based on 
circumstance but more directly by the resolution you choose to operate the sensor at. For this,
you can determine the background noise threshold programmatically:
```c
int16_t get_background_threshold() {
    int16_t avg = 0; // Store a rolling average.
    for (uint8_t i = 0; i < SAMPLES; ++i) { // Count up to a specific number of samples.
        avg = (avg * (i - 1) + read_gyro(GYRO_XOUT_H, GYRO_XOUT_L)) / i; // Add a new sample to the average.
    }
    return avg; // return the average value.
}
```

If a similar routine to this is ran at startup, we can obtain a general margin for error. Of 
course, you have to be very careful - a value too high will prevent the sensor from reading
very slow movements. 

#### Rolling Average
_How do I implement something like this?_ you ask? Luckily for you, you just did. In finding the
average background noise for a sensor across a predetermined number of samples, you compensated
for the variation between samples. Essentially, up to a reasonable number, you update an average
of the past $$n$$ samples and reset it when that reasonable number is reached. The most important
concern is that this will affect the resolution of your sensor, but not the sampling interval. 
Because an average calculation is made for each reading from the sensor, the average value at any
given time can be used in place of the actual reading.
