Input Control and Robot Command
===============================

**Table of Contents**
1. [Overview](#overview)
2. [Command mapping and training](#command-mapping-and-training)

Overview
--------
The Robot is controlled by a specific set of commands. Each command will make
the robot perform that action, like `forward`, `back`, `left`, etc.

Since the Robot Control Software allows for multiple types of hardware
controllers (serial input, IR remote, Bluetooth remote, etc.), the commands are
abstracted from the actual input controllers.

This means that each input controller has it's own _task_ which is responsible
for getting input from that hardware controller. Since the input method for each
controller may be different, the controller input is then decoded to a specific
robot command.

If a valid robot command was found for the input, that Robot command is placed
in the `commandStore` which will later be available to any component that
performs actions based on new commands.


Command mapping and training
----------------------------
Every input controller will have a `commandMap` which maps the input data
specific to that hardware controller to a Robot Control Command.

These maps are stored in EEPROM and retrieved on boot.

The control/command interfaces also has built in training capabilities to allow
each input controller to be trained for each Robot Control Command. To use this
the following is needed:

* Each input controller has to override some methods of the `InputHandler` class
  to allow prompting for input to train a specific command, and also to read the
  command from the input controller and assign it to the Robot Controll command
  in the command map.
* For now the Serial console is needed for prompting and feedback
* One Robot Control Command is called `Learn`, which when issued, will enter the
  learn mode. In order to make this command available, the input method mapping
  for this hardware controller should be hard-coded in the `commandMaps`
  structure for that controller in the `commands.cpp` source file.
* In order to ensure command integrity for the EEPROM stored commands, an
  EEPROM signature is used with the stored maps. This is a user created
  signature, which is just and arbitrary 16-bit integer value, and gets prepended
  to the EEPROM stored structure. This number should be modified every time the
  EEPROM stored structure changes (a new input method is added, command ordering
  is changed, commands are added or removed, etc.). When reading the stored
  command maps from EEPROM, the stored signature is compared to the hard-coded
  signature. If these differ, the structure has changed and the stored commands
  are invalid, and retraining is required.


