#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include "vector.h"
#include <fstream>

typedef enum
{
    WALL = 0,
    ENTRY,
    EXIT,
    NEdgeKind
} EdgeKind_t;

typedef enum
{
    Line = 0,
    Arc,
    nSegmentKind
} SegmentKind_t;

class SectorEdge
{
    public:
	EdgeKind_t		kind;		/* !< the kind of edge */
	int			start;		/* !< index into vertex array */
};

class WallEdge
{
    public:
	int			neighbor;	/* !< index into sector array */
	int			neighborEdge;	/* !< index into the neighboring sector's edge array */
};

class Sector
{
    public:
	std::vector<SectorEdge>	edge;		/* !< index of sector edges */
};

class Segment
{
    public:
	SegmentKind_t		kind;		/* !< the kind of segment */
	int			start;		/* !< index into vertex array */
	int			end;		/* !< index into vertex array */
	float			length;		/* !< the length of the segment */
};

class LineSegment : public Segment
{
};

class ArcSegment : public Segment
{
    public:
	int			center;		/* !< index into vertex array */
	float			angle;		/* !< angle of arc from start to end radians */
};

class Lane
{
    std::vector<Segment>	segment;	/* !< segment array */
};

class Map
{
    public:
	std::string		version;	/* !< track file version */
	std::string		name;		/* !< the name of the track */
	std::string		revision;	/* !< revision number for the track */
	std::vector<Vec3f_t>	vertex;		/* !< vertex array */
	std::vector<Sector>	sector;		/* !< sector array */
	std::vector<Lane>	lane;		/* !< lane array */

	Map();
	Map(fstream);
	~Map();
	Intersect(Vec3f_t start, Vec3f_t end);
};
#endif
