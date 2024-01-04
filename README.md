
![headhunter](https://github.com/Lionskey/HeadHunter/assets/55106700/51dd0e74-ea55-4b2e-9caf-bec3dd8dcd0c)

# HeadHunter
HeadHunter is an adversary emulation framework and command & control (C2) server with asynchronous, beacon based encrypted communications. HeadHunter also includes compatible custom agents and a server bundled agent generator with cross compilation capabilities.
HeadHunter has functionality to generate binary and shellcode agents for Windows 32- and 64-bit, and GNU/Linux based operating systems. 

Once deployed, a Hunter agent will call back to the HeadHunter command and control infrastructure, requesting agent tasking from the operator until provided. If no commands are provided, the agent will sleep and continuously beacon until the agent is provided with instructions.  

## Notes

I am not liable for any damage caused by this software. This software is under the discretion of the end user.

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
