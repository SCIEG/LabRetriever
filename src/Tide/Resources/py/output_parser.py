import csv, traceback, sys

def outputParse(data):
    try:
        m = {}
        k = None
        alleles = None
        csvr = csv.reader(data.splitlines())
        for row in csvr:
            if len(row) == 0 or (len(row) == 1 and len(row[0].strip()) == 0):
                continue
            if k is None and len(row) == 1:
                k = row[0].strip()
                continue
            if alleles is None and row[0] == 'Probabilities Ratios':
                alleles = dict(zip(range(1,len(row)), row[1:]))
                continue
            if k is not None and alleles is not None and len(row) == (len(alleles) + 1):
                d = {}
                for i in alleles.keys():
                    d[alleles[i].strip()] = row[i].strip()
                m[k] = d
                k = None
                alleles = None
    except Exception, e:
        return "".join(traceback.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
    return m


def writeOutput(file, data):
    f = open(file, 'w')
    for r in data:
        print >> f, ",".join(r)
    f.close()

window.writeOutput = writeOutput

window.outputParse = outputParse