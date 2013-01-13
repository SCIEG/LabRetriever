# R commands to compute LR for for Identifiler for a single source, or two or three person mix. This will work with all R functions for LR's with dropout. This has been successfuly run with...

# LR1unk_KEL_nosamp
# LR2unk_KEL_fix_nosamp_fixIBD.R
# LR3unk_KEL_fix_nosamp_noIBD.R

#R code by David Balding, updated and modified by Kirk Lohmueller

#Need to load the R function (either "LR3unk_KEL_fix_nosamp_noIBD"...) first.  THen copy and paste this stuff into R to call the functions.  Note, need to manually input the alleles and change the paths to the allele freq files.

#Note, all individuals are assumed to be unrelated here.

#The analyst can use any allele frequency table, although they need to be in a specific configuration expected by this program. Here we use John Butler's allele frequencies in tables; the user will have to insert the correct path to each allele table per locus. 

# This version allows for the possibility that a stutter peak recorded in the epg could be masking a minor component allele; allowing for masking by stutter tends to reduce the LR, since it allows more possibilities for alternative contributors of the minor component.

# The variable "callXX" for locus XX is a list of replicate calls, each of length 1 here as there are no replicates.  The element of this outer list is a list of length two, corresponding to the alleles of [[1]] crime scene profile (csp) alleles [[2]] masking alleles, due either directly to a major/assumed donor or a stutter from one of these alleles.  Old Comment: the first list has the minor alleles that are not from an "assumed contributor," and can be any number of alleles, dependent on the R functions for which it will serve as input data.  Any alleles that 1) ARE from an assumed contributor, OR were 2) in a stutter position, AND 3) above the detection threshold, AND 4) below the stutter threshold would go where the word "NULL" is.

# This datafile will produce a table of the per-locus numerator, denominator, and LR. It will also provide a row with the totals for the entire profile    KPI modification 12/12/12 (that magic date!)

# Casename: Wood

# Item #: Penile Swab EC

# LR # of unknown program: 1Unk

dg = c(0.2,0.3) #Put the DO probs here
a = 0.5     # set value between 0 and 1
cc = 0.01	# set value between 0 and 1 #this is Din
rel = c(0,0)  # relatedness: probability that alternate possible contributor has 1 and 2 alleles ibd with suspected contributor; kept at (0,0) for all analyses here

race = 1 # 1 = Afr-Am; 3 = Cauc; 5 = Hisp;

headers<-cbind("P(DO)","Locus","Numerator","Denominator","LR") #Create a table with these headers

d8 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D8_B.count.csv",head=T,sep=",")
nd8 = d8[,race]			# allele counts
names(nd8) = d8[,2]		# allele names
ssd8 = c("11","13")		# genotye of suspected contributor of minor component
calld8 = list(list(c("13"),c("11","12"))) # the first list has the minor alleles that are not from an "assumed contributor," and can be any number of alleles, dependent on the R functions for which it will serve as input data.  Any alleles that 1) ARE from an assumed contributor, OR were 2) in a stutter position, AND 3) above the detection threshold, AND 4) below the stutter threshold would go where the word "NULL" is.
all_checkd8<-c(unlist(calld8),ssd8)
nd8<-rare_adjust(all_checkd8,nd8)
nmr = LRnumer(ssd8,calld8,nd8,dg,a,cc)
dnm = (nmr*rel[2]+LRdenom(ssd8,calld8,rel,nd8,dg,a,cc))
LRd8 = nmr/dnm
mx = max(LRd8)
d8_LRs<-cbind(dg,"D8",nmr,dnm,LRd8) #write the per-locus LRs to a file
totnmr=nmr
totdnm=dnm

d21 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D21_B.count.csv",head=T,sep=",")
nd21 = d21[,race]			# allele counts
names(nd21) = d21[,2]		# allele names
ssd21 = c("30","32.2")
calld21 = list(list(c(NULL),c("30")))
all_checkd21<-c(unlist(calld21),ssd21)
nd21<-rare_adjust(all_checkd21,nd21)
nmr = LRnumer(ssd21,calld21,nd21,dg,a,cc)
dnm =(nmr*rel[2]+LRdenom(ssd21,calld21,rel,nd21,dg,a,cc))
LRd21 = nmr/dnm
mx = max(mx,LRd21)
d21_LRs<-cbind(dg,"D21",nmr,dnm,LRd21)
totnmr=totnmr*nmr
totdnm=totdnm*dnm



totLR<-LRd8*LRd21

#write the per-locus LRs to a file:
locus_LRs<-rbind(headers,d8_LRs,d21_LRs)
sort(locus_LRs,method="quick")
locus_totals<-cbind(dg,"Totals",totnmr,totdnm,totLR)

write.table(locus_LRs,"/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Lab Retriever/Lab Retriever testing/Test 1 2 loci/Test 2 Two PDOs/two loci 1Unk Identifiler 2PDOs.txt", sep="\t",col.names=FALSE, row.names=FALSE, append=TRUE)

write.table(locus_totals,"/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Lab Retriever/Lab Retriever testing/Test 1 2 loci/Test 2 Two PDOs/two loci 1Unk Identifiler 2PDOs.txt", sep="\t",col.names=FALSE, row.names=FALSE, append=TRUE)
