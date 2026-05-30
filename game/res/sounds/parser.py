import sys
import json
import math

filename = sys.argv[1]
sceneName = sys.argv[2]
nodeData = list()
nodes = list()
idlist = dict()
idCounter : int = 0

class Node:
    __slots__ = ("id","parent","data")
    def __init__(self, id, parent, data):
        self.id = id
        self.data = data
        self.parent = parent
    def ToDict(self):
        ret = {
            "id" : self.id,
            "parent" : int(self.parent)
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

    if value.lower() == "true":
        return True
    if value.lower() == "false":
        return False

    if value.lower() == "null":
        return None

    try:
        return int(value)
    except ValueError:
        pass

    try:
        return float(value)
    except ValueError:
        pass
    
    return value

def GetHeader(line):
    objName : str
    objParent : str = "0"
    for pair in line.split():
        separated = pair.split('=')
        if separated[0] == "name":
            objName = separated[1].strip('"')
        elif separated[0] == "parent":
            objParent = separated[1].strip('"')
            objParent = objParent[:len(objParent)-2]
    return objName, objParent

def GetMeta(line):
    separated = line.split('=')
    metaName = separated[0].split('/')[1]
    metaName = metaName.strip(' ')
    rawValue = separated[1].strip(' "\n')
    metaValue = ConvertType(rawValue)
    return metaName, metaValue
        
def ParseNode():
    global idCounter
    nName, nParent = GetHeader(nodeData.pop(0))
    nData = dict()
    for line in nodeData:
        if line.find("metadata") >= 0:
            n, v = GetMeta(line)
            nData.update({n : v})
        elif line.find("transform") >= 0:
            n, v = GetTransform(line)
            nData.update({n : v})
    if nParent == "0":
        idlist.update({nName : 0})
    elif nParent == '.':
        idlist.update({nName : idCounter})
        nParent = "0"
    else:
        idlist.update({nName : idCounter})
    for i in idlist:
        if nParent == i:
            nParent = idlist[i]
    idCounter+=1
    nodes.append(Node(nName,nParent,nData))

firstNode = False   

with open(filename, "r") as gdScene:
    while True:
        line = gdScene.readline()
        if line == "\n":
            continue
        if line.find("node") >=0:
            if firstNode:
                ParseNode()
            nodeData.clear()
            nodeData.append(line)
            firstNode = True
        elif firstNode:
            nodeData.append(line)
        if not line: 
            ParseNode()
            break

jsonDict = {
    "name" : sceneName
}

for n in nodes:
    nDict = n.ToDict()
    nId = nDict.pop("id")
    nId = idlist[nId]
    if nId == 0:
        nId+=1
        nDict["class"] = "Node"
        nDict["process"] = True
        nDict["input"] = False
        nDict["physics"] = False
    else:
        nId+=1
        nDict["parent"]+=1
    jsonDict.update({str(nId) : nDict})

with open(sceneName+".json","w") as parsed:
    json.dump(jsonDict,parsed,indent=4)
