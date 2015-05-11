# mi3stat: multithreaded replacement for 'i3status'

(C) 2014 by lynix <lynix47@gmail.com>
        
## SUMMARY
mi3stat is a multithreaded drop-in replacement for 'i3status', a neat tool
generating status information for minimalistic status bars like i3bar, dzen2,
xmobar, etc.

I wrote mi3stat because of i3status' lack of a memory gauge, and because I
was looking for a small coding project to keep up C skills.


## REQUIREMENTS
The NVIDIA module requires the `nvidia-settings` binary in `/usr/bin` in order
to read GPU temperatures.
If support for the Aquaero 5 is enabled (`make enable_aq5=1`), a copy of
`aerotools-ng` is required (see https://github.com/lynix/aerotools-ng).


## USAGE
mi3stat supports JSON or plaintext output mode, the latter chosen by `-t` as
command line argument.
A configuration file can be specified using the `-c` argument, by default
mi3stat will look for `~/.mi3stat.conf`. See the following section for an
example configuration file.


## CONFIGURATION
The configuration file is an ini-styled plain text file holding en enumeration
of the modules to be loaded and several options. An example that shows all
available modules and their options is shown below. The option *interval* is
optional and defaults to 1 second.

    [global]
    interval = 1
    
    [module0]
    type     = cpu_usage
    prefix   = CPU: 
    
    [module1]
    type     = cpu_temp
    interval = 3
    sensor   = /sys/bus/platform/devices/coretemp.0/temp3_input
    
    [module2]
    type     = mem_usage
    prefix   = MEM: 
    interval = 5
    
    [module3]
    type     = nvidia_temp
    prefix   = GPU: 
    interval = 7
    
    [module4]
    type     = aquaero5_temp
    prefix   = WATER: 
    interval = 5
    
    [module5]
    type     = disk_free
    prefix   = HOME: 
    interval = 61
    path     = /home
    
    [module6]
    type     = disk_free
    prefix   = ARCH: 
    interval = 31
    pata     = /mnt/archive
    
    [module7]
    type     = net_rates
    prefix   = ETH: 
    device   = eth0
    
    [module8]
    type     = clock
    interval = 9
    format   = %a %d. %b  %H:%M


## BUGS / PATCHES
Feel free to report suggestions for improvements or any bugs you find using the
*issues* function on GitHub.


## LICENSE
This program is published under the terms of the GNU General Public License,
version 3. See the file 'LICENSE' for more information.
