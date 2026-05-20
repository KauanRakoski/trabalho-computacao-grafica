import os, math
cwd = os.path.dirname(os.path.abspath(__file__))
objpath = os.path.join(cwd, 'data', 'map', 'Once Upon A Tire.obj')
pathpath = os.path.join(cwd, 'data', 'crash_path.txt')

verts=[]
tri_normals=[]
tri_verts=[]
with open(objpath, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        if line.startswith('v '):
            parts=line.split()
            if len(parts) >= 4:
                verts.append((float(parts[1]), float(parts[2]), float(parts[3])))
        elif line.startswith('f '):
            parts=line.split()[1:]
            idxs=[]
            for p in parts:
                if '/' in p:
                    idxs.append(int(p.split('/')[0]) - 1)
                else:
                    idxs.append(int(p)-1)
            if len(idxs) == 3:
                v0, v1, v2 = verts[idxs[0]], verts[idxs[1]], verts[idxs[2]]
                edge1 = (v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2])
                edge2 = (v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2])
                nx = edge1[1]*edge2[2] - edge1[2]*edge2[1]
                ny = edge1[2]*edge2[0] - edge1[0]*edge2[2]
                nz = edge1[0]*edge2[1] - edge1[1]*edge2[0]
                norm = math.sqrt(nx*nx + ny*ny + nz*nz)
                tri_normals.append(abs(ny)/norm if norm > 1e-9 else 1.0)
                tri_verts.append((v0, v1, v2))

points=[]
with open(pathpath, 'r', encoding='utf-8', errors='ignore') as f:
    for line in f:
        parts = line.split()
        if len(parts) >= 3:
            points.append((float(parts[0]), float(parts[2])))

def sign(p1, p2, p3):
    return (p1[0]-p3[0])*(p2[1]-p3[1]) - (p2[0]-p3[0])*(p1[1]-p3[1])

def inside(px, pz, a, b, c):
    d1 = sign((px,pz), (a[0], a[2]), (b[0], b[2]))
    d2 = sign((px,pz), (b[0], b[2]), (c[0], c[2]))
    d3 = sign((px,pz), (c[0], c[2]), (a[0], a[2]))
    has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
    has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)
    return not (has_neg and has_pos)

counts = {}
covered = 0
for i,(a,b,c) in enumerate(tri_verts):
    minx = min(a[0], b[0], c[0]); maxx = max(a[0], b[0], c[0])
    minz = min(a[2], b[2], c[2]); maxz = max(a[2], b[2], c[2])
    if maxx - minx < 1e-6 or maxz - minz < 1e-6:
        continue
    for px,pz in points[::10]:
        if px < minx or px > maxx or pz < minz or pz > maxz:
            continue
        if inside(px, pz, a, b, c):
            counts[int(tri_normals[i]*100)] = counts.get(int(tri_normals[i]*100),0)+1
            covered += 1
            break

print('path-covered triangles:', covered)
for k in sorted(counts):
    print(f'{k/100:.2f}: {counts[k]}')
