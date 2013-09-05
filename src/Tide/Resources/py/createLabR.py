
def createLabR():
    """ format of csv file expected:
    alpha,DECIMAL
    Drop-in rate,DECIMAL
    Drop-out rate,DECIMAL
    IBD Probs,DECIMAL,DECIMAL,DECIMAL
    Race,RACE
    XXX_Assumed,(ALLELE,)*
    XXX_Suspected,(ALLELE,)*
    XXX_Unattributed,(ALLELE,)*
    """
    out = open(Ti.Filesystem.getFile(Ti.Filesystem.getApplicationDataDirectory(), "output.csv").nativePath(), "w")
    print >> out, "alpha,%d"
    print >> out, "Drop-in rate,"
    print >> out, "Drop-out rate,"
    print >> out, "IBD Probs,,,"
    print >> out, "Race,"
    for
    print >> out, "%s_Assumed,"
    print >> out, "%s_Suspected,"
    print >> out, "%s_Unattributed,"
