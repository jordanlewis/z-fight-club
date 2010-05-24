import Blender
import re
import simplejson as json

trackfile = open("/Users/msteffen/Documents/School/Game Design/z-fight-club/code/tests/tracks/clover.trk","r")
comment = re.compile("/\*.*?\*/",re.DOTALL)
tdata = json.loads(comment.sub("",trackfile.read()))

#mesh = Blender.Mesh.New(name = "Line")
mesh = Blender.Mesh.New("Track")

for v in tdata["vertices"]:
	temp = v[2]
	v[2] = v[1]
	v[1] = temp

mesh.verts.extend(tdata["vertices"])
for s in tdata["sectors"]:
	for e in range(len(s["edges"])):
		mesh.edges.extend((s["edges"][e-1]["start"],
		                   s["edges"][e]["start"])
		                 )

#o = Blender.Object.New(type="Mesh",name="Line")
o = Blender.Object.New("Mesh","Track")

o.link(mesh)

sn = Blender.Scene.GetCurrent()
sn.link(o)

Blender.Redraw()
