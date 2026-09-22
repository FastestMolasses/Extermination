#!/usr/bin/env python3
"""Recover the opening's original 176-byte morph vertices and seven deltas.

001CAA00 draws the body with bone actor+94 collapsed (001C7420), then
001CB3C0 uploads that bone's full matrix and eight face weights. Kernel
0023C4B0 reads 32 vertices per packet, accumulates seven position deltas,
and retains the original normals. This exporter keeps exact attribute and
delta bytes in its weld key. It exports geometry only, not an invented RNG
timeline. Outputs contain original assets and must remain ignored locally.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import struct
from pathlib import Path
import export_native as native
import export_opening_actors as actors

FACES=(('player','chunk03/f16_id18.bin',0,0x11749C0,0x8102B0,0x18),
       ('roger','chunk15/f18_id94.bin',0x86000,0x18C8740,0x7A96E0,0x88))

def records(raw):
    blocks,qwc,nodes,size=struct.unpack_from('<4I',raw)
    if nodes!=1 or struct.unpack_from('<I',raw,0x34)[0]!=7:
        raise ValueError('not a seven-shape original face model')
    if size!=64+blocks*0x1630 or qwc!=blocks*355 or len(raw)!=size:
        raise ValueError('face block dimensions disagree')
    result=[]
    for b in range(blocks):
        off=64+b*0x1630
        if struct.unpack_from('<2I',raw,off+8)!=(0x01000404,0x6C008000):
            raise ValueError('unexpected face first VIF UNPACK')
        if struct.unpack_from('<2I',raw,off+0x1018)!=(0x01000404,0x6C608100):
            raise ValueError('unexpected face continuation VIF UNPACK')
        data=raw[off+16:off+0x1010]+raw[off+0x1020:off+0x1620]
        result.append([data[i*176:(i+1)*176] for i in range(32)])
    return result

def mesh(raw):
    vertices={};textures=[];texture_ids={};deltas=[]
    pos=[];normals=[];uv=[];tex=[];indices=[]
    for block in records(raw):
        run=[]
        for v in block:
            word=struct.unpack_from('<I',v,60)[0]
            if word&0x3FF:raise ValueError('face references a nonzero VU matrix slot')
            texkey=struct.unpack_from('<Q',v)[0]&native.TEX0_KEY_MASK
            if texkey not in texture_ids:
                texture_ids[texkey]=len(textures)
                fields=native.tex0_fields(texkey);fields['key']=texkey
                if fields['psm']!=0x14:raise ValueError('face texture is not PSMT4')
                textures.append(fields)
            tid=texture_ids[texkey]
            key=(v[48:60],v[32:44],v[16:24],tid,v[64:176])
            if key not in vertices:
                vertices[key]=len(pos)
                pos.append(struct.unpack('<3f',key[0]));normals.append(struct.unpack('<3f',key[1]))
                uv.append(struct.unpack('<2f',key[2]));tex.append(tid)
                deltas.append(tuple(x for i in range(7) for x in struct.unpack_from('<3f',v,64+16*i)))
            run.append(vertices[key])
            if len(run)>=3 and not word&0x8000:
                a,b,c=run[-3:]
                if len({a,b,c})==3:
                    indices.extend((a,b,c) if len(run)%2==0 else (b,a,c))
    return [(pos,normals,indices,[7]*len(pos),uv,tex)],textures,deltas

def suppression_proof(body):
    sections,_=actors.exact_mesh_sections(body)
    counts=[0,0,0,0]
    for positions,normals,indices,bones,uv,tex in sections:
        for i in range(0,len(indices),3):
            counts[sum(bones[v]==7 for v in indices[i:i+3])]+=1
    if counts[1] or counts[2]:
        raise ValueError('mixed head/body triangle requires original collapsed-bone path')
    return {'head_triangles':counts[3],'mixed_triangles':counts[1]+counts[2]}

def main():
    root=Path(__file__).resolve().parents[1]
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--extract',type=Path,default=root/'extract')
    p.add_argument('--gs',type=Path,required=True)
    p.add_argument('--reference-ee',type=Path,required=True)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('--report',type=Path,required=True)
    args=p.parse_args();ram=args.reference_ee.read_bytes();report={}
    args.out.mkdir(parents=True,exist_ok=True)
    for name,source,offset,address,actor,resource in FACES:
        data=(args.extract/source).read_bytes()
        size=struct.unpack_from('<I',data,offset+12)[0];raw=data[offset:offset+size]
        if raw!=ram[address:address+size]:raise ValueError(name+': original RAM model bytes differ')
        if struct.unpack_from('<I',ram,0x28A490+resource*4)[0]!=address:
            raise ValueError(name+': original resource table binding differs')
        block=struct.unpack_from('<I',ram,actor+0x90)[0]
        if struct.unpack_from('<h',ram,actor+0x94)[0]!=7 or struct.unpack_from('<I',ram,block+0x60)[0]!=address:
            raise ValueError(name+': original face binding differs')
        sections,textures,deltas=mesh(raw)
        entries,texels=native.build_texture_blob(None,textures,p2s=args.gs)
        out=args.out/(name+'_face.emdl')
        native.write_emdl(out,sections,[],[-1]*21,[[native.mat_identity()]*21],60.0,entries,texels)
        morph=args.out/(name+'_face.emfm')
        payload=struct.pack('<4s4I',b'EMFM',1,len(deltas),7,7)
        payload+=b''.join(struct.pack('<21f',*v) for v in deltas)
        morph.write_bytes(payload)
        _,bodyfile,bodyoff,_,_,_=next(a for a in actors.ACTORS if a[0]==name)
        body=(args.extract/bodyfile).read_bytes()[bodyoff:]
        report[name]={'source':source,'offset':offset,'size':size,'resource':resource,
                      'reference_address':address,'actor_address':actor,'bone':7,
                      'source_sha256':hashlib.sha256(raw).hexdigest(),
                      'vertices':len(deltas),'triangles':len(sections[0][2])//3,
                      'textures':len(textures),'body_suppression':suppression_proof(body),
                      'snapshot_weights':struct.unpack_from('<8f',ram,block+0x40),
                      'mesh_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),
                      'morph_sha256':hashlib.sha256(payload).hexdigest()}
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report,indent=2))

if __name__=='__main__':main()
