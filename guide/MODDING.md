# WORK IN PROGRESS

# Modding Guide
This guide will tell you the most basic things you need to know about modding, how to add characters, stages, etc. (Stop porting mods, just play them on the real hardware, grr...)

## Add a new Stage or Character to the script
- `makefile`
Open the makefile and locate the section where script files are included. Add the path of the new script file that you want to add to the list.

- `scenes/stage/stage.c`
Open the stage.c file located in the scenes/stage directory. Look for the section where other header files are included. Add an #include statement for the .h file associated with the new stage or character you want to add.

## Characters
In this section, we'll discuss how to create and modify characters.
To create a new character, start by making a copy of an existing character file (for example, Dad, Boyfriend, or GF). Only copy the .c and .h files, not the pre-compiled files! Place the copied files in the src/characters folder.
Next, open the two files of the copied character (the .c and .h files) and replace the old character name with the name of the new character. You can use the Ctrl+F shortcut to find and replace the name, but be careful with uppercase and lowercase letters.

# Offsetting

If you already have an existing character, you can start to offset the character's frames. There are easier solutions and more difficult ones, such as:
- Igor's tool (Compatible only with characters created using Psych Engine)
This tool offers a convenient solution designed specifically for Psych Engine characters, allowing for easy adjustment of frame offsets.

- My tool (Not recommended at the moment)
I have developed a tool for this purpose, but I don't recommend using it just yet. It may still have some issues or limitations that need to be resolved.

- Manual offsetting
This method involves manually adjusting the frame offsets, which can be a cumbersome process. It requires modifying the character's code directly to achieve the desired effect. This option can be more challenging and time-consuming compared to using specialized tools.

# 

# Icon

## Stages
