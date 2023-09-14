serverfiles := ${wildcard src/server/*}
headhunter: ${serverfiles}
	cc -Wall src/server/headhunter.c -o headhunter -lpthread

clean:
	rm headhunter

install: headhunter
	cp headhunter /usr/bin/
	mkdir -p /usr/lib/headhunter
	cp -r src/* /usr/lib/headhunter
	mkdir -p /usr/share/headhunter 
	cp -r * /usr/share/headhunter/

remove:
	rm /usr/bin/headhunter
	rm -rf /usr/share/headhunter/
	rm -rf /usr/lib/headhunter
