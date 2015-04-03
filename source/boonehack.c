#include <time.h>
#include <stdio.h>
#include <string.h>

#include "g_local.h"
#include "boonehack.h"

edict_t *g_edicts;

void booneTeamWin(int winner, struct game_locals_s* game, int teamAscore, int teamBscore)
{
	int loserteam = (winner==1)?2:1;
	int i, trii;
        char buf[1024];
        char timeStr[64];
        
	edict_t* cl_ent;
	for (i = 0; i < game->maxclients; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (cl_ent->inuse)
		{
			if( cl_ent->client->resp.team == loserteam )
			{
				//printf("%s is on team %i and therefore needs a booneKamp!\n", cl_ent->client->pers.netname, team );
				cl_ent->client->resp.boone.stat[BOONE_TAKE]++;
			}

		}
	}
	
	if( BOONE_LOG )
        {
            int shots, score;
            float accuracy;
            
            char* tra = "bgcolor=\"#66FFCC\"";
            char* trb = "bgcolor=\"#D1FFF0\"";
            
            
            
            booneTime(timeStr);
            sprintf(buf,"{ \"booneEvent\":\"teamWin\", \"Team\":%i, \"time\":\"%s\", \"teama\":%i, \"teamb\":%i }\n", winner,timeStr, teamAscore, teamBscore );
            booneWrite(BOONE_JSON, buf);

            sprintf(buf,"<b>[%s] Round Over, Team%i wins, (Team1: %i vs Team2: %i)</b>\n", timeStr, winner, teamAscore, teamBscore );
            booneWrite(BOONE_HTML, buf);
            
            booneWrite(BOONE_HTML, "<br>\n<table border=1><tr bgcolor=\"#AFCAE6\"><td colspan=10><b>Stats</b></td></tr><tr bgcolor=\"#99CCFF\"><td><b>Player</b></td><td><b>Score</b><td><b>Shots</b></td><td><b>Accuracy</b></td><td><b>Accuracy Awards</b></td><td><b>Impressive Awards</b></td><td><b>Excellent Awards</b></td><td><b>Plums</b></td><td><b>booneGive</b></td><td><b>booneTake</b></td></tr>\n");

            trii=0;
            for (i = 0; i < game->maxclients; i++)
            {
                cl_ent = g_edicts + 1 + i;
                if (cl_ent->inuse)
                {
                    score = cl_ent->client->resp.score;
                    shots = cl_ent->client->resp.stats_shots_t;
                    if (shots > 9999)
                        shots = 9999;
                        if (shots != 0)
                            accuracy =(((double)cl_ent->client->resp.stats_shots_h / (double) shots) * 100.0);
                        else
                            accuracy = 0;

                            trii++;
                              sprintf(buf, "<tr %s><td>%s</td><td>%i</td><td>%i</td><td>%.2f %%</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td></tr>\n", ((trii%2==0)?tra:trb),
	       cl_ent->client->pers.netname,score, shots, accuracy,
              cl_ent->client->resp.boone.stat[BOONE_ACCURACY],cl_ent->client->resp.boone.stat[BOONE_IMPRESSIVE], cl_ent->client->resp.boone.stat[BOONE_EXCELLENT], cl_ent->client->resp.boone.stat[BOONE_PLUM],
              cl_ent->client->resp.boone.stat[BOONE_GIVE], cl_ent->client->resp.boone.stat[BOONE_TAKE] );
      booneWrite(BOONE_HTML, buf);

                }
            }
            
            //
            
            booneWrite(BOONE_HTML, "</table>\n<br>\n");

            for (i = 0; i < game->maxclients; i++)
            {
                cl_ent = g_edicts + 1 + i;
                if (cl_ent->inuse)
                {
                    booneBuddy_t* it = cl_ent->client->resp.boone.buddies;
                    if( !it )
                    {
                        sprintf(buf, "<b>%s was not involved in any booneEvents?! Lazy SOB!</b>\n",cl_ent->client->pers.netname);
                        booneWrite(BOONE_HTML, buf);
                    } else {

                        sprintf(buf, "\n<table border=1><tr bgcolor=\"#99FF99\"><td colspan=8><b>%s's booneBuddies</b></td></tr><tr bgcolor=\"#CCFFCC\"><td><u>booneBuddy</u></td><td><u>Imp Take</u></td><td><u>Imp Give</u></td><td><u>Acc Take</u></td><td><u>Acc Give</u></td><td><u>Exc Take</u></td><td><u>Exc Give</u></td><td><u>boone</u></td></tr>\n",cl_ent->client->pers.netname);
                        booneWrite(BOONE_HTML, buf);


                            

                            while( (it=it->next) )
                            {
                                trii++;
                                    sprintf(buf,"<tr %s><td>%s</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td></tr>\n",((trii%2==0)?tra:trb),it->name,
                                                    it->taken[BOONE_IMPRESSIVE], it->given[BOONE_IMPRESSIVE],
                                                    it->taken[BOONE_ACCURACY], it->given[BOONE_ACCURACY],
                                                    it->taken[BOONE_EXCELLENT], it->given[BOONE_EXCELLENT], (it->boone));
                                    booneWrite(BOONE_HTML, buf);
                            }
                            booneWrite(BOONE_HTML, "</table><br>\n");
                    }
                }
            }
    
            
            
            booneWrite(BOONE_HTML, "<br><hr><br><br>");
            
    }

}


void booneUpdate( edict_t* self, edict_t* other, int eventType, int direction )
{
	int foundBuddy=0;
	//Create list if it's not there
	if(!self->client->resp.boone.buddies)
	{
		self->client->resp.boone.buddies=malloc(sizeof(booneBuddy_t));
		self->client->resp.boone.buddies->next=NULL;
	}

	//Find player
	booneBuddy_t* it = self->client->resp.boone.buddies;

	while( (it=it->next) )
	{

		if(strcmp(it->name, other->client->pers.netname ) == 0 )
		{

			foundBuddy=1;
			break;
		}
	}

	if( !foundBuddy )
	{
		it = self->client->resp.boone.buddies;
		while(1)
		{
			if( !it->next )
			{
				break;
			}
			it=it->next;
		}

		//Create if not there
		it->next = malloc(sizeof(booneBuddy_t));
		it = it->next;
		memset( it, 0, sizeof(booneBuddy_t ) );
		strcpy( it->name, other->client->pers.netname );
	}


	it->boone += ( boonePrice[eventType]*direction );

	if( direction == BOONE_IN )
	{
		it->taken[eventType]++;
	} else {
		it->given[eventType]++;
	}

	//Recalc
	int booneGive=0;
	int booneTake=self->client->resp.boone.stat[BOONE_PLUM]*boonePrice[BOONE_PLUM];

	it = self->client->resp.boone.buddies;
	while( (it=it->next) )
	{
		if( it->boone > 0 )
		{
			booneTake+=it->boone;
		}
		if( it->boone < 0 )
		{
			booneGive+= it->boone*-1;
		}
	}
	self->client->resp.boone.stat[BOONE_GIVE] = booneGive;
	self->client->resp.boone.stat[BOONE_TAKE] = booneTake;

}

void booneEvent(int eventType, edict_t* victim, edict_t* attacker )
{

        char buf[1024];
        
        if( BOONE_LOG )
        {
            switch(eventType)
            {
            case BOONE_PLUM:
                    sprintf(buf,"{ \"booneEvent\":\"plu\", \"klutz\":\"%s\" }\n", victim->client->pers.netname );
                    booneWrite(BOONE_JSON, buf );
                    
            break;
            case BOONE_IMPRESSIVE:
                    sprintf(buf,"{ \"booneEvent\":\"imp\", \"victim\":\"%s\", \"attacker\":\"%s\" }\n", victim->client->pers.netname, attacker->client->pers.netname );
                    booneWrite(BOONE_JSON, buf );
            break;
            case BOONE_ACCURACY:
                    sprintf(buf,"{ \"booneEvent\":\"acc\", \"victim\":\"%s\", \"attacker\":\"%s\" }\n", victim->client->pers.netname, attacker->client->pers.netname );
                    booneWrite(BOONE_JSON, buf );
            break;
            case BOONE_EXCELLENT:
                    sprintf(buf,"{ \"booneEvent\":\"exc\", \"victim\":\"%s\", \"attacker\":\"%s\" }\n", victim->client->pers.netname, attacker->client->pers.netname );
                    booneWrite(BOONE_JSON, buf );
            break;
            }
        }

	if( attacker == NULL )
	{

		victim->client->resp.boone.stat[eventType]++;
		victim->client->resp.boone.stat[BOONE_TAKE] += boonePrice[eventType];
		return;
	}


	booneUpdate( victim, attacker, eventType, BOONE_IN );
	booneUpdate( attacker, victim, eventType, BOONE_OUT );
	attacker->client->resp.boone.stat[eventType]++;

}

void booneClear(edict_t* ent)
{
	memset( &ent->client->resp.boone, 0, sizeof(booneStats_t) );
}

void booneTime( char* ltm )
{
	struct tm *now;
	time_t tnow;
	tnow = time ((time_t *) 0);
	now = localtime (&tnow);
	(void) strftime (ltm, 64, "%Y-%m-%d %H:%M:%S", now);
}

void booneBeginRound()
{
        //Log it
    char buf[1024];
    char timeStr[64]="\0";
    
    booneTime(timeStr);
    
    sprintf(buf, "{ \"booneEvent\":\"newroud\", \"time\":\"%s\" }", timeStr );
    booneWrite(BOONE_JSON, buf );
    
    sprintf(buf, "<br><b>[%s] New Round</b>\n", timeStr );
    booneWrite(BOONE_HTML, buf );
}

static FILE* b_logfp[2] = { NULL, NULL };


void booneWrite( int log, char* str )
{
    cvar_t* cv;
    
    if(log==BOONE_JSON)
    {
        cv = sv_boonejson;
    } else if( log== BOONE_HTML )
    {
        cv = sv_boonehtml;
    }
    
    if( cv->string[0] == 0 || cv->string[0] == '0' ) //Empty string or a 0 character = disabled, yes that means paths  can't being with zero, I don't care.
    {
        return;
    }

    if( b_logfp[log] == NULL )
    {
        b_logfp[log] = fopen( cv->string, "a"  );
    }
    
    if( b_logfp[log] != NULL )
    {
        fwrite( str, strlen(str), 1, b_logfp[log] );
        fflush(b_logfp[log]);
    } else {
        printf("BOONE_DEBUG: fopen for %s (log %i) failed.\n", cv->string, log);
    }
}
