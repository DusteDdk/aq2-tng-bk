#ifndef BOONEHACK_INCLUDED
#define BOONEHACK_INCLUDED

#define BOONE_IMPRESSIVE	0
#define BOONE_ACCURACY		1
#define BOONE_EXCELLENT		2
#define BOONE_PLUM			3
#define BOONE_GIVE			4
#define BOONE_TAKE			5

#define BOONE_OUT			-1
#define BOONE_IN			1

#define BOONE_HTML  0
#define BOONE_JSON  1

#define BOONE_LOG   (sv_boonejson->string[0] || sv_boonehtml->string[0])

#define BOONE_TEAM_TIE  32
static const int boonePrice[4] = { 1, 2, 3, 1 };


typedef struct booneBuddy_s
{
	char name[16];
	int boone;
	int given[3], taken[3];
	struct booneBuddy_s* next;
} booneBuddy_t;

typedef struct 
{
	int stat[6];
	booneBuddy_t* buddies;
} booneStats_t;

struct game_locals_s;
void booneBeginRound();
void booneTeamWin(int winner, struct game_locals_s* game, int teamAscore, int teamBscore);
void booneEvent(int eventType, edict_t* victim, edict_t* attacker );
void booneClear(edict_t* ent);
char* booneEventName(int i);
void booneWrite( int log, char* str );
void booneTime( char* ltm );
void booneShutdown();
#ifdef WIN32
static int booneSrvThread(void *unused);
#endif

void booneSrvStart();

#endif
