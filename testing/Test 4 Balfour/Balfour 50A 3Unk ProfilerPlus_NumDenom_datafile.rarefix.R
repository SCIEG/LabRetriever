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

# Casename:Balfour

# Item #:50A

# LR # of unknown program: 3

dg = c(0.01) #Put the DO probs here
a = 0.5     # set value between 0 and 1
cc = 0.01	# set value between 0 and 1 #this is Din
rel = c(0,0)  # relatedness: probability that alternate possible contributor has 1 and 2 alleles ibd with suspected contributor; kept at (0,0) for all analyses here

race = 1 # 1 = Afr-Am; 3 = Cauc; 5 = Hisp;

headers<-cbind("Locus","Numerator","Denominator","LR") #Create a table with these headers

d8 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D8_B.count.csv",head=T,sep=",")
nd8 = d8[,race]			# allele counts
names(nd8) = d8[,2]		# allele names
ssd8 = c("13","15")		
calld8 = list(list(c("12","13","14","15","16"),c(NULL))) # the first list has the minor alleles that are not from an "assumed contributor," and can be any number of alleles, dependent on the R functions for which it will serve as input data.  Any alleles that 1) ARE from an assumed contributor, OR were 2) in a stutter position, AND 3) above the detection threshold, AND 4) below the stutter threshold would go where the word "NULL" is.
all_checkd8<-c(unlist(calld8),ssd8)
nd8<-rare_adjust(all_checkd8,nd8)
nmr = LRnumer(ssd8,calld8,nd8,dg,a,cc)
dnm = (nmr*rel[2]+LRdenom(ssd8,calld8,rel,nd8,dg,a,cc))
LRd8 = nmr/dnm
mx = max(LRd8)
d8_LRs<-cbind("D8",nmr,dnm,LRd8) #write the per-locus LRs to a file
totnmr=nmr
totdnm=dnm

d21 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D21_B.count.csv",head=T,sep=",")
nd21 = d21[,race]			# allele counts
names(nd21) = d21[,2]		# allele names
ssd21 = c("35","36")
calld21 = list(list(c("35","36"),c(NULL)))
all_checkd21<-c(unlist(calld21),ssd21)
nd21<-rare_adjust(all_checkd21,nd21)
nmr = LRnumer(ssd21,calld21,nd21,dg,a,cc)
dnm =(nmr*rel[2]+LRdenom(ssd21,calld21,rel,nd21,dg,a,cc))
LRd21 = nmr/dnm
mx = max(mx,LRd21)
d21_LRs<-cbind("D21",nmr,dnm,LRd21)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d3 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D3_B.count.csv",head=T,sep=",")
nd3 = d3[,race]			# allele counts
names(nd3) = d3[,2]		# allele names
ssd3 = c("15","16")		# genotye of suspected contributor of minor component
calld3 = list(list(c("15","16"),c(NULL)))  
all_checkd3<-c(unlist(calld3),ssd3)
nd3<-rare_adjust(all_checkd3,nd3)
nmr = LRnumer(ssd3,calld3,nd3,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd3,calld3,rel,nd3,dg,a,cc))
LRd3 = nmr/dnm
mx = max(mx,LRd3)
d3_LRs<-cbind("D3",nmr,dnm,LRd3)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d13 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D13_B.count.csv",head=T,sep=",")
nd13 = d13[,race]			# allele counts
names(nd13) = d13[,2]		# allele names
ssd13 = c("13")
calld13 = list(list(c("13"),c(NULL)))
all_checkd13<-c(unlist(calld13),ssd13)
nd13<-rare_adjust(all_checkd13,nd13)
nmr = LRnumer(ssd13,calld13,nd13,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd13,calld13,rel,nd13,dg,a,cc))
LRd13 = nmr/dnm
mx = max(mx,LRd13)
d13_LRs<-cbind("D13",nmr,dnm,LRd13)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

vwa = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/vWA_B.count.csv",head=T,sep=",")
nvwa = vwa[,race]			# allele counts
names(nvwa) = vwa[,2]		# allele names
ssvwa = c("16","17")
callvwa = list(list(c("16","17"),c(NULL)))
all_checkvwa<-c(unlist(callvwa),ssvwa)
nvwa<-rare_adjust(all_checkvwa,nvwa)
nmr = LRnumer(ssvwa,callvwa,nvwa,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssvwa,callvwa,rel,nvwa,dg,a,cc))
LRvwa = nmr/dnm
mx = max(mx,LRvwa)
vwa_LRs<-cbind("VWA",nmr,dnm,LRvwa)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d5 = read.table("/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D5_B.count.csv",head=T,sep=",")
nd5 = d5[,race]			# allele counts
names(nd5) = d5[,2]		# allele names
ssd5 = c("12","13")
calld5 = list(list(c("11","12","13"),c(NULL)))  
all_checkd5<-c(unlist(calld5),ssd5)
nd5<-rare_adjust(all_checkd5,nd5)
nmr = LRnumer(ssd5,calld5,nd5,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd5,calld5,rel,nd5,dg,a,cc))
LRd5 = nmr/dnm
mx = max(mx,LRd5)
d5_LRs<-cbind("D5",nmr,dnm,LRd5)
totnmr=totnmr*nmr
totdnm=totdnm*dnm


totLR<-LRd8*LRd21*LRd3*LRd13*LRvwa*LRd5

#write the per-locus LRs to a file:
locus_LRs<-rbind(headers,d8_LRs,d21_LRs,d3_LRs,d13_LRs,vwa_LRs,d5_LRs)
locus_totals<-cbind("Totals",totnmr,totdnm,totLR)

write.table(locus_LRs,"/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Lab Retriever/Lab Retriever testing/Test 4/Balfour 50A 3Unk ProfilerPlus R result.txt", sep="\t",col.names=FALSE, row.names=FALSE, append=TRUE)

write.table(locus_totals,"/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Lab Retriever/Lab Retriever testing/Test 4/Balfour 50A 3Unk ProfilerPlus R result.txt", sep="\t",col.names=FALSE, row.names=FALSE, append=TRUE)


# Create file, place the race header in first quotes, and then add the Locus_LR table to that file. This creates a table with the race at the beginning of the table; multiple race computations can be performed in one file by copying and pasting the same code but changing the race number at the beginning of the code. Change the race below for each iteration.

#sink(NULL)
#sink ("INSERT PATH/FILENAME TO OUTPUT HERE", append=TRUE)
"INSERT RACE HERE"

#write output file:
#write.table(locus_LRs,"INSERT PATH/FILENAME TO OUTPUT HERE",append=TRUE,quote=FALSE,sep="\t",col.names=FALSE)

date()
