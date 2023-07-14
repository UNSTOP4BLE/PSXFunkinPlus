Due to changes in the MSYS2 installation instructions, the old `compile.md` document will not work. Therefore, I do not recommend using the installation instructions on the website as they will not be effective. Instead, I have gathered the commands that need to be entered in the terminal. Please follow the updated installation guide below:

# Compiling PSXFunkin (MSYS2 ONLY)

## Installation Guide
- Start by downloading the installer for MSYS2 from their website: https://www.msys2.org/

### Installation advice
- Make sure your Windows version is 64-bit and is Windows 7 or newer.
- Choose a short and simple installation folder name using only ASCII characters. The folder should be on an NTFS volume and should not contain accents, spaces, symlinks, subst, network drives, or FAT.

### Run the MSYS2 installer and follow these steps:
Open a terminal or command prompt.
Update the package database and base packages by entering the following command: `pacman -Syu` (Note: If your setup file is not up-to-date, you may need to run the command twice.)
After the update, run "MSYS2 MSYS" from the Start menu.
To update the remaining base packages, run the following command: `pacman -Su`

Now that MSYS2 is installed and updated, you may want to install additional tools and the mingw-w64 GCC for compiling. Run the following command:
`pacman -S --needed base-devel mingw-w64-x86_64-toolchain`
(If the terminal prompts with "Enter a selection (default=all):", simply press Enter to continue.)

### Additional resources:
For more information, you can refer to the introduction page, which will guide you on using the Start menu items and installing the required packages.
If you encounter any issues or need further details, check the Detailed MSYS2 install guide for troubleshooting tips and keeping your MSYS2 installation up-to-date.

## Installing "g++-mipsel-none-elf-11.2.0"
Once you have set up MSYS2, you will need to copy the MIPS toolchain. Download it from this link: http://static.grumpycoder.net/pixel/mips/g++-mipsel-none-elf-11.2.0.zip.

Close MSYS2 if it is open. Extract the downloaded zip file and copy the following folders into `msys64/usr/local/`:
- `bin, include, lib, libexec, mipsel-none-elf`

IMPORTANT: Delete the following files in the bin folder, or your terminal may not work properly:
- `cat, cp, echo, make, mkdir, touch, rm, which`

Open "MSYS2 MinGW 64-bit" from the Start Menu. To install necessary libraries, run the following command and accept any prompts:
`pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-tinyxml2 mingw-w64-x86_64-ffmpeg`

These steps will ensure that you have the MIPS toolchain set up correctly in MSYS2 and install the required libraries for further development.

## Installing mkpsxiso
Download the latest Windows zip file from this GitHub page: https://github.com/Lameguy64/mkpsxiso.

Extract the downloaded zip file.
Copy the `bin` folder from the extracted contents.
Navigate to `msys64/usr/local/` directory.
Paste the copied `bin` folder into the `msys64/usr/local/` directory.

After completing these steps, the mkpsxiso installation will be finished, and you can start using it from the command line or terminal.

## Copying PsyQ files
First, go to the [mips](/mips/) folder of the repo, and create a new folder named `psyq`.

Then, download the converted PsyQ library from http://psx.arthus.net/sdk/Psy-Q/psyq-4_7-converted-light.zip. Just extract the contents of this into the new `psyq` folder.

## Compiling tools, converting assets, and compiling PSXFunkin
Ensure you are in the repository directory that contains all the necessary makefiles.
Run the following command: `make -f Makefile.assets -jX`

This command will perform the following tasks:
- Compile the tools found in the `tools/` directory.
- Convert the PNG files in `iso/` to TIM files that can be displayed by the PS1.
- Convert the character JSON files in `iso/` to CHR files containing mapping and art data.
- Convert the OGG files in `iso/music/` to XA files for PS1 audio (This step may take some time).
- Convert the JSON files in `iso/chart/` to CHT files playable by the game.
- Compile PSXFunkin and generate a `funkin.ps-exe` file in the same directory.
- Create the `.bin` and `.cue` files using the PS-EXE and assets located in the iso/ directory.
- Once the command completes successfully, you should have the compiled game and the resulting `.bin` and `.cue` files ready to use.

Note: Make sure you have the necessary PSX license file named `licensea.dat` in the same directory as `funkin.xml`. If you don't have the license file, remove the line `<license file="licensea.dat"/>` from `funkin.xml`, but be aware that this may result in potential issues on some emulators due to BIOS checks.
Tip: To speed up the `make` command, use `-jX` at the end, where X is your CPU core count multiplied by two. This maximizes CPU usage for faster execution.
