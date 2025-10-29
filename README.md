# CharlieplexModule
Example script for an unnamed charlieplex double 7-segment display for Arduino Nano

# Back Story
One day I found half of a broken baby food maker, which still had an intact motor and control panel. The control panel intrigued me since it lacks any membrane switches or mechanical buttons and instead appeared to operate by capacitance. I tried to deduce how to connect it to a UART or other interface but then gave up and decided to extract just the LED module. 

![c-plex module](https://github.com/leftwebbing/CharlieplexModule/blob/main/CharliePlex.jpg)
![side of c-plex module with part number](https://github.com/leftwebbing/CharlieplexModule/blob/main/CharliePlexPartNumber.jpg)
![back of c-plex module with mystery pinout](https://github.com/leftwebbing/CharlieplexModule/blob/main/CharliePlexBack.jpg)

This is the baby food maker where it came from [https://www.alibaba.com/product-detail/detail_1601604703955.html]

![baby food maker where I found the c-plex module](https://s.alicdn.com/@sc04/kf/H4e8adeb6761047afa0ea010f09b5c9fdd.jpg_150x150.jpg)

Its part number doesn't show up anywhere so I started testing individual pins. A module with only pins looked unusual and inadequate to control the 7 + 1 + 7 + 1 = 16 elements on its display. Surprisingly, two different elements lit up when I applied a 3V coin battery to two terminals. 

This module turns out to operate with a “Charlieplex” framework. Its two top pins are not connected and not involved in controlling the elements. With 5 pins, a total of n * (n-1) = 5* (5-1) = 20 elements can be controlled. 

It's one of these but with two digits and two decimal points
![single 7-segment display with decimal point](https://blog.eletrogate.com/wp-content/uploads/2019/02/Display-7-segments-1-digit-G-2048x1229.jpg)

After searching for some time, I could not find the spec sheet for the module and decided to interrogate them manually with the coin battery, and recorded this chart: 

<table>
    <tr>
        <td></td>
        <td>1 Neg</td>
        <td>2</td>
        <td>3</td>
        <td>4</td>
        <td>5</td>
    </tr>
    <tr>
        <td>1 Pos</td>
        <td>None </td>
        <td>1E</td>
        <td>1F</td>
        <td>1G</td>
        <td>None</td>
    </tr>
    <tr>
        <td>2</td>
        <td>1A</td>
        <td>None</td>
        <td>2C</td>
        <td>2D</td>
        <td>2E</td>
    </tr>
    <tr>
        <td>3</td>
        <td>1B</td>
        <td>1H</td>
        <td>None</td>
        <td>2F</td>
        <td>2G</td>
    </tr>
    <tr>
        <td>4</td>
        <td>1C</td>
        <td>2A</td>
        <td>None </td>
        <td>None</td>
        <td>2H</td>
    </tr>
    <tr>
        <td>5</td>
        <td>1D</td>
        <td>2B</td>
        <td>None </td>
        <td>none</td>
        <td>None</td>
    </tr>
</table>

So how can the module be controlled? It looked like an ideal project for an Arduino Nano. First I imagined that the various elements could be controlled by activating various subsets at a high flicker rate, but then decided to control them row-wise.  
I outlined a general algorithm for sending one of 0-9 plus other characters, then Windsurf helped A LOT by fixing my syntax and suggesting code shortcuts. 
At this point it's just a demonstration but the algorithm can be applied to other uses. 
