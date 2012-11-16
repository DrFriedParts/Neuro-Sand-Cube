# OVERVIEW

Configuration file is located at ```/data/NSC/nsc_config.json```. The overall format is an object with arrays-of-objects as properties. Each of these inner-most objects represents a single event or data value to provide or accept.

```json
{ 
	"aliases":
	{ 
          "vr_controller":"127.0.0.1",
          "my_pc":"10.16.1.45",
          "neurotrigger":"COM3:115200N81"
    },
	"outputs":
    [
        {
          "id":"player_x", 
          "consumers":["COM3:115200N81", 127.0.0.1, "neurotrigger"]
        },
        {
          "id":"player_y", 
          "consumers":["COM3:115200N81"]
        }
    ],
  "inputs":
    [
        { "id":"restart_map",
          "from":["my_pc","neurotrigger"]
        },
        { "id":"flush_water_reward",
          "from":["my_pc","vr_controller"],
          "route":"neurotrigger"
        }
    ]
}

```

The ```aliases``` record specifies named aliases for destinations to facilitate easy maintainence of the configuration file is hosts change. ```consumers``` of other ```id```'s can be listed by COM port, IP address, or Alias.

If a client is not listed in any ```consumers:``` property, then *all* configured outputs are sent to it. This is useful for connecting a console laptop for debugging monitoring.

Similarly, if a client is not listed in any ```from:``` field, then *all* inputs are accepted from it. Same reason.

If an input is sent by a client that is not one of the supported input commands (eg restart_map) it can still be routed to other clients if specified in the above configuration.

# OUTPUTS

Response format for data outputs is typically:

```json
[ 
{ "id": "player_x", "value": 737.891, "change_count": 5  }, 
{ "id": "timestamp", "value": "18:05:28.183"  }, 
{ "id": "frame", "value": 100444  }
]
```

## CORE

These properties apply to and are in common with all value definitions. 

### Properties

* id -- Example: ```player_x``` -- This is the key for this event definition.
* consumers -- ```["COM3:115200N81", "COM1:9600N81", "127.0.0.1", "10.16.1.4"]``` 
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
* delta -- ```true``` -- Optional. On ```true``` the player_y value will express only the difference since the last transmission. On ```false``` the absolute value of the current player position will be sent.



## player_left_click

The left mouse button has been clicked! Value = 1

### Properties

* id -- ```player_left_click```

       
## player_right_click

The right mouse button has been clicked! Value = 1

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

## player_angle

Player's angle around the VR world y-axis.

### Properties

* id -- ```player_angle```
* delta -- ```true``` -- Optional. On ```true``` the player_angle value will express only the difference since the last transmission. On ```false``` the absolute value of the current player angle will be sent.

## distance_traveled

Player's total distance traveled from the start of the level.

### Properties

* id -- ```distance_traveled```
* delta -- ```true``` -- Optional. On ```true``` the distance_traveled value will express only the difference since the last transmission. On ```false``` the absolute value of the current player angle will be sent.

## trial_start

Indicates that a trial has started.  At the moment, a trial map is determined to be any map that contains the word 'newtrain'.  This will be 
made more flexible in the near future.
The 'value' returned is the name of the map.

### Properties

* id -- ```trial_start```

## correct_trial

Indicates whether a trial has been performed correctly.  A trial is marked as correct if the player reaches ```level_trigger_1```.

### Properties

* id -- ```correct_trial```

## incorrect_trial

Indicates whether a trial has been performed incorrectly.  A trial is marked as incorrect if the player reaches ```level_trigger_2```.

### Properties

* id -- ```incorrect_trial```

## reward_issued

Indicates whether a reward has been issued. If routed to neurotrigger, this will cause the reward to be released.

### Properties

* id -- ```reward_issued```


# INPUTS

## Core

### Properties

* from -- List of clients to accept this command from. Note that if a client is not listed in *any* of the ```from``` attributes, then *all* commands are accepted. If the client is listed anywhere, then only the specified commands are accepted from it. This helps with 3rd party instruments that are verbose on their external communication channels.

* route -- List of destinations to route the command. If ```route``` is specified, then the incoming command is piped to the specified destinations. This is useful for controlling/reseting/sync'ing external systems to the VR environment in an simple orderly manner. Some commands take action. This happens in addition to routing (e.g. any command may be routed and routing doesn't effect the normal behavior of that command).

### Examples

Restarts the current map and let's a serial-attached device and a network-attached console (at ```10.16.1.7```) know that it occured. The network client must already have an open connection to this server to see the message. Messages are not delay-tolerant -- the client must be connected at the time of the event to receive.

```coffeescript
{
  id: "restart_map",
  route: ["COM3:115200N81","10.16.1.7"],
}
```

In this example, as before, the level is restarted, but now all other (assuming this is the total contents of the input section) output from ```10.16.1.7``` is ignored. Putting the same alias in both "from" and "route" results in a local echo.

```coffeescript
{
  id: "restart_map",
  from: ["10.16.1.7"],
  route: ["COM3:115200N81","10.16.1.7"],
}
```

### Usage

Using the prior configuration as an example, imagine the following sent from the serial device connected at ```COM3:115200N81```:

```coffeescript
{
  "id":"restart_map"
}
```

The NSC restarts its current level (performing any associated actions as specified in the config file, such as resetting level counters) and echoes the message back to ```COM3``` in addition to sending it to ```10.16.1.7```. The message looks the same as the original (except we remove whitespace and terminate with a new-line):

```coffeescript
{"id":"restart_map"}\n
```


## restart_map

Restarts the current map

### Properties

* id -- ```restart_map```

## issue_reward

Triggers the ```reward_issued``` event

### Properties

* id -- ```issue_reward```

## reset_counter

Restarts the occurance counter on the specified output

### Properties

* id -- ```reset_counter```
* target -- ```player_left_click```

## else

Any ```id``` that isn't specifically ennumerated above (e.g. given a specific action that effects the NSC server/environment directly) found in the config file is simply processed without taking a local action. It still routes and respects ```from``` rules. This is crazy useful in interfacing and configuring external instruments without having to modify the NSC source code to explicitly support them. 

### Properties

* id -- ```<anything else not listed above>```


# CUBE2 scripting extensions

The following extensions have been made to the CUBE2 scripting interface:

## Additional attributes

### invmousex

```invmousex``` inverts the mouse x-axis, as the CUBE2 attribute ```invmouse``` does for the y-axis.

## Additional commands

### issue_reward
The ```issue_reward``` script command triggers the Neuro-Sand-Cube ```reward_issued``` event.  

Some examples of how to use this in a trigger, when placed in a map configuration file:
* level_trigger_1 = [issue_reward]

The above will simply trigger the ```reward_issued``` event when the trigger is activated.

* level_trigger_1 = [issue_reward ; map newtrain_2LR]
The above will trigger the ```reward_issued``` event when the trigger is activated, and thereafter load the map specified by ```newtrain_2LR```.

TEMPORARY NOTE: at present, when ```issue_reward``` is triggered, instead of sending ```reward_issued``` to the serial port, it will send ```level_restart``` so that the reward system keeps functioning until the firmware is upgraded. You should however remove neurotrigger from the list of consumers for ```level_restart``` (it will still receive the ```level_restart``` caused by the ```issue_reward``` event) in order to avoid rewards occurring for map restarts. 