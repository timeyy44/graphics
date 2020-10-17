//
//  tools2.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/10/9.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef tools2_h
#define tools2_h

#include <fstream>
#include <string>
#include <vector>

using namespace std;
//using namespace glm;

float minX = 9999.f;
float minY = 9999.f;
float minZ = 9999.f;
float maxX = -9999.f;
float maxY = -9999.f;
float maxZ = -9999.f;
float maxLen;

struct ATOM {
    string atom;
    int serial;
    string name;
    string dis;
    string fzbs;
    string altLos;
    string res;
    string chainID;
    int resSeq;
    string iCode;
    vec3 pos;
    float occ;
    float temp;
    string segID;
    string ele;
    string charge;
};

void setMinMax(vec3 pos) {
    if (minX > pos.x())
        minX = pos.x();
    if (maxX < pos.x())
        maxX = pos.x();
    if (minY > pos.y())
        minY = pos.y();
    if (maxY < pos.y())
        maxY = pos.y();
    if (minZ > pos.z())
        minZ = pos.z();
    if (maxZ < pos.z())
        maxZ = pos.z();
}

vec3 getBasic() {
    return vec3((minX+maxX)/2.f, (minY+maxY)/2.f, (minZ+maxZ)/2.f);
}

float getMaxLen() {
    return (maxX < maxY ? maxY : maxX)+5.f;
}

bool phraseA(ifstream& file, vector<vector<ATOM>*> *atomss) {
    string p;
    struct ATOM atom;
    vector<ATOM> *atoms = new vector<ATOM>;
    atomss->push_back(atoms);
    vec3 pos;
    while(getline(file, p)) {
        if (p.substr(0, 3) == "TER") {
            atoms = new vector<ATOM>;
            atomss->push_back(atoms);
            continue;
        }
        
        if (p.substr(0, 4) != "ATOM") {
            if (p.substr(0, 6) != "HETATM")
                continue;
            else
                atom.atom = "HETATM";
        } else
            atom.atom = "ATOM";
        
        atom.serial = stoi(p.substr(6, 5));
        atom.name = p.substr(12, 2);
        atom.dis = p.substr(14, 1);
        /*if (p.substr(15, 1) == " ")
            atom.fzbs = -1;
        else
            atom.fzbs = stoi(p.substr(15, 1));*/
        atom.fzbs = p.substr(15, 1);
        atom.altLos = p.substr(16, 1);
        atom.res = p.substr(17, 3);
        atom.chainID = p.substr(21, 1);
        atom.resSeq = stoi(p.substr(22, 4));
        atom.iCode = p.substr(26, 1);
        
        pos = vec3(stof(p.substr(30, 8)),
                   stof(p.substr(38, 8)),
                   stof(p.substr(46, 8)));
        
        setMinMax(pos);
        
        atom.pos = pos;// - basic;
        
        atom.occ = stof(p.substr(54, 6));
        atom.temp = stof(p.substr(60, 6));
        
        atom.segID = p.substr(72, 4);
        atom.ele = p.substr(76, 2);
        atom.charge = p.substr(78, 2);
        
        atomss->back()->push_back(atom);
    }
    
    return true;
}

vec3 getColor(string name) {
    if (name == " C")
        return vec3(0.2f, 0.2f, 0.2f);
    else if (name == " H")
        return vec3(0.8f, 0.8f, 0.8f);
    else if (name == " O")
        return vec3(0.8f, 0.4f, 0.3f);
    else if (name == " N")
        return vec3(0.3f, 0.4f, 0.8f);
    else
        return vec3(0.3f, 0.6f, 0.3f);
}

float getRadius(string name) {
    if (name == " C")
        return 1.2f;
    else if (name == " H")
        return 0.1f;
    else if (name == " O")
        return 1.6f;
    else if (name == " N")
        return 1.4f;
    else
        return 1.f;
}

#endif /* tools2_h */
