/*! \file track-parser.h
 *
 * \author John Reppy
 *
 * Track-file loader.
 */

/*
 * COPYRIGHT (c) 2010 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _TRACK_PARSER_H_
#define _TRACK_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Utilities/vector.h"	// for the Vec3f_t type

typedef struct struct_sect Sector_t;
typedef struct struct_edge Edge_t;
typedef struct struct_lane Lane_t;
typedef struct struct_segment Segment_t;

typedef enum {			//!< kinds of walls
    WALL_EDGE,
    ENTRY_EDGE,
    EXIT_EDGE
} EdgeKind_t;

typedef enum {			//!< kinds of segments
    LINE_SEGMENT,
    ARC_SEGMENT
} SegKind_t;

typedef struct {
    char	*name;		//!< track name
    int		nVerts;		//!< number of vertices
    int		nSects;		//!< number of sectors
    int		nLanes;		//!< number of lanes
    Vec3f_t	*verts;		//!< array of #nVerts vertices
    Sector_t	*sects;		//!< array of #nSects sectors
    Lane_t	*lanes;		//!< array of #nLanes lanes
} TrackData_t;

struct struct_sect {
    uint16_t	id;		//!< ID of this sector
    uint16_t	nEdges;		//!< number of edges
    Edge_t	*edges;		//!< the array of edges in CCW order
    
};

struct struct_edge {
    uint16_t	kind;		//!< edge kind
    uint16_t	start;		//!< index of start vertex for this edge
    uint16_t	neighborS;	//!< sector index of neighbor connected by this edge.
    uint16_t	neighborE;	//!< edge index of this edge in the neighbor sector
};

struct struct_lane {
    int		nSegs;		//!< the number of segments in the lane
    Segment_t	*segs;		//!< the array of segments that define the lane
};

struct struct_segment {
    uint16_t	kind;		//!< the kind of segment
    uint16_t	start;		//!< the start vertex of the segment
    uint16_t	end;		//!< the end vertex of the segment
    uint16_t	center;		//!< vertex ID for center point of an arc segment
    float	length;		//!< length of the segment
    float	angle;		//!< angle of an arc segment
};

/*! \brief load a track file
 *  \param file the name of the track file
 *  \return the track-file data or 0 if there was an error.
 */
TrackData_t *LoadTrackData (const char *file);

/*! \brief free a track file.
 */
void FreeTrackData (TrackData_t *data);

#ifdef __cplusplus
}
#endif
#endif /* !_TRACK_PARSER_H_ */
