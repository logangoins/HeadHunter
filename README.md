<img src="https://socialify.git.ci/Lionskey/HeadHunter/image?description=1&font=Source%20Code%20Pro&logo=https%3A%2F%2Fi.redd.it%2F6isihfajc73a1.png&name=1&owner=1&pattern=Solid&theme=Dark">

# HeadHunter
HeadHunter is an adversary emulation framework and command & control (C2) server with XOR encrypted communications along with a bundled agent generator with cross compilation capabilities. 
HeadHunter has functionality to generate binary agents for GNU/Linux and Windows 32- and 64-bit operating systems. 
 
## Notes

I am not liable for any damage caused by this software. This software is under the discretion of the end user.

## Dependencies
mingw-w64 (For cross compilation to Windows)

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
headhunter
``` 
