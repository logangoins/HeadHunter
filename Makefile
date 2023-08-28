serverfiles := ${wildcard src/server/*}
headhunter: ${serverfiles}
	cc -Wall src/server/headhunter.c -o bin/headhunter -lpthread

install: headhunter
	cp bin/headhunter /usr/bin/
	mkdir -p /usr/lib/headhunter
	cp -r src/* /usr/lib/headhunter
	mkdir -p /usr/share/headhunter 
	cp -r * /usr/share/headhunter/

remove:
	rm /usr/bin/headhunter
	rm -rf /usr/share/headhunter/
	rm -rf /usr/lib/headhunter
