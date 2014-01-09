GeneMapperOutputParser

This is a program that reads the output from the program GeneMapper by David
Balding, and generates an input file for LabRetriever. It requires that the
output from the GeneMapper program contains Sample Data/File, Marker, and
Allele X, where X is a number. The Sample Data/File should be unique per
instance. Currently, the program only handles alleles found in
"data/LocusAbbreviationMappings.csv".

This program takes advantage of the fact that the assumed alleles and suspect
alleles will tend to be the same values over and over again, but the data
from the samples will be different. Thus, the user need only to input the
assumed and suspect alleles once, and the unattributed alleles will be
calculated from the GeneMapper data.

To use the program, use "Browse" to load a GeneMapper output file and click
"Load". Then, you can either fill out the assumed/suspect alleles, or
add/remove samples to include in the LabRetriever input file. It does not matter
in which order you do this in.
 - To fill out the assumed or suspect alleles, type
   in the allele in the box, separated by spaces and/or commas. The "detected"
   alleles is for reference--it shows the alleles read from the GeneMapper file.
 - To add/remove samples, double-click on the sample name on the left, and the
   sample will move from "Samples to export" to "Samples to exclude" and vice
   versa.
Clicking on a sample will show what alleles have been detected in the sample.
Leaving cells blank is okay--it will be created in the output file, but it will
not be used, as long as the suspect alleles data is empty. (If there's no
suspect data, then the sample there is useless.) When you are done, you can
create an output file (which is the input file for LabRetriever) by typing an
output file name, and clicking "Export".

LabRetriever

The input file is a CSV (Comma Separated Value) file where there are no empty
lines in between data, and each line is either
  - alpha,DECIMAL
  - Drop-in rate,DECIMAL
  - Drop-out rate,DECIMAL
  - IBD Probs,DECIMAL,DECIMAL,DECIMAL
  - Race,RACE
  - XXX_Assumed,(ALLELE,)*
  - XXX_Suspected,(ALLELE,)*
  - XXX_Unattributed,(ALLELE,)*
where, DECIMAL is a number between 0 and 1, RACE is either AFRICAN_AMERICAN,
HISPANIC, or CAUCASIAN, and ALLELE is an allele for locus XXX. Here's what
each input means:
  - alpha,DECIMAL
        Controls the probability of drop out for a homozygote allele. This
        number is best kept at the default number, unless you know what you are
        doing (see Balding's paper).
        Default value: 0.5
  - Drop-in rate,DECIMAL
        The probability that an allele appears in a sample, either by lab error
        or by an unknown person.
        Default value: 0.01
  - Drop-out rate,DECIMAL
        The probability that an allele is not present in a sample when it should
        have been there.
        Default value: 0.05
  - IBD Probs,DECIMAL,DECIMAL,DECIMAL
        These numbers control the probabilities that an unknown person share
        alleles with the suspect by descent. (IBD = identical by descent). These
        numbers should sum to 1. The first number corresponds to the probability
        that the person shares 0 alleles in common; the second number to 1
        allele in common; the third, 2 alleles. A complete stranger would have
        a distribution of 1,0,0. A sibling would have 0.25,.5,.25.
        Default value:1,0,0
  - Race,RACE
        The race of the person. This controls the allele population
        distribution, which is used to calculate the probability that a given
        person has an allele.
        Default value: AFRICAN_AMERICAN
  - XXX-Assumed,(ALLELE,)*
    XXX-Suspected,(ALLELE,)*
    XXX-Unattributed,(ALLELE,)*
        The alleles from locus XXX. If XXX_Suspected is empty, then the program
        will not compute anything for that allele.
Each line header must be unique except for XXX_Unattributed, which there can be
as many as you want. If a line is not specified, then it gets the default value,
expect for the locus (XXX) lines. All lines must be specified to include it in
the test. For example, if for locus D2, there are no assumed alleles, but the
suspect alleles are 9 and 10, and the unattributed alleles are 10 and 11, the
following lines must appear in the input file:
  D2-Assumed
  D2-Suspected,9,10
  D2-Unattributed,10,11
Order of the alleles do not matter. Alleles can have multiplicity (repeated),
except for assumed alleles.

Currently the only way to use LabRetriever is to use the GUI. The GUI can either
load an input file, or data can be typed directly in. If a file is loaded, the
data will be imported to the GUI. Parameters can be editted on the left. Also,
checkboxes of sample situations to run on can be selected. Any number of them
can be selected, and they will be calculated when "(Save and) Go!" is pressed.
The table on the right is how the allele data imported. There are four columns,
locus, assumed alleles, suspected alleles and unattributed alleles. For each
locus to test for, enter the locus name under "Locus," and fill out the
alleles, which can be delimited by spaces and/or commas. Additional replicate
data can be entered by putting the unattributed data underneath the first line,
and leaving all other cells in that row blank. This can be repeated multiple
times. Here is an example where the table data and the import data is the same:

    Locus | Assumed Alleles | Suspect Alleles | Unattributed Alleles
   -------|-----------------|-----------------|----------------------
     D16  | 10, 11          | 11 12           | 12,13
          |                 |                 | 12
          |                 |                 | 13
     D17  | 9               | 10 10           | 10
          |                 |                 | 11

	Input File:
	...
    D16-Assumed,10,11
	D16-Suspected,11,12
	D16-Unattributed,12,13
	D16-Unattributed,12
	D16-Unattributed,13
    D17-Assumed,9
	D17-Suspected,10,10
	D17-Unattributed,10
	D17-Unattributed,11
	
Select an output file and click "(Save and) Go!" to run the program. The data
entered will be autosaved in the "autosave" folder with the current day and
time. The program will then "seem" to hang, but it is running. (In the future,
there may be a progress bar to show progress). Eventually, a pop up will come up
to notify that the process is done. The output file will have data of the log
and regular probabilities of each situation, along with the ratios between every
two situation, in csv format.