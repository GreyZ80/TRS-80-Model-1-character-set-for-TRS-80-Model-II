# TRS-80-Model-1-character-set-for-TRS-80-Model-II
A new character generator that brings the TRS-80 Model 1 character set, including the 2x3 graphics to the Big Tandy machines. No hardware modification, just a 2732 (4Kx8) EPROM and a wire with clip. Reverse text is still possible, but the Chr code for it changed. 

IMPORTANT:
Use an EPROM that is fast enough for the pixel generation. This means that the Access Time should be less or equal to 350ns. Using slower EPROMS (e.g. 450ns access time) will result in anoying flickering pixels with some characters. 
