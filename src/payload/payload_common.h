#ifndef _PAYLOAD_COMMON_H_
#define _PAYLOAD_COMMON_H_

#define MSG_HELLO "starting control session - type \"help\" for commands\n"

#define MSG_HELP "\n\t  Hunter Agent v1.0 Commands\n-------------------------------------------\nshell <command> - task the agent to run a command\nhelp - displays this menu\nexit - exits back to command server\n\n"

#define MSG_EXIT_CMD "\nExiting control session\n"
#define MSG_INVALID                                                            \
    "\nInvalid control session command, type \"help\" for a list of "          \
    "commands\n"

#endif
