import csv
import os
import string

VALID_LOCII = ["D8", "D21", "D7", "CSF", "D3", "TH0", "D13", "D16", "D2", "D19", "VWA", "TPO", "D18", "D5", "FGA"]


def locusName(name):
    if string.lower(name).startswith("vwa"):
        return "vWA"
    if name.startswith('D'):
        return name.split("S")[0]
    if name.startswith('CSF'):
        return 'CSF'
    if name.startswith('TPO'):
        return 'TPO'
    if name.startswith('TH0') or name.startswith('THO'):
        return 'TH0'

    return name


def getKeys(csv):
    keys = {}
    idx = -1

    for i in csv.next():
        idx += 1
        i = i.lower()
        if "file" in i:
            keys['file'] = idx
        elif "sample" in i:
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

    if file.endswith('.csv'):
        file = file[0:-4]

    file = os.path.basename(file)
    keys = getKeys(csvr)

    if not keys:
        return None

    data = {}
    ordered = []

    for r in csvr:
        locus = locusName(r[keys['locus']].strip().upper())

        if len(locus) == 0:
            continue

        als = []

        for x in keys['call']:
            if x >= len(r):
                continue

            call = r[x].strip()

            if len(call):
                als.append(call)
            else:
                break

        name = r[keys['name']].strip()

        if keys.has_key('file') and len(r) > keys['file']:
            name = r[keys['file']] + " | " + name
        else:
            name = file + " | " + name

        data.setdefault(name, {})[locus] = als

        if len(ordered) == 0 or ordered[-1] != name:
            ordered.append(name)

    fileData = []
    for keyName in ordered:
        loci_names = []
        d = data.get(keyName)
        for locus in d:
            if not locus in loci_names:
                loci_names.append(locus)
        d['name'] = keyName
        d['loci'] = loci_names
        fileData.append(d)
    return fileData

#if __name__ == "__main__":
#    path = "/Users/crob/Desktop/removed_loci_test.csv"
#    print load(path)

window.load = load
