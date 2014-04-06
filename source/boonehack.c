#include "g_local.h"
#include "boonehack.h"

edict_t *g_edicts;

void booneTeamWin(int winner, struct game_locals_s* game)
{
	int team = (winner==1)?2:1;
	int i;
	edict_t* cl_ent;
	for (i = 0; i < game->maxclients; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (cl_ent->inuse)
		{
			if( cl_ent->client->resp.team == team )
			{
				printf("%s is on team %i and therefore needs a booneKamp!\n", cl_ent->client->pers.netname, team );
				cl_ent->client->resp.boone.stat[BOONE_TAKE]++;
			}

		}
	}
	printf("{ \"booneEvent\":\"teamWin\", \"Team\":%i }\n", winner);
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

	switch(eventType)
	{
	case BOONE_PLUM:
		printf("{ \"booneEvent\":\"plu\", \"klutz\":\"%s\" }\n", victim->client->pers.netname );
	break;
	case BOONE_IMPRESSIVE:
		printf("{ \"booneEvent\":\"imp\", \"victim\":\"%s\", \"attacker\":\"%s\" }\n", victim->client->pers.netname, attacker->client->pers.netname );
	break;
	case BOONE_ACCURACY:
		printf("{ \"booneEvent\":\"acc\", \"victim\":\"%s\", \"attacker\":\"%s\" }\n", victim->client->pers.netname, attacker->client->pers.netname );
	break;
	case BOONE_EXCELLENT:
		printf("{ \"booneEvent\":\"exc\", \"victim\":\"%s\", \"attacker\":\"%s\" }\n", victim->client->pers.netname, attacker->client->pers.netname );
	break;
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
