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

# Casename:

# Item #:

# LR # of unknown program: 

dg = c(0.05) #Put the DO probs here
a = 0.5     # set value between 0 and 1
cc = 0.01	# set value between 0 and 1 #this is Din
rel = c(0,0)  # relatedness: probability that alternate possible contributor has 1 and 2 alleles ibd with suspected contributor; kept at (0,0) for all analyses here

race = 1 # 1 = Afr-Am; 3 = Cauc; 5 = Hisp;

csf = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/CSF_B.count.csv",head=T,sep=",")
ncsf = csf[,race]			# allele counts
names(ncsf) = csf[,2]		# allele names
sscsf = c("9","12")
callcsf = list(list(c("9","12"),c("10","11"))) 
all_checkcsf<-c(unlist(callcsf),sscsf)
ncsf<-rare_adjust(all_checkcsf,ncsf) 
nmr = LRnumer(sscsf,callcsf,ncsf,dg,a,cc)
LRcsf = nmr/(nmr*rel[2]+LRdenom(sscsf,callcsf,rel,ncsf,dg,a,cc))
mx = max(LRcsf)

d2 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D2_B.count.csv",head=T,sep=",")
nd2 = d2[,race]
names(nd2) = d2[,2]		# allele names
ssd2 = c("18","19")
calld2 = list(list(c("19"),c("22","23"))) 
all_checkd2<-c(unlist(calld2),ssd2)
nd2<-rare_adjust(all_checkd2,nd2)
nmr = LRnumer(ssd2,calld2,nd2,dg,a,cc)
LRd2 = nmr/(nmr*rel[2]+LRdenom(ssd2,calld2,rel,nd2,dg,a,cc))
mx = max(mx,LRd2)

totLR<-LRcsf*LRd2

#write the per-locus LRs to a file:
locus_LRs<-rbind(dg,LRcsf,LRd2,totLR)

#write output file:
write.table(locus_LRs,"/Users/keithMacPro/Documents/Windows shared folder/Window application files/Lab Retriever Testing/Test 1 2 loci/Test_1_R_result.txt",append=TRUE,quote=FALSE,sep="\t",col.names=FALSE)

date()
