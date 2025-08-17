# TRS-80 Model 1 character set for TRS-80 Model II
A new character generator that brings the TRS-80 Model 1 character set, including the 2x3 graphics to the Big Tandy machines. No hardware modification, just a 2732 (4Kx8) EPROM and a wire with clip. Reverse text is still possible. 

## Compare Model 1 and Model II screens

In normal operation, the screens have the following properties:

| Property | Model I | Model II | units |
| ---- | --- | --- | ---- |
| Screen width  | 64 | 80 | characters |
|   | 384 | 640 | pixels |
| Screen height | 16 | 24 | lines |
|   | 128 | 240 | pixels |
| Character width | 6 | 8 pixels | pixels |
| Character height | 9 | 10 | pixels |

The difference in character size allows for true descenders. It also means that the lower row of blocks in the Tandy graphics have a height of 4 instead of 3 pixels.

## Versions
There is currently (2025) one version of the character generator EPROM.\
This version has the Tandy graphics in the same location as the Model 1. This means that the reverse characters in the Model II are moved.

@@@@@@@@@ screenshot Model 1 characters.  Use machine lange routine embedded in Basic
@@@@@@@@@ screenshot Model II characters

The following short Basic programs was used to create the dump:
```perl
100 CLS
110 FOR D = 1 to 40
120 READ D
130 PR$ = PR$ + chr$(D)
140 NEXT D
150 VP = VARPTR(PR$)
160 EXEC VP
200 GOTO 200
1000 DATA 1,2,3,4,5 .....
```
The embedded Z80 assembler code is:
```perl
  LD HL, 0FD00
  LD A,1  ; screen memory on
  OUT (0FFh),A
  LD B, 0
REP:  LD A,L
      LD (HL),A
      INC HL
      DJNZ REP
  XOR  A
  OUT (0FFh),A  ; screen memory off
  RET
```



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

