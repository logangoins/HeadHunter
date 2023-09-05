<img src="https://socialify.git.ci/Lionskey/Headhunter/image?description=1&descriptionEditable=Command%20and%20Control%20(C2)%20Server%20and%20Payload%20Generator&font=Bitter&language=1&name=1&owner=1&pattern=Circuit%20Board&theme=Dark">

# HeadHunter
HeadHunter is a reverse shell handler for GNU Linux based operating systems. 
HeadHunter has functionality to generate binary payloads for GNU Linux and Microsoft Windows 64 and 32 bit operating systems.
You can also use any other reverse shell payload (netcat, powershell, etc.) while keeping full functionality with HeadHunter.

## Notes

HeadHunter operates very similar to a netcat listener, if a full TTY session on a GNU/Linux machine is required you have to spawn it yourself with a command like this:
```
python3 -c 'import pty; pty.spawn("/bin/bash")'
```
or any of its alternatives

I am not liable for any damage caused by this software. This software is for educational purposes only. This software is under the discretion of the end user.

## Dependencies
mingw-w64 (For cross compliation to Windows)

gcc

make

## Installation 

1. Clone the repository
``` 
git clone https://github.com/Lionskey/HeadHunter.git
```

2. Change directory to source tree
``` 
cd HeadHunter/
```

3. Install HeadHunter binary and payload source
```
make && sudo make install
```

4. Enjoy
``` 
headhunter --help
``` 
