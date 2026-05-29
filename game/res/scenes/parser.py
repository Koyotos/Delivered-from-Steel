import sys
import json
import math
import os
import re

filename = sys.argv[1]
sceneName = sys.argv[2]
nodeData = list()
nodes = list()
idlist = dict()
idCounter : int = 0
ext_resources = dict()

class Node:
    __slots__ = ("id","parent","data")
    def __init__(self, id, parent, data):
        self.id = id
        self.data = data
        self.parent = parent
        
    def ToDict(self):
        p_id = 0
        try:
            p_id = int(self.parent)
        except ValueError:
            p_id = 0

        ret = {
            "id" : self.id,
            "parent" : p_id
        }
        ret.update(self.data)
        return ret

def GetTransform(line):
    valStartIdx = line.find('(')
    valList = line[(valStartIdx+1):]
    valList = valList[:(len(valList)-2)]
    converted = list()
    for value in valList.split(','):
        converted.append(float(value))
    px,py,pz = converted[9], converted[10], converted[11]
    sx = math.sqrt(converted[0]**2 + converted[1]**2 + converted[2]**2)
    sy = math.sqrt(converted[3]**2 + converted[4]**2 + converted[5]**2)
    sz = math.sqrt(converted[6]**2 + converted[7]**2 + converted[8]**2)
    r00 = converted[0] / sx
    r01 = converted[1] / sx
    r02 = converted[2] / sx
    r10 = converted[3] / sy
    r11 = converted[4] / sy
    r12 = converted[5] / sy
    r20 = converted[6] / sz
    r21 = converted[7] / sz
    r22 = converted[8] / sz
    ry = math.asin(-r20)
    if abs(r20) != 1:
        rx = math.atan2(r21, r22)
        rz = math.atan2(r10, r00)
    else:
        rx = 0
        rz = math.atan2(-r01, r11)

    return "transform",[px, py, pz, rx, ry, rz, sx, sy, sz]

def ConvertType(value: str):
    value = value.strip()
    if value.lower() == "true": return True
    if value.lower() == "false": return False
    if value.lower() == "null": return None
    
    try: return int(value)
    except ValueError: pass
    
    try: return float(value)
    except ValueError: pass
    
    if value.startswith("[") and value.endswith("]"):
        try: return json.loads(value)
        except ValueError: pass
    
    return value

def GetHeader(line):
    objName = ""
    objParent = "."
    instanceId = ""
    
    name_match = re.search(r'name="([^"]+)"', line)
    if name_match: objName = name_match.group(1)
    
    parent_match = re.search(r'parent="([^"]+)"', line)
    if parent_match: objParent = parent_match.group(1)
        
    instance_match = re.search(r'instance=ExtResource\("([^"]+)"\)', line)
    if instance_match: instanceId = instance_match.group(1)
        
    return objName, objParent, instanceId

def GetMeta(line):
    separated = line.split('=', 1)
    metaName = separated[0].split('/')[1].strip()
    rawValue = separated[1].strip(' "\n')
    metaValue = ConvertType(rawValue)
    return metaName, metaValue

def resolve_godot_path(res_path):
    clean_path = res_path.replace("res://", "").replace("/", os.sep)
    current_dir = os.path.dirname(os.path.abspath(filename))
    
    project_root = current_dir
    while True:
        if os.path.exists(os.path.join(project_root, "project.godot")):
            full_path = os.path.join(project_root, clean_path)
            if os.path.exists(full_path): return full_path
        parent = os.path.dirname(project_root)
        if parent == project_root:
            break
        project_root = parent
        
    fallback_path = os.path.join(current_dir, clean_path)
    if os.path.exists(fallback_path): return fallback_path
    if os.path.exists(clean_path): return clean_path
    return None

def ParseNode():
    global idCounter
    header = nodeData.pop(0)
    nName, nParent, nInstance = GetHeader(header)
    nData = dict()
    
    if nInstance:
        if nInstance in ext_resources:
            res_path = ext_resources[nInstance]
            prefab_path = resolve_godot_path(res_path)
            
            if prefab_path:
                try:
                    meta_count = 0
                    with open(prefab_path, "r", encoding="utf-8") as pf:
                        for pf_line in pf:
                            if pf_line.startswith("[node "): 
                                if meta_count > 0: break 
                            elif pf_line.find("metadata/") >= 0:
                                n, v = GetMeta(pf_line)
                                nData[n] = v
                                meta_count += 1
                    if meta_count == 0:
                        nData["DEBUG_ERROR"] = f"File {prefab_path} has no metadata."
                except Exception as e:
                    nData["DEBUG_ERROR"] = f"Error reading: {str(e)}"
            else:
                nData["DEBUG_ERROR"] = f"Cant find: {res_path}"
        else:
            nData["DEBUG_ERROR"] = f"No ID {nInstance} in file"

    for line in nodeData:
        if line.find("metadata/") >= 0:
            n, v = GetMeta(line)
            nData[n] = v
        elif line.find("transform") >= 0:
            n, v = GetTransform(line)
            nData[n] = v
            
    if nParent == "0" or nParent == "":
        idlist.update({nName : 0})
    elif nParent == '.':
        idlist.update({nName : idCounter})
        nParent = "0"
    else:
        idlist.update({nName : idCounter})
        
    for i in idlist:
        if nParent == i:
            nParent = idlist[i]
            
    idCounter += 1
    nodes.append(Node(nName, nParent, nData))

firstNode = False   

with open(filename, "r", encoding="utf-8") as gdScene:
    while True:
        line = gdScene.readline()
        if not line:
            if firstNode:
                ParseNode()
            break
        if line == "\n":
            continue
            
        if line.startswith("[ext_resource"):
            path_match = re.search(r' path="([^"]+)"', line)
            id_match = re.search(r' id="([^"]+)"', line)
            if path_match and id_match:
                ext_resources[id_match.group(1)] = path_match.group(1)
            
        if line.startswith("[node "):
            if firstNode:
                ParseNode()
            nodeData.clear()
            nodeData.append(line)
            firstNode = True
        elif firstNode:
            nodeData.append(line)

jsonDict = {
    "name" : sceneName
}

for n in nodes:
    nDict = n.ToDict()
    nId = nDict.pop("id")
    nId = idlist.get(nId, nId)
    
    if nId == 0:
        nId += 1
        nDict["class"] = "Node"
        nDict["process"] = True
        nDict["input"] = False
        nDict["physics"] = False
    else:
        nId += 1
        nDict["parent"] += 1
        
    if nDict.get("class") == "Light" and "transform" in nDict:
        del nDict["transform"]
        
    jsonDict.update({str(nId) : nDict})

with open(sceneName+".json", "w", encoding="utf-8") as parsed:
    json.dump(jsonDict, parsed, indent=4)