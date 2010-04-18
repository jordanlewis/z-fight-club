/*! \file track-parser.c
 *
 * \author John Reppy
 */

/*
 * COPYRIGHT (c) 2010 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "Utilities/defs.h"
#include "track-parser.h"
#include "json.h"
#include <string.h>
#include <strings.h>

/*! \brief copy a string value from a JSON value
 */
static inline char *CopyString (JSON_Value_t *v)
{
    const char *s = JSON_GetString (v);
    if (s == 0)
	return 0;
    else
	return strcpy (NEWVEC(char, strlen(s)+1), s);

}

/*! \brief get an unsigned 16-bit integer field
 */
static inline bool GetUInt16Field (JSON_Value_t *obj, const char *fld, uint16_t *n)
{
    int		i;
    if (JSON_GetInt(JSON_GetField(obj, fld), &i)
    && (0 <= i) && (i < 65536)) {
	*n = (uint16_t)i;
	return true;
    }
    else
	return false;
}

/* LoadTrackData:
 */
TrackData_t *LoadTrackData (const char *file)
{
    JSON_Value_t *root = JSON_ParseFile (file);
    
    if (root == 0)
	return 0;

  // allocate and initialize the track data structure
    TrackData_t *data = NEW(TrackData_t);
    bzero (data, sizeof(TrackData_t));
    
  // get the track name
    if ((data->name = CopyString (JSON_GetField(root, "name"))) == 0)
	goto error_cleanup;

  // get the numbers of track elements
    if (!JSON_GetInt(JSON_GetField(root, "num-vertices"), &(data->nVerts))
    ||  !JSON_GetInt(JSON_GetField(root, "num-sectors"), &(data->nSects))
    ||  !JSON_GetInt(JSON_GetField(root, "num-lanes"), &(data->nLanes)))
	goto error_cleanup;

  // get the vertices
    if ((data->nVerts < 0) || (65536 <= data->nVerts))
	goto error_cleanup;
    else {
	JSON_Array_t *verts = JSON_GetArray(JSON_GetField(root, "vertices"));
	if (verts->length != data->nVerts)
	    goto error_cleanup;
	data->verts = NEWVEC(Vec3f_t, data->nVerts);
	for (int i = 0;  i < data->nVerts;  i++) {
	    JSON_Array_t *vec = JSON_GetArray(verts->elems[i]);
	    if ((vec == 0) || (vec->length != 3))
		goto error_cleanup;
	    for (int j = 0;  j < 3;  j++) {
		if (!JSON_GetFloat(vec->elems[j], &(data->verts[i][j])))
		    goto error_cleanup;
	    }
	}
    }

  // get the sectors
    if ((data->nSects < 0) || (65536 <= data->nSects))
	goto error_cleanup;
    else {
	JSON_Array_t *sects = JSON_GetArray(JSON_GetField(root, "sectors"));
	if (sects->length != data->nSects)
	    goto error_cleanup;
	data->sects = NEWVEC(Sector_t, data->nSects);
	bzero (data->sects, sizeof(Sector_t)*data->nSects);
	for (int i = 0;  i < data->nSects;  i++) {
	    JSON_Value_t *sect = sects->elems[i];
	    JSON_Array_t *edges = JSON_GetArray(JSON_GetField(sect, "edges"));
	    data->sects[i].id = i;
	    if ((edges == 0)
	    || (! GetUInt16Field(sect, "num-edges", &(data->sects[i].nEdges)))
	    || (edges->length != data->sects[i].nEdges))
		goto error_cleanup;
	    data->sects[i].edges = NEWVEC(Edge_t, data->sects[i].nEdges);
	  // get edge data
	    for (int j = 0;  j < data->sects[i].nEdges;  j++) {
		JSON_Value_t *edge = edges->elems[j];
		const char *kind = JSON_GetString(JSON_GetField(edge, "kind"));
		if ((kind == 0)
		|| (! GetUInt16Field(edge, "start", &(data->sects[i].edges[j].start)))
		|| (data->nVerts <= data->sects[i].edges[j].start))
		    goto error_cleanup;
		if (strcasecmp(kind, "wall") == 0)
		    data->sects[i].edges[j].kind = WALL_EDGE;
		else if (strcasecmp(kind, "entry") == 0)
		    data->sects[i].edges[j].kind = ENTRY_EDGE;
		else if (strcasecmp(kind, "exit") == 0)
		    data->sects[i].edges[j].kind = EXIT_EDGE;
		else
		    goto error_cleanup;
		if (data->sects[i].edges[j].kind != WALL_EDGE) {
		    if ((!GetUInt16Field(edge, "neighbor", &(data->sects[i].edges[j].neighborS)))
		    ||  (data->nSects <= data->sects[i].edges[j].neighborS)
		    ||  (!GetUInt16Field(edge, "neighbor-edge", &(data->sects[i].edges[j].neighborE))))
			goto error_cleanup;
		}
	    }
	}
      /* NOTE: we are ignoring sector attributes; add code here to deal with them as necessary */
    }

  // get the lanes
    if ((data->nLanes < 0) || (65536 <= data->nLanes))
	goto error_cleanup;
    else {
	JSON_Array_t *lanes = JSON_GetArray(JSON_GetField(root, "lanes"));
	if (lanes->length != data->nLanes)
	    goto error_cleanup;
	data->lanes = NEWVEC(Lane_t, data->nLanes);
	bzero (data->lanes, sizeof(Lane_t)*data->nLanes);
	for (int i = 0;  i < data->nLanes;  i++) {
	    JSON_Value_t *lane = lanes->elems[i];
	    JSON_Array_t *segs = JSON_GetArray(JSON_GetField(lane, "segments"));
	    if ((segs == 0)
	    || !JSON_GetInt(JSON_GetField(lane, "num-segments"), &(data->lanes[i].nSegs))
	    || (segs->length != data->lanes[i].nSegs))
		goto error_cleanup;
	    data->lanes[i].segs = NEWVEC(Segment_t, data->lanes[i].nSegs);
	  // get segment data
	    for (int j = 0;  j < data->lanes[i].nSegs;  j++) {
		JSON_Value_t *seg = segs->elems[j];
		const char *kind = JSON_GetString(JSON_GetField(seg, "kind"));
		if (strcasecmp(kind, "arc") == 0) {
		    data->lanes[i].segs[j].kind = ARC_SEGMENT;
		    if ((! GetUInt16Field(seg, "center", &(data->lanes[i].segs[j].center)))
		    ||  (! JSON_GetFloat(JSON_GetField(seg, "angle"), &(data->lanes[i].segs[j].angle))))
			goto error_cleanup;
		}
		else if (strcasecmp(kind, "line") == 0)
		    data->lanes[i].segs[j].kind = LINE_SEGMENT;
		else
		    goto error_cleanup;
		if ((! GetUInt16Field(seg, "start", &(data->lanes[i].segs[j].start)))
		||  (! GetUInt16Field(seg, "end", &(data->lanes[i].segs[j].end)))
		||  (! JSON_GetFloat(JSON_GetField(seg, "length"), &(data->lanes[i].segs[j].length))))
		    goto error_cleanup;
	    }
	}
    }

    JSON_Free (root);

    return data;

  error_cleanup:  // jump to here on errors
    JSON_Free (root);
    FreeTrackData (data);
    return 0;

}

/* FreeTrackData:
 */
void FreeTrackData (TrackData_t *data)
{
    if (data == 0)
	return;

    if (data->name != 0) FREE(data->name);
    if (data->verts != 0) FREE(data->verts);
    if (data->sects != 0) {
	for (int i = 0;  i < data->nSects;  i++) {
	    if (data->sects[i].edges != 0) FREE(data->sects[i].edges);
	}
	FREE (data->sects);
    }
    if (data->lanes != 0) {
	for (int i = 0;  i < data->nLanes;  i++) {
	    if (data->lanes[i].segs != 0) FREE(data->lanes[i].segs);
	}
	FREE (data->lanes);
    }
    FREE (data);

}
