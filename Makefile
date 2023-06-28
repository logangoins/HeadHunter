serverfiles := ${wildcard src/server/*}
headhunter: ${serverfiles}
	cc -Wall src/server/headhunter.c -o headhunter -lpthread

install: headhunter
	cp headhunter /usr/bin/
	mkdir -p /usr/share/headhunter 
	cp -r * /usr/share/headhunter/

remove:
	rm /usr/bin/headhunter
	rm -rf /usr/share/headhunter/
