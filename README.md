# ESP_Homie_WS2812FX [![Build Status](https://travis-ci.org/euphi/ESP_Homie_WS2812FX.svg?branch=master)](https://travis-ci.org/euphi/ESP_Homie_WS2812FX) [![platformio](https://img.shields.io/badge/available--at-platformio-orange)](https://platformio.org/lib/show/1857/ESP_Homie_WS2812FX/)
HomieNode of [Homie-ESP8266](https://github.com/homieiot/homie-esp8266) for [WS2812FX](https://github.com/kitesurfer1404/WS2812FX) Library


## openHAB integration

Example configuration for a device with ID `labor`:

### Items
```
Group WS2812

Dimmer lab_ws_bright  "Brightness"   	(WS2812) {mqtt=">[local:homie/labor/WS2812FX/brightness/set:state:*:default]"}
Dimmer lab_ws_speed   "Speed" 		(WS2812) {mqtt=">[local:homie/labor/WS2812FX/speed/set:state:*:default]"}
String lab_ws_mode    "Mode [%s]"     	(WS2812) {mqtt=">[local:homie/labor/WS2812FX/mode/set:COMMAND:*:default], <[local:homie/labor/WS2812FX/mode:state:default]"}
```

### Sitemap
```
sitemap lab label="Test Sitemap" {

Frame label="WS2812" {
	Slider item=lab_ws_bright
	Slider item=lab_ws_speed
	Switch item=lab_ws_mode mappings=[next="+", prev="-"]
}


}
```

### Better scaling for speed (percent instead of 8bit)

Change item `lab_ws_speed` to:
```
Dimmer lab_ws_speed   "Geschwindigkeit" (Labor) {mqtt=">[local:homie/labor/WS2812FX/speed/set:state:*:JS(scale_percent_to_byte8.js)]"}
```

and create file `scale_percent_to_byte8.js` in `transform/` subdirectory.
```javascript
(function(i) {   
    return i*255/100;
})(input)
```
