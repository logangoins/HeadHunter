#ifndef _PAYLOAD_COMMON_H_
#define _PAYLOAD_COMMON_H_

#define MSG_HELLO "starting control session - type \"help\" for commands\n"
#define MSG_HELP "\n\t  Hunter Agent v1.0 Commands\n-------------------------------------------\nshell <command> - task the agent to run a command\ndownload <file> - exfiltrate a file through the agent\nhelp - displays this menu\nbg - backgrounds the agent command session\nexit - tasks the agent to exit\n\n"
#define MSG_EXIT_CMD "\nExiting command session\n"
#define MSG_INVALID "\nInvalid command session command, type \"help\" for a list of commands\n"

#endif
