vertices = 0
normals = 0
textures = 0
faces = 0

with open("phantom.obj") as f:
    content = f.readlines()

for item in content:
	if item.startswith('v '):
		vertices += 1
	elif item.startswith('vt'):
		textures += 1
	elif item.startswith('vn'):
		normals += 1
	elif item.startswith('f '):
		faces += 1

print "vertices " + str(vertices) + "\n"
print "normals " + str(normals) + "\n"
print "textures " + str(textures) + "\n"
print "faces " + str(faces) + "\n"


