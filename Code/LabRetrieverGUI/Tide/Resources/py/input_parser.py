
import csv

VALID_LOCII = ["D8","D21","D7","CSF","D3","THO","D13","D16","D2","D19","vWA","TPO","D18","D5","FGA"]

def locusName(name):
    if (name.startswith('D')):
        return name.split("S")[0]
    if (name.startswith('CSF')):
        return 'CSF'
    if (name.startswith('TPO')):
        return 'TPO'
    return name

def getKeys(csv):
    keys = {}
    idx = -1
    for i in csv.next():
        idx += 1
        i = i.lower()
        if "sample" in i:
            keys['name'] = idx
        elif "locus" in i or 'marker' in i:
            keys['locus'] = idx
        elif "call" == i:
            keys.setdefault('call', []).append(idx)
        elif "rfu" == i:
            keys.setdefault('rfu', []).append(idx)
        elif keys.has_key('name') and "allele" in i:
            keys.setdefault('call', []).append(idx)
    if not keys:
        return getKeys(csv)
    return keys

def load(file):
    """
    reads a genemapper or <x> output csv file
    creates a list of maps for the files with the following information:
        {'name': <sample name>, '<LOCUS>':[<call value>, ... ]}
    """
    global VALID_LOCII
    csvr = csv.reader(open(file).read().splitlines())
    keys = getKeys(csvr)
    if not keys:
        return None
    data = {}
    ordered = []
    for r in csvr:
        locus = locusName(r[keys['locus']])
        if locus not in VALID_LOCII:
            continue
        als = []
        for x in keys['call']:
            if x >= len(r):
                continue
            call = r[x]
            if len(call):
                als.append(call)
            else:
                break

        data.setdefault(r[keys['name']], {})[locus] = als
        if len(ordered) == 0 or ordered[-1] != r[keys['name']]:
            ordered.append(r[keys['name']])
    fileData = []
    for keyName in ordered:
        d = data.get(keyName)
        d['name'] = keyName
        fileData.append(d)
    return fileData

window.load = load