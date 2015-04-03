#include <time.h>
#include <stdio.h>
#include <string.h>

#include "g_local.h"
#include "boonehack.h"

edict_t *g_edicts;

extern level_locals_t level;

static FILE* b_logfp[2] = { NULL, NULL };

void booneTeamWin(int winner, struct game_locals_s* game, int teamAscore, int teamBscore)
{
	int loserteam = (winner==1)?2:1;
	int i, trii;
        char buf[1024];
        char timeStr[64];
        char winStr[64];
        char buf2[64];

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
            
            char* tra = "bgcolor=\"#f0f0f0\"";
            char* trb = "bgcolor=\"#d5d5d5\"";
            
            
            
            booneTime(timeStr);
            sprintf(buf,"{ \"booneEvent\":\"teamWin\", \"Team\":%i, \"time\":\"%s\", \"teama\":%i, \"teamb\":%i, \"levelname\":\"%s\" }\n", winner,timeStr, teamAscore, teamBscore, level.level_name );
            booneWrite(BOONE_JSON, buf);

            if( winner == BOONE_TEAM_TIE )
            {
                loserteam = BOONE_TEAM_TIE;
                sprintf(winStr, "Teams are tied!", winner );
            } else {
                sprintf(winStr, "Team %i wins!", winner );               
            }
            
            sprintf(buf,"<b><h3>[%s] Round Over - %s</h3>Map: %s -- %s<br>[Team1] %s : %i<br>[Team2] %s : %i</b><br><br>\n", timeStr, winStr,level.mapname, level.level_name, teams[1].name, teams[1].score,teams[2].name, teams[2].score );
            booneWrite(BOONE_HTML, buf);
            
            booneWrite(BOONE_HTML, "<table border=1><tr bgcolor=\"#FF4D4D\"><td colspan=11><b>Scores</b></td></tr><tr bgcolor=\"#99CCFF\"><td><b>Team</b></td><td><b>Player</b></td><td><b>Score</b><td><b>Shots</b></td><td><b>Accuracy</b></td><td><b>Accuracy Awards</b></td><td><b>Impressive Awards</b></td><td><b>Excellent Awards</b></td><td><b>Plums</b></td><td><b>booneGive</b></td><td><b>booneTake</b></td></tr>\n");

            trii=0;
            
            int teamSort;
            
            for( teamSort = 0; teamSort < 2; teamSort ++ )
            {
            
                for (i = 0; i < game->maxclients; i++)
                {
                    cl_ent = g_edicts + 1 + i;
                    if (cl_ent->inuse && ( (teamSort==0 && cl_ent->client->resp.team == winner)||(teamSort==1 && cl_ent->client->resp.team == loserteam)||(teamSort==1 && winner==BOONE_TEAM_TIE ) ) )
                    {
                        score = cl_ent->client->resp.score;
                        shots = cl_ent->client->resp.stats_shots_t;
                        if (shots > 9999)
                            shots = 9999;
                        if (shots != 0)
                            accuracy =(((double)cl_ent->client->resp.stats_shots_h / (double) shots) * 100.0);
                        else
                            accuracy = 0;

                        
                        sprintf(buf, "<tr %s><td>%i</td><td>%s</td><td>%i</td><td>%i</td><td>%.2f %%</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td><b>%i</b></td></tr>\n", ((trii++%2==0)?tra:trb),
                            cl_ent->client->resp.team, cl_ent->client->pers.netname,score, shots, accuracy,
                            cl_ent->client->resp.boone.stat[BOONE_ACCURACY],cl_ent->client->resp.boone.stat[BOONE_IMPRESSIVE], cl_ent->client->resp.boone.stat[BOONE_EXCELLENT], cl_ent->client->resp.boone.stat[BOONE_PLUM],
                            cl_ent->client->resp.boone.stat[BOONE_GIVE], cl_ent->client->resp.boone.stat[BOONE_TAKE] );
                        booneWrite(BOONE_HTML, buf);

                    }
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
                    if( !it && !cl_ent->client->resp.boone.stat[BOONE_PLUM] && cl_ent->client->resp.team != loserteam )
                    {
                        sprintf(buf, "<b>%s was not involved in any boone action?! Lazy SOB!</b><br>\n",cl_ent->client->pers.netname);
                        booneWrite(BOONE_HTML, buf);
                    } else {
                        sprintf(buf, "\n<table border=1><tr bgcolor=\"#99FF99\"><td colspan=8><b>%s's booneBuddies</b></td></tr><tr bgcolor=\"#99CCFF\"><td><b>booneBuddy</b></td><td><b>Imp Take</b></td><td><b>Imp Give</b></td><td><b>Acc Take</b></td><td><b>Acc Give</b></td><td><b>Exc Take</b></td><td><b>Exc Give</b></td><td><b>boone</b></td></tr>\n",cl_ent->client->pers.netname);
                        booneWrite(BOONE_HTML, buf);
                            
                        trii=0;

                        if( it )
                        {
                            while( (it=it->next) )
                            {
                                
                                
                                if( (it->boone) > 0 )
                                {
                                    sprintf( buf2, "<td bgcolor=\"#ff6464\">%i</td>", (it->boone) );
                                } else if( (it->boone) < 0 )
                                {
                                    sprintf( buf2, "<td bgcolor=\"#84ac7c\">%i</td>", (it->boone) );
                                } else {
                                    sprintf( buf2, "<td>0</td>" );
                                }
                                
                                    sprintf(buf,"<tr %s><td>%s</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td><td>%i</td>%s</tr>\n",((trii++%2==0)?tra:trb),it->name,
                                                    it->taken[BOONE_IMPRESSIVE], it->given[BOONE_IMPRESSIVE],
                                                    it->taken[BOONE_ACCURACY], it->given[BOONE_ACCURACY],
                                                    it->taken[BOONE_EXCELLENT], it->given[BOONE_EXCELLENT], buf2);
                                    booneWrite(BOONE_HTML, buf);
                            }
                        }
                        
                        if( cl_ent->client->resp.team == loserteam )
                        {
                                    sprintf(buf,"<tr %s><td><b>Your Team</b></td><td colspan=6>%s = losers = boone</td><td bgcolor=\"#ff6464\">1</td></tr>\n",((trii++%2==0)?tra:trb), teams[loserteam].name);
                                    booneWrite(BOONE_HTML, buf);
                        }                            
                        
                        if( cl_ent->client->resp.boone.stat[BOONE_PLUM] )
                        {
                                    sprintf(buf,"<tr %s><td><b>The World</b></td><td colspan=6>Gravity + solid ground = plum = boone</td><td bgcolor=\"#ff6060\">%i</td></tr>\n",((trii++%2==0)?tra:trb), cl_ent->client->resp.boone.stat[BOONE_PLUM] );
                                    booneWrite(BOONE_HTML, buf);
                        }

                        booneWrite(BOONE_HTML, "</table><br>\n");
                    }
                }
            }
    
            
            
            booneWrite(BOONE_HTML, "<br><hr><br>\n");

            if( b_logfp[BOONE_JSON] )
            {
                fclose(  b_logfp[BOONE_JSON] );
                b_logfp[BOONE_JSON]=NULL;
            }

            if( b_logfp[BOONE_HTML] )
            {
                fclose(  b_logfp[BOONE_HTML] );
                b_logfp[BOONE_HTML]=NULL;
            }
            
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

        booneSrvStart();
        
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


// The "webserver" - Listen to a port, on incoming connection, ignre request, stream all data from file and close connection.
// TODO: Set header refresh time to 2 seconds after match end?

#ifdef WIN32
#include <io.h>
#include <winsock2.h>
#define bzero(a,b)		memset(a,0,b)
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>
#endif
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>






static int booneSrvRunning=0;
static void booneSrvStart()
{
if(!booneSrvRunning)
{
    if( !booneSrvRunning )
    {
#ifdef WIN32
 DWORD tid;
        booneSrvRunning=1;
        printf("Starting booneserver on port 8080.\n");
      CloseHandle (CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) booneSrvThread,
				 NULL, 0, &tid));   
#else
        printf("booneserver not supported for unix yet.\n");
#endif
    }
}
}

#ifdef WIN32
static int
booneSrvThread(void *unused)
{
  int sok, read_sok, len, flen;
  char *p;
  char buf[512];
  char outbuf[512];
  char timeStr[64];
  struct sockaddr_in addr;

  sok = socket (AF_INET, SOCK_STREAM, 0);
  if (sok == INVALID_SOCKET)
  {
      printf("BOONE_WEB_DEBUG: Invalid socket\n");
    return 0;
  }

  len = 1;
  setsockopt (sok, SOL_SOCKET, SO_REUSEADDR, (char *) &len, sizeof (len));

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons (8080);

  if (bind (sok, (struct sockaddr *) &addr, sizeof (addr)) == SOCKET_ERROR)
    {
      printf("BOONE_WEB_DEBUG: Socket Bind Error\n");
      closesocket (sok);
      return 0;
    }

  if (listen (sok, 1) == SOCKET_ERROR)
    {
      printf("BOONE_WEB_DEBUG: Socket Listen Error\n");
      closesocket (sok);
      return 0;
    }

  len = sizeof (addr);
  
  while( 1 )
  {
    read_sok = accept (sok, (struct sockaddr *) &addr, &len);
    
    if (read_sok == INVALID_SOCKET)
    {
        printf("BOONE_WEB_DEBUG: Socket Accept Error\n");
        return 0;
    }

    printf("BOONE_WEB_DEBUG: Servicing request from %s\n", inet_ntoa (addr.sin_addr));
    
            
    recv (read_sok, buf, sizeof (buf) - 1, 0);
    buf[sizeof (buf) - 1] = 0;        /* ensure null termination */
    
    printf("BOONE_WEB_DEBUG: recv: '%s'\n", buf);

    if( sv_boonehtml->string[0] )
    {
        FILE* fp;
        if( fp= fopen( sv_boonehtml->string, "rb" ) )
        {
            booneTime(timeStr);
            fseek(fp, 0, SEEK_END);
            flen = ftell(fp);
            
            char* data = malloc( flen+1 );
            data[flen]=0;
            
            sprintf(outbuf, "HTTP/1.1 200 OK\r\nServer: BooneHack\r\ncache-control: private, max-age=1, no-cache\r\nPragma: no-cache\r\nContent-Type: text/html; charset=ISO-8859-1\r\n\r\n");
            send(read_sok, outbuf, strlen(outbuf), 0);

            sprintf(outbuf, "<html><head><title>aq2tng-booneHack by DST - %s</title></head><body><h2>AQ2:TNG-booneHack Stats</h2>\n", timeStr);
            send(read_sok, outbuf, strlen(outbuf), 0);
            
            rewind(fp);
            int rs=fread(data, 1, flen, fp);
            if( rs != flen )
            {
                printf("Boonedebug: expected to read %i, but read %i (strlen=%i)\n", flen,rs,strlen(data) );
            }
            send(read_sok, data, rs, 0);
            
            free(data);

            sprintf(outbuf, "</body></html>");
            send(read_sok, outbuf, strlen(outbuf), 0);

            fclose(fp);
        } else {
            sprintf(outbuf, "Could not open '%s' for reading, maybe there are no stats recorded yet.",sv_boonehtml->string );
            send(read_sok, outbuf, strlen (outbuf), 0);
        }
    } else {
        sprintf(outbuf, "boonehtml is disabled, ask the server admin to set boonehtml to a filename." );;        
        send(read_sok, outbuf, strlen (outbuf), 0);
    }

    //sleep (1);
    closesocket (read_sok);
//    
  }
}

#endif
