I used break to make breakpoints around the lines I deemed important, display to display what was happening to p and q and result and info locals to see the state of everything as well as normal stuff like continue and step

The array that gets printed is getting corrupted when we have an out-of-bounds error on line 27. data4 only has like 60 slots, but we are overwriting data[100], etc. which must go into stringToPrint. 

(gdb) info locals
offset = 75
i = 150
data = "\000\000\000m-!nbzc"
stringToPrint = "Some additional text\377\377"
data3 = "\330\377\000\000\202\bo\367\000\000\000\000\000\000楢\237\000\000"
data4 = "Well, maybe just a little uis to eat xl,ijtst loe  un!"

I put a break in between each of the for loops, found it must happen in the last for loop, and then put breaks around line 27 and displayed offset, data4[offset], and stringToPrint to see when stringToPrint got corrupted.
