# Unreal Map Check Automation
Map Check automation tooling for Unreal Engine development.

## Map Check overview

Map Check is a error checking tool for your levels while developing in the Unreal Editor.
You can find details about how this works on
(Unreal's documentation)[https://docs.unrealengine.com/latest/en-US/map-check-errors-in-unreal-engine/].

The normal way to run Map Check is to navigate to Build -> Map Check while you have
a level open in the editor.

![Map Check in Editor](Images/MapCheckInEditor.jpg)

Among some other generic things, this iterates over all
the actors you have in a scene and calls the `CheckForErrors` method, which traditionally
populates information into a MapCheck `FMessageLog`. 

## Running the Commandlet

Running this process via a commandlet allows for headless operation (ie without the Unreal
Editor window needing to open). This pairs nicely with your build machines, and the
commandlet can even produce a JSON file of warnings and errors for you to parse.

![Arguments](Images/arguments.jpg)

To run a commandlet through the editor, pass the argument `-run=MapCheck`. This is based off
of the name of the commandlet, so if you do locally make changes to the name, you will need
to update any script that requires this argument. As seen in the above image, you can
append additional arguments to further control output of the commandlet. You may prefer to
parse the logged output of the execution.

By default, the parameter for `jsonoutput` is appended to the Project's Saved Data are (this
is often excluded from version control considerations).

## Dependencies

A MapCheckAutomation.Build.cs file is included in this repository in case you want to use
this as a module by itself.
