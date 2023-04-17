//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef QUERYCACHE_H
#define QUERYCACHE_H
#ifdef _WIN32
#pragma once
#endif

#if defined(TERROR)
#define SUPPORT_QUERY_CACHE 1
#else
#define SUPPORT_QUERY_CACHE 0
#endif

#include "tier0/platform.h"

// this system provides several piece of functionality to ai or other systems which wish to do
// traces and other trace-like queries. 

// a. By maintaining a set of incrementally updated trace results, it makes it simple to have ai
// code use hyteresis on traces as an optimization method.

// b. By updating the cache entries outside of the entity think functions, the update is done in a
// fully multi-threaded fashion


enum EEntityOffsetMode_t
{
	EOFFSET_MODE_WORLDSPACE_CENTER,
	EOFFSET_MODE_EYEPOSITION,
	EOFFSET_MODE_NONE,										// nop
};

// Compile out query cache structures if we don't use them
#if SUPPORT_QUERY_CACHE

#include "mathlib/vector.h"

enum EQueryType_t
{
	EQUERY_INVALID = 0,									// an invalid or unused entry
	EQUERY_TRACELINE,
	EQUERY_ENTITY_LOS_CHECK,

};

#define QCACHE_MAXPNTS 3									// maximum number of points/entities
															// involved in a query

struct QueryCacheKey_t
{
	EQueryType_t m_Type;
	int m_nNumValidPoints;
	Vector m_Points[QCACHE_MAXPNTS];
	EHANDLE m_pEntities[QCACHE_MAXPNTS];
	EEntityOffsetMode_t m_nOffsetMode[QCACHE_MAXPNTS];
	unsigned int m_nTraceMask;
	unsigned int m_nHashIdx;
	int m_nCollisionGroup;
	ShouldHitFunc_t m_pTraceFilterFunction;

	float m_flMinimumUpdateInterval;

	void ComputeHashIndex( void );

	bool Matches( QueryCacheKey_t const *pNode ) const ;
};

struct QueryCacheEntry_t
{
	QueryCacheEntry_t *m_pNext;
	QueryCacheEntry_t *m_pPrev;
	QueryCacheKey_t m_QueryParams;
	float m_flLastUpdateTime;
	bool m_bUsedSinceUpdated;								// was this cell referenced?
	bool m_bSpeculativelyDone;
	bool m_bResult;											// for queries with a boolean result

	void IssueQuery( void );

};

#endif // SUPPORT_QUERY_CACHE

// Keeping function stubs for compatibility
bool IsLineOfSightBetweenTwoEntitiesClear( CBaseEntity *pSrcEntity,
										   EEntityOffsetMode_t nSrcOffsetMode,
										   CBaseEntity *pDestEntity,
										   EEntityOffsetMode_t nDestOffsetMode,
										   CBaseEntity *pSkipEntity,
										   int nCollisionGroup,
										   unsigned int nTraceMask,
										   ShouldHitFunc_t pTraceFilterCallback,
										   float flMinimumUpdateInterval = 0.2
	);



// call during main loop for threaded update of the query cache
void UpdateQueryCache( void );

// call on level transition or other significant step-functions
void InvalidateQueryCache( void );

#endif // querycache_h
