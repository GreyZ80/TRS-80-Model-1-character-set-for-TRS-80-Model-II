# TRS-80 Model 1 character set for TRS-80 Model II
A new character generator that brings the TRS-80 Model 1 character set, including the 2x3 graphics to the Big Tandy machines. No hardware modification, just a 2732 (4Kx8) EPROM and a wire with clip. Reverse text is still possible. 

## Versions
The are 2 versions of the character EPROM:
- Version A has the reverse characters in the same location as the original character generator.
- Version B has the Tandy graphics in the same location as the Model 1.
Unfortunately A and B cannot be combined :( . See screenshots.

> [!IMPORTANT] 
> Use an EPROM that is fast enough for the pixel generation. This means that the Access Time should be less or equal to 350ns. Using slower EPROMS (e.g. 450ns access time) will result in anoying flickering pixels with some characters. 

## Bill of materials
The follwoing hardware is needed for the new character generator
- 2732 type EPROM. Parallel access. Serial EPROMs can not be used. Check the datasheet for the required access time of maximal 350ns.
- Piece of wire, 4 inches in length.
- Little test clip connected to the wire. Alternatively you can solder the wire to the IC ?? pin ?

## Preparation
Use an EPROM Programmer to program the 2732 with the characterset of your choice. If you have no access to an EPROM Progammer contact me for a ready to use EPROM.

##Installation

In order to install the new character generator, you will have to open the Model II. The character generator is located on the video board. All big Tandy computers have a dedicated video board. The character generator is placed in a 24 pin socket, wihich labelled U2?
- 
