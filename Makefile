serverfiles := ${wildcard src/server/*}
headhunter: ${serverfiles}
	gcc -Wall src/server/headhunter.c -o headhunter -lpthread

install: headhunter
	cp headhunter /usr/bin/
	mkdir -p /usr/lib/headhunter/ 
	cp src/payload/* /usr/lib/headhunter/

remove:
	rm /usr/bin/headhunter
	rm -rf /usr/lib/headhunter/
