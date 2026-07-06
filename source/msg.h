#ifndef _MSG_H
#define _MSG_H

/* ======================
 * Game intro messages 
 * ======================
 */

#define WELCOME_MSG \
"\n\n"\
"\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n"\
"\t                   WELCOME TO THE DRAGON'S DEN                    \n"\
"\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n"\
"\t                                                                  \n"\
"\t                ^    ^                                            \n"\
"\t               / \\  //\\                                         \n"\
"\t |\\___/|      /   \\//  .\\                                      \n"\
"\t /O  O  \\__  /    //  | \\ \\                                    \n"\
"\t /  /    \\/_/    //   |  \\  \\                                  \n"\
"\t@__@'     \\/_   //    |   \\   \\                                \n"\
"\t   |       \\/_ //     |    \\    \\                              \n"\
"\t   |        \\///      |     \\     \\                            \n"\
"\t  _|_ /   )  //       |      \\     _\\                           \n"\
"\t '/,_ _ _/  ( ; -.    |    _ _\\.-~        .-~~~^-.               \n"\
"\t ,-{        _      `-.|.-~-.           .~         `.              \n"\
"\t  '/\\      /                 ~-. _ .-~      .-~^-.  \\           \n"\
"\t     `.   {            }                   /@@@@@ \\  \\          \n"\
"\t@@ .----~-.\\        \\-'                 .~ @@@@@@@@\\  `. \\^-. \n"\
"\t@ ///.----..>    c   \\             _ -~@@@@@@@@@@@@@`.  ^-`   ^-_\n"\
"\t@@@ ///-._ _ _ _ _ _ _}^ - - - - ~  @@@@@@@@@@@@@@@@@@ ~--,   .-~ \n"\
"\t@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ /.-'    \n"\
"\t @@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@@     @@@@@@@@@@@@@@@     \n"\
"\n\n"

#define NAME_REQUEST_MSG \
"\n\tWhat is your name? "

#define GREETING_MSG \
"\n\n\tHello %s!\n"

/* ======================
 * Menus
 * ======================
 */
#define MAIN_MENU_MSG \
"\n\t~~~~~~~~~~\n"\
"\n\t[N]ew\n"\
"\t[L]oad\n"\
"\t[S]ave\n"\
"\t[C]ontinue\n"\
"\t[H]elp\n"\
"\t[Q]uit\n"\
"\n\t--> "

#define GAME_MENU_MSG \
"\n[M]ove to room (specify id after M)\n"\
"[S]hoot arrow into room (specify id after S)\n"\
"[E]xit the caves\n"\
"\n--> "

#define INSTRUCTIONS_MSG \
"\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"\
"\tThe Dragon's Den is a cave system of interconnected rooms.\n"\
"\tEach room has passages to other rooms that are helpfully labeled with\n"\
"\tunique id numbers (thanks to adventurers like you!)\n"\
"\n\tYou move from one room to another by typing M and the room's id number.\n"\
"\n\tBut be careful!\n"\
"\n\tSome rooms are mined. If you enter a room trapped with a mine, then\n"\
"\tit explodes, destroying that room and teleporting you to a random room!\n"\
"\n\tSome rooms contain unstable portals. The portals make a buzzing sound\n"\
"\tthat you can hear from the next room over. If you enter a portal room,\n"\
"\tthe portal is automatically activated, it pulls you in and teleports you\n"\
"\tinto a new unexplored room!\n"\
"\n\tThe dragon has its lair in one of the rooms. Thankfully it smells bad\n"\
"\tenough that you can smell it from the next room over!\n"\
"\n\tEven more thankfully, you are equipped with a bow and magic arrows.\n"\
"\n\tType S followed by a neighboring room's id number to shoot an arrow\n"\
"\tinto that room. If you are lucky, you'll hit the dragon and win!\n"\
"\n\tIf you run out of arrows, don't despair. You may find extra arrows\n"\
"\tin some rooms, left there by other unlucky adventurers.\n"\
"\n\tNeedless to say, if you walk into the dragon's lair, you will also\n"\
"\tbecome one of those unlucky adventurers.\n"\
"\n\tHave fun!\n"\
"\n\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"

#define WRONG_SELECTION_MSG \
"\nUnrecognized selection.\n"

#define CONFIRM_NEW_GAME_MSG \
"\nA game is currently in progress."\
" Are you sure you want to start a different game? (y/n) "

#define CONFIRM_QUIT_MSG \
"\nA game is currently in progress."\
" Are you sure you want to quit? (y/n) "

#define NO_GAME_IN_PROGRESS_MSG \
"\nThere is no game in progress. Start a new game or load a game.\n"

/* ======================
 * Room descriptions 
 * ======================
 */
#define ENTER_ROOM_MSG \
"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"\
"\nYou look around the room you are in.\n"

#define TRIGGERED_MINE_MSG \
"\nOops! You triggered a mine. It exploded and the room is collapsing!!\n"\
"\nYou black out and wake up in another room.\n"

#define TRIGGERED_PORTAL_MSG \
"\nThere is a mysterious portal in the room.\n"\
"\nOh no, it pulls you in!\n"

#define FOUND_ARROW_MSG \
"\nYou found an arrow. You now have %d arrows.\n"

#define ENCOUNTERED_DRAGON_MSG \
"\nThere's a dragon in the room! Back up! Back up! ... Too late ... RIP.\n"

/* ======================
 * Neighboring room info
 * ======================
 */

#define PASSAGE_INFO_MSG \
"\nThere are %d passages to other rooms:"

#define DETECTED_DRAGON_MSG \
"\nYou smell a dragon.\n"

#define DETECTED_PORTAL_MSG \
"\nYou hear a buzzing sound.\n"

/* ======================
 * After action report
 * ======================
 */

#define NO_PASSAGE_MSG \
"\nThere is no passage to that room. Try again!\n"

#define SHOT_DRAGON_MSG \
"\nYou shot the dragon and claimed the loot! Congratulations!\n"

#define ARROW_MISSED_MSG \
"\nYour arrow hit a wall. You now have %d arrows.\n"

#define NO_ARROWS_MSG \
"\nYou reach for an arrow and grasp only air.\n"\
"\nTry to look for more arrows in other rooms.\n"
 
/* ======================
 * File I/O messages
 * ======================
 */

#define FILE_EXISTS_MSG \
"\nsave_file exists. Overwrite? (y/n) "

#define CORRUPTED_ERROR_MSG \
"\nCorrupted save_file.\n"

#define OPEN_ERROR_MSG \
"\nCould not open file.\n"

#define WRITE_ERROR_MSG \
"\nError saving game.\n"

#define READ_ERROR_MSG \
"\nError loading game.\n"

#define SAVE_OK_MSG \
"\nGame saved.\n"

#endif
