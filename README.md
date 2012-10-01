# OVERVIEW

Configuration file is located at ```/data/NSC/nsc_config.json```. The overall format is an object with arrays-of-objects as properties. Each of these inner-most objects represents a single event or data value to provide or accept.

```json
{
  "outputs":
    [
        {
          "id":"player_x", 
          "consumers":["COM3:115200N81"]
        },
        {
          "id":"player_y", 
          "consumers":["COM3:115200N81"]
        }
    ]
  "inputs":
    [
    ]
}
```



# OUTPUTS

Response format for data outputs is typically:

```json
[
  "value":{}
  ""
]
```

## CORE

These properties apply to and are in common with all value definitions. 

### Properties

* id -- Example: ```player_x``` -- This is the key for this event definition.
* consumers -- ```["COM3:115200N81", "COM1:9600N81"]```     
* only_on_change -- ```true``` -- Send continuously or only when it changes from last-sent value. 
* reset_counter_on_restart -- ```true``` -- reset this counter when the level restarts.
* reset_counter_on_teleport -- ```true``` -- reset this counter when the player teleports within the current map.
* interval -- ```0``` -- send as fast as possible (on every frame). ```(any n > 0)``` -- send data only on every ```n```th frame. interval has no effect when only_on_change = ```true```.



## player_x

Player's x-coordinate in the VR world.

### Properties

* id -- ```player_x```
* delta -- ```true``` -- Optional. On ```true``` the player_x value will express only the difference since the last transmission. On ```false``` the absolute value of the current player position will be sent.



## player_y

Player's y-coordinate in the VR world.

### Properties

* id -- ```player_y```
* delta -- ```true``` -- Optional. On ```true``` the player_x value will express only the difference since the last transmission. On ```false``` the absolute value of the current player position will be sent.



## player_left_click

The left mouse button has been clicked! Sends both press (value = 1) and release (value = 0) events or button status.

### Properties

* id -- ```player_left_click```

       
## player_right_click

The right mouse button has been clicked! Sends both press (value = 1) and release (value = 0) events or button status.

### Properties

* id -- ```player_left_click```

## level_restart

The level has been loaded. Value = 1

### Properties

* id -- ```level_restart```

## teleport

The mouse has teleported. Value = 1

### Properties

* id -- ```teleport```

