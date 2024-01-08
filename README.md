
<p align="center">
  <img src="https://github.com/shellph1sh/shellph1sh.github.io/assets/55106700/38a0993b-569d-4a87-9742-675d2759407f">
</p>

# HeadHunter
HeadHunter is an adversary emulation framework and command & control (C2) server with asynchronous, beacon based encrypted communications. HeadHunter also includes compatible custom agents and a server bundled agent generator with cross compilation capabilities.
HeadHunter has functionality to generate binary and shellcode agents for Windows 32- and 64-bit, and GNU/Linux based operating systems. 

Once deployed, a Hunter agent will call back to the HeadHunter command and control infrastructure, requesting agent tasking from the operator until provided. If no commands are provided, the agent will sleep and continuously beacon until the agent is provided with instructions.  

While HeadHunter does have a GNU/Linux agent, development will mostly be focused on the Windows agent, meaning the GNU/Linux agent will have minimal support.

## Dependencies
mingw-w64 (For cross compilation to Windows)

gcc

make

objdump (For shellcode generation)

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

## Extra Notes

I am not liable for any damage caused by this software. This software is under the discretion of the end user.
