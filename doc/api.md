# Documentation for Class I2C\_TPA2016

- [Public Types](#public-types)
- [Public Functions](#public-functions)
- [Public Functions Documentation](#public-functions-documentation)
- [Public Types Documentation](#public-types-documentation)

## Public Types

| Type | Name |
| ---: | :--- |
| enum  | [**TPA2016\_COMPRESSION\_RATIO**](#enum-tpa2016-compression-ratio)  <br> |
| enum  | [**TPA2016\_LIMITER\_NOISEGATE**](#enum-tpa2016-limiter-noisegate)  <br> |


## Public Functions

| Type | Name |
| ---: | :--- |
|   | [**I2C\_TPA2016**](#function-i2c-tpa2016) (uint8\_t bus, uint8\_t address=TPA2016\_I2CADDR) <br>_Opens a I2C connection and configure device as a slave._  |
|  float | [**attackTime**](#function-attacktime) () <br> |
|  TPA2016\_COMPRESSION\_RATIO | [**compressionRatio**](#function-compressionratio) () <br> |
|  void | [**disableHoldControl**](#function-disableholdcontrol) () <br>_Set hold time to 0, effectively disabling it._  |
|  void | [**enableChannels**](#function-enablechannels) (bool right, bool left) <br> |
|  void | [**enableLimiter**](#function-enablelimiter) (bool limiter) <br>_Control output limiter activation._  |
|  void | [**enableNoiseGate**](#function-enablenoisegate) (bool noiseGate) <br>_Control noise gate function._  |
|  int8\_t | [**gain**](#function-gain) () <br> |
|  bool | [**holdControlEnabled**](#function-holdcontrolenabled) () <br> |
|  float | [**holdTime**](#function-holdtime) () <br> |
|  bool | [**leftEnabled**](#function-leftenabled) () <br> |
|  bool | [**leftShorted**](#function-leftshorted) () <br>_Returns true if a short circuit occurred on left speaker._  |
|  bool | [**limiterEnabled**](#function-limiterenabled) () <br> |
|  float | [**limiterLevel**](#function-limiterlevel) () <br> |
|  uint8\_t | [**maxGain**](#function-maxgain) () <br> |
|  bool | [**noiseGateEnabled**](#function-noisegateenabled) () <br> |
|  TPA2016\_LIMITER\_NOISEGATE | [**noiseGateThreshold**](#function-noisegatethreshold) () <br> |
|  bool | [**ready**](#function-ready) () <br> |
|  float | [**releaseTime**](#function-releasetime) () <br> |
|  void | [**resetShort**](#function-resetshort) (bool right, bool left) <br> |
|  bool | [**rightEnabled**](#function-rightenabled) () <br> |
|  bool | [**rightShorted**](#function-rightshorted) () <br>_Returns true if a short circuit occurred on right speaker._  |
|  void | [**setAttackTime**](#function-setattacktime) (float attack) <br>_Changes the minimum time between gain decreases._  |
|  void | [**setCompressionRatio**](#function-setcompressionratio) (TPA2016\_COMPRESSION\_RATIO ratio) <br> |
|  void | [**setGain**](#function-setgain) (int8\_t gain) <br>_Choose fixed gain._  |
|  void | [**setHoldTime**](#function-setholdtime) (float hold) <br>_Changes the minimum time between a gain decrease (attack) and a gain increase (release)_  |
|  void | [**setLimiterLevel**](#function-setlimiterlevel) (float limit) <br> |
|  void | [**setMaxGain**](#function-setmaxgain) (uint8\_t maxGain) <br>_Set maximum gain the amplifier can achieve._  |
|  void | [**setNoiseGateThreshold**](#function-setnoisegatethreshold) (TPA2016\_LIMITER\_NOISEGATE threshold) <br>_Change activation threshold of Noise Gate function Cannot be called if compression ratio is 1:1._  |
|  void | [**setReleaseTime**](#function-setreleasetime) (float release) <br>_Changes the minimum time between gain increases._  |
|  void | [**softwareShutdown**](#function-softwareshutdown) (bool shutdown) <br>_Control bias, oscillator and control functions._  |
|  bool | [**tooHot**](#function-toohot) () <br>_Returns true if a hardware shutdown due to overheat happened._  |
|   | [**~I2C\_TPA2016**](#function-i2c-tpa2016) () <br> |

## Public Functions Documentation

### <a href="#function-i2c-tpa2016" id="function-i2c-tpa2016">function I2C\_TPA2016 </a>

```cpp
I2C_TPA2016::I2C_TPA2016 (
    uint8_t bus,
    uint8_t address=TPA2016_I2CADDR
)
```


**Parameters:**


* **bus** But number (I2C adapter)
* **address** Address of the slave device



**Exception:**


* **std::runtime\_error** If any error when configuring device





### <a href="#function-attacktime" id="function-attacktime">function attackTime </a>


```cpp
float I2C_TPA2016::attackTime ()
```



### <a href="#function-compressionratio" id="function-compressionratio">function compressionRatio </a>


```cpp
TPA2016_COMPRESSION_RATIO I2C_TPA2016::compressionRatio ()
```



### <a href="#function-disableholdcontrol" id="function-disableholdcontrol">function disableHoldControl </a>


```cpp
void I2C_TPA2016::disableHoldControl ()
```



### <a href="#function-enablechannels" id="function-enablechannels">function enableChannels </a>


```cpp
void I2C_TPA2016::enableChannels (
    bool right,
    bool left
)
```



### <a href="#function-enablelimiter" id="function-enablelimiter">function enableLimiter </a>


```cpp
void I2C_TPA2016::enableLimiter (
    bool limiter
)
```




**Parameters:**


* **limiter** Can only be false when compression ratio is 1:1



**Exception:**


* **std::logic\_error** if limiter == false and compression ratio is not 1:1





### <a href="#function-enablenoisegate" id="function-enablenoisegate">function enableNoiseGate </a>


```cpp
void I2C_TPA2016::enableNoiseGate (
    bool noiseGate
)
```




**Parameters:**


* **noiseGate** can only be true if compression ratio is not 1:1



**Exception:**


* **std::logic\_error** If noiseGate is true and compression ratio is 1:1





### <a href="#function-gain" id="function-gain">function gain </a>


```cpp
int8_t I2C_TPA2016::gain ()
```



### <a href="#function-holdcontrolenabled" id="function-holdcontrolenabled">function holdControlEnabled </a>


```cpp
bool I2C_TPA2016::holdControlEnabled ()
```



### <a href="#function-holdtime" id="function-holdtime">function holdTime </a>


```cpp
float I2C_TPA2016::holdTime ()
```



### <a href="#function-leftenabled" id="function-leftenabled">function leftEnabled </a>


```cpp
bool I2C_TPA2016::leftEnabled ()
```



### <a href="#function-leftshorted" id="function-leftshorted">function leftShorted </a>


```cpp
bool I2C_TPA2016::leftShorted ()
```



### <a href="#function-limiterenabled" id="function-limiterenabled">function limiterEnabled </a>


```cpp
bool I2C_TPA2016::limiterEnabled ()
```



### <a href="#function-limiterlevel" id="function-limiterlevel">function limiterLevel </a>


```cpp
float I2C_TPA2016::limiterLevel ()
```



### <a href="#function-maxgain" id="function-maxgain">function maxGain </a>


```cpp
uint8_t I2C_TPA2016::maxGain ()
```



### <a href="#function-noisegateenabled" id="function-noisegateenabled">function noiseGateEnabled </a>


```cpp
bool I2C_TPA2016::noiseGateEnabled ()
```



### <a href="#function-noisegatethreshold" id="function-noisegatethreshold">function noiseGateThreshold </a>


```cpp
TPA2016_LIMITER_NOISEGATE I2C_TPA2016::noiseGateThreshold ()
```



### <a href="#function-ready" id="function-ready">function ready </a>


```cpp
bool I2C_TPA2016::ready ()
```



### <a href="#function-releasetime" id="function-releasetime">function releaseTime </a>


```cpp
float I2C_TPA2016::releaseTime ()
```



### <a href="#function-resetshort" id="function-resetshort">function resetShort </a>


```cpp
void I2C_TPA2016::resetShort (
    bool right,
    bool left
)
```



### <a href="#function-rightenabled" id="function-rightenabled">function rightEnabled </a>


```cpp
bool I2C_TPA2016::rightEnabled ()
```



### <a href="#function-rightshorted" id="function-rightshorted">function rightShorted </a>


```cpp
bool I2C_TPA2016::rightShorted ()
```



### <a href="#function-setattacktime" id="function-setattacktime">function setAttackTime </a>


```cpp
void I2C_TPA2016::setAttackTime (
    float attack
)
```




**Parameters:**


* **attack** Attack time in ms/6dB (1.28 &lt;= x &lt;= 80.66)



**Exception:**


* **std::out\_of\_range**





### <a href="#function-setcompressionratio" id="function-setcompressionratio">function setCompressionRatio </a>


```cpp
void I2C_TPA2016::setCompressionRatio (
    TPA2016_COMPRESSION_RATIO ratio
)
```



### <a href="#function-setgain" id="function-setgain">function setGain </a>


```cpp
void I2C_TPA2016::setGain (
    int8_t gain
)
```




**Parameters:**


* **gain** Gain in dB, maximum 30 dB. Minimum -28dB if compression is enabled, 0 otherwise.



**Exception:**


* **std::out\_of\_range**





### <a href="#function-setholdtime" id="function-setholdtime">function setHoldTime </a>


```cpp
void I2C_TPA2016::setHoldTime (
    float hold
)
```




**Parameters:**


* **hold** Hold time in second (per step) (0.0137 &lt;= x &lt;= 0.8631)



**Exception:**


* **std::out\_of\_range**





### <a href="#function-setlimiterlevel" id="function-setlimiterlevel">function setLimiterLevel </a>


```cpp
void I2C_TPA2016::setLimiterLevel (
    float limit
)
```



### <a href="#function-setmaxgain" id="function-setmaxgain">function setMaxGain </a>


```cpp
void I2C_TPA2016::setMaxGain (
    uint8_t maxGain
)
```




**Parameters:**


* **maxGain** Maximum gain in dB (0 &lt;= x &lt;= 30)



**Exception:**


* **std::out\_of\_range**





### <a href="#function-setnoisegatethreshold" id="function-setnoisegatethreshold">function setNoiseGateThreshold </a>


```cpp
void I2C_TPA2016::setNoiseGateThreshold (
    TPA2016_LIMITER_NOISEGATE threshold
)
```




**Exception:**


* **std::logic\_error** if compression ratio is 1:1





### <a href="#function-setreleasetime" id="function-setreleasetime">function setReleaseTime </a>


```cpp
void I2C_TPA2016::setReleaseTime (
    float release
)
```




**Parameters:**


* **release** Release time in sec/6dB (0.1644 &lt;= x &lt;= 10.36)



**Exception:**


* **std::out\_of\_range**





### <a href="#function-softwareshutdown" id="function-softwareshutdown">function softwareShutdown </a>


```cpp
void I2C_TPA2016::softwareShutdown (
    bool shutdown
)
```



### <a href="#function-toohot" id="function-toohot">function tooHot </a>


```cpp
bool I2C_TPA2016::tooHot ()
```



### <a href="#function-i2c-tpa2016" id="function-i2c-tpa2016">function ~I2C\_TPA2016 </a>


```cpp
I2C_TPA2016::~I2C_TPA2016 ()
```
## Public Types Documentation


### <a href="#enum-tpa2016-compression-ratio" id="enum-tpa2016-compression-ratio">enum TPA2016\_COMPRESSION\_RATIO </a>


```cpp
enum TPA2016_COMPRESSION_RATIO {
    _1_1 = 0x00,
    _1_2 = 0x01,
    _1_4 = 0x02,
    _1_8 = 0x03
};
```



### <a href="#enum-tpa2016-limiter-noisegate" id="enum-tpa2016-limiter-noisegate">enum TPA2016\_LIMITER\_NOISEGATE </a>


```cpp
enum TPA2016_LIMITER_NOISEGATE {
    _1MV = 0x00,
    _4MV = 0x20,
    _10MV = 0x40,
    _20MV = 0x60
};
```
