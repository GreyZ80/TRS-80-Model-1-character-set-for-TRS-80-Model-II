# TRS-80 Model 1 character set for TRS-80 Model II
A new character generator that brings the TRS-80 Model 1 character set, including the 2x3 graphics to the Big Tandy machines. No hardware modification, just a 2732 (4Kx8) EPROM and a wire with clip. Reverse text is still possible. The character matrix of the Model 1 is 6x9 pixels. For the Model II the matrix is 6x10. This means that the Model II has true descenders. It als means that the lower row of block of the Tandy graphics are 4 pixels in height, instead of 3.

## Compare Model 1 and Model II screens


| Property | Model I | Model II |
| ---- | --- | --- |
| Screen width in characters | 64 | 80 |
| Screen width in pixels | 384 | 640 |
| Screen height in characters | 16 | 24 |
| Screen height in pixels | 128 | 240 |
| Character width | 6 pixels | 8 pixels |
| Character height | 9 pixels | 10 pixels |



## Versions
There is currently (2025) 1 version of the character generator EPROM.\
This version has the Tandy graphics in the same location as the Model 1. This means that the reverse characters in the Model II are moved.


> [!IMPORTANT] 
> Use an EPROM that is fast enough for the pixel generation. This means that the **Access Time** should be less or equal to **350ns**. Using slower EPROMS (e.g. 450ns access time) will result in anoying flickering pixels with some characters. 

## Bill of materials
The following hardware is needed for the new character generator
- 2732 type EPROM. Parallel access. Serial EPROMs can not be used. Check the datasheet for the required access time of maximal 350ns.
- Piece of wire, 4 inches in length.
- Little test clip connected to the wire. Alternatively you can solder the wire to the IC ?? pin ?

## Preparation
Use an EPROM Programmer to program the 2732 with the characterset of your choice. If you have no access to an EPROM Progammer contact me for a ready to use EPROM.
Verify the content.
Bend pin 20 horizontal, as this pin is to be connected to the wire.
Solder the wire to the pin.
Solder the test clip to the other end of the wire.

## Installation
In order to install the new character generator, you will have to open the Model II. The character generator is located on the video board. All big Tandy computers have a dedicated video board. The character generator is placed in a 24 pin socket, which labelled U2?.\
- Remove the cover of the Model II by removing the 2 bolts and gently lifting it up.
- Loosen and remove the horizontal bracket that secures the PCB board. Prevent the bolts and/or nuts to drop inside the computer.
- Locate the video board. It is the board with 2 small connector at the edge. One is for the video signal going to the monitor. The other is the connector with the cable going to the keyboard.
- Remove the 2 connectors (6 pin and 5 pin) from the video board edge. They might be secured with some white glue.
- Gentle remove the video board and place it on the table with the chips op top and the card egde connector facing aways from you.
- Also remove the board to the right of the video board position. Remove connectors when needed. This is done to make it easier to place the video board (with test clip) back into the computer.
- Use a small screwdrive to pry the 24 pin chip out of it's socket. Gently work it up from the two short sides.
- Place the old character generator in an ESD bag or a piece of aluminum foil for storage. Then put it in a labelled enveloppe.
- Put the new character generator in the 24 pin socket. Make sure the orientation is correct. The notch must have the same orientation as the notch of the 40 pin chip. Pointing to the right.
- Locate chip U?? and connect the test clip to pin ?. This can be tricky, but is possible. 
- Double check chip orientation and test clip connection.
- Place the video board back in the computer. Make sure the test clip remains in place.
- Place the board that was loacted to the right of the video board back into the computer. Again make sure the test clip remains in place.
- Double check correct seating of both boards in their 80 pin sockets.\

## Quick test
Now correct operation of the character generator can be checked. Remove any floppy from the drive. Disconnect Hard Drive when used.
Power on the machine. It should ask for the floppy disk. The white background will have changed to ???????????????????????????????????????????\
Power down the machine
When there were problems, you will have to check your work.

## Finishing up
- Mount the bracket
- Place the cover

The machine can now be used again.\
@@@ Test using basic\
@@@ Test using assembler

