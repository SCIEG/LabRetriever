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

# Casename: Morales	

# Item #: Condom NS Interior

# LR # of unknown program: 1 Unk

dg = c(0.1) #Put the DO probs here
a = 0.5     # set value between 0 and 1
cc = 0.01	# set value between 0 and 1 #this is Din
rel = c(0,0)  # relatedness: probability that alternate possible contributor has 1 and 2 alleles ibd with suspected contributor; kept at (0,0) for all analyses here

race = 1 # 1 = Afr-Am; 3 = Cauc; 5 = Hisp;

headers<-cbind("Locus","Numerator","Denominator","LR") #Create a table with these headers

d8 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D8_B.count.csv",head=T,sep=",")
nd8 = d8[,race]			# allele counts
names(nd8) = d8[,2]		# allele names
ssd8 = c("12","13")		# genotye of suspected contributor of minor component
calld8 = list(list(c("12","13"),c("14","16"))) # the first list has the minor alleles that are not from an "assumed contributor," and can be any number of alleles, dependent on the R functions for which it will serve as input data.  Any alleles that 1) ARE from an assumed contributor, OR were 2) in a stutter position, AND 3) above the detection threshold, AND 4) below the stutter threshold would go where the word "NULL" is.
all_checkd8<-c(unlist(calld8),ssd8)
nd8<-rare_adjust(all_checkd8,nd8)
nmr = LRnumer(ssd8,calld8,nd8,dg,a,cc)
dnm = (nmr*rel[2]+LRdenom(ssd8,calld8,rel,nd8,dg,a,cc))
LRd8 = nmr/dnm
mx = max(LRd8)
d8_LRs<-cbind("D8",nmr,dnm,LRd8) #write the per-locus LRs to a file
totnmr=nmr
totdnm=dnm

d21 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D21_B.count.csv",head=T,sep=",")
nd21 = d21[,race]			# allele counts
names(nd21) = d21[,2]		# allele names
ssd21 = c("29","30")
calld21 = list(list(c("30"),c("29","31.2")))
all_checkd21<-c(unlist(calld21),ssd21)
nd21<-rare_adjust(all_checkd21,nd21)
nmr = LRnumer(ssd21,calld21,nd21,dg,a,cc)
dnm =(nmr*rel[2]+LRdenom(ssd21,calld21,rel,nd21,dg,a,cc))
LRd21 = nmr/dnm
mx = max(mx,LRd21)
d21_LRs<-cbind("D21",nmr,dnm,LRd21)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d7 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D7_B.count.csv",head=T,sep=",")
nd7 = d7[,race]			# allele counts
names(nd7) = d7[,2]		# allele names
ssd7 = c("7","8")
calld7 = list(list(c("7"),c("8","10")))
all_checkd7<-c(unlist(calld7),ssd7)
nd7<-rare_adjust(all_checkd7,nd7)
nmr = LRnumer(ssd7,calld7,nd7,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd7,calld7,rel,nd7,dg,a,cc))
LRd7 = nmr/dnm
mx = max(mx,LRd7)
d7_LRs<-cbind("D7",nmr,dnm,LRd7)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

csf = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/CSF_B.count.csv",head=T,sep=",")
ncsf = csf[,race]			# allele counts
names(ncsf) = csf[,2]		# allele names
sscsf = c("7","9")
callcsf = list(list(c("7","9"),c("10","12")))   
all_checkcsf<-c(unlist(callcsf),sscsf)
ncsf<-rare_adjust(all_checkcsf,ncsf) 
nmr = LRnumer(sscsf,callcsf,ncsf,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(sscsf,callcsf,rel,ncsf,dg,a,cc))
LRcsf = nmr/dnm
mx = max(mx,LRcsf)
dcsf_LRs<-cbind("CSF",nmr,dnm,LRcsf)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d3 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D3_B.count.csv",head=T,sep=",")
nd3 = d3[,race]			# allele counts
names(nd3) = d3[,2]		# allele names
ssd3 = c("14","15")		
calld3 = list(list(c("14","15"),c("17")))   
all_checkd3<-c(unlist(calld3),ssd3)
nd3<-rare_adjust(all_checkd3,nd3)
nmr = LRnumer(ssd3,calld3,nd3,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd3,calld3,rel,nd3,dg,a,cc))
LRd3 = nmr/dnm
mx = max(mx,LRd3)
d3_LRs<-cbind("D3",nmr,dnm,LRd3)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

tho = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/THO_B.count.csv",head=T,sep=",")
ntho = tho[,race]			# allele counts
names(ntho) = tho[,2]		# allele names
sstho = c("6","8")
calltho = list(list(c(NULL),c("6","8")))  
all_checktho<-c(unlist(calltho),sstho)
ntho<-rare_adjust(all_checktho,ntho)
nmr = LRnumer(sstho,calltho,ntho,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(sstho,calltho,rel,ntho,dg,a,cc))
LRtho = nmr/dnm
mx = max(mx,LRtho)
tho_LRs<-cbind("THO",nmr,dnm,LRtho)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d13 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D13_B.count.csv",head=T,sep=",")
nd13 = d13[,race]			# allele counts
names(nd13) = d13[,2]		# allele names
ssd13 = c("10","12")
calld13 = list(list(c("10"),c("8","12")))
all_checkd13<-c(unlist(calld13),ssd13)
nd13<-rare_adjust(all_checkd13,nd13)
nmr = LRnumer(ssd13,calld13,nd13,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd13,calld13,rel,nd13,dg,a,cc))
LRd13 = nmr/dnm
mx = max(mx,LRd13)
d13_LRs<-cbind("D13",nmr,dnm,LRd13)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d16 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D16_B.count.csv",head=T,sep=",")
nd16 = d16[,race]			# allele counts
names(nd16) = d16[,2]		# allele names
ssd16 = c("9","13")
calld16 = list(list(c("9","13"),c("10","11")))
all_checkd16<-c(unlist(calld16),ssd16)
nd16<-rare_adjust(all_checkd16,nd16)
nmr = LRnumer(ssd16,calld16,nd16,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd16,calld16,rel,nd16,dg,a,cc))
LRd16 = nmr/dnm
mx = max(mx,LRd16)
d16_LRs<-cbind("D16",nmr,dnm,LRd16)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

vwa = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/vWA_B.count.csv",head=T,sep=",")
nvwa = vwa[,race]			# allele counts
names(nvwa) = vwa[,2]		# allele names
ssvwa = c("16","18")
callvwa = list(list(c("16","18"),c("15","19")))
all_checkvwa<-c(unlist(callvwa),ssvwa)
nvwa<-rare_adjust(all_checkvwa,nvwa)
nmr = LRnumer(ssvwa,callvwa,nvwa,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssvwa,callvwa,rel,nvwa,dg,a,cc))
LRvwa = nmr/dnm
mx = max(mx,LRvwa)
vwa_LRs<-cbind("VWA",nmr,dnm,LRvwa)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

tpo = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/tpo_B.count.csv",head=T,sep=",")
ntpo = tpo[,race]			# allele counts
names(ntpo) = tpo[,2]		# allele names
sstpo = c("9","11")
calltpo = list(list(c("9"),c("11")))
all_checktpo<-c(unlist(calltpo),sstpo)
ntpo<-rare_adjust(all_checktpo,ntpo)
nmr = LRnumer(sstpo,calltpo,ntpo,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(sstpo,calltpo,rel,ntpo,dg,a,cc))
LRtpo = nmr/dnm
mx = max(mx,LRtpo)
tpo_LRs<-cbind("TPOX",nmr,dnm,LRtpo)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d18 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D18_B.count.csv",head=T,sep=",")
nd18 = d18[,race]			# allele counts
names(nd18) = d18[,2]		# allele names
ssd18 = c("12","19")
calld18 = list(list(c("19"),c("12","20")))
all_checkd18<-c(unlist(calld18),ssd18)
nd18<-rare_adjust(all_checkd18,nd18)
nmr = LRnumer(ssd18,calld18,nd18,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd18,calld18,rel,nd18,dg,a,cc))
LRd18 = nmr/dnm
mx = max(mx,LRd18)
d18_LRs<-cbind("D18",nmr,dnm,LRd18)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

d5 = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/D5_B.count.csv",head=T,sep=",")
nd5 = d5[,race]			# allele counts
names(nd5) = d5[,2]		# allele names
ssd5 = c("12")
calld5 = list(list(c(NULL),c("11","12")))   
all_checkd5<-c(unlist(calld5),ssd5)
nd5<-rare_adjust(all_checkd5,nd5)
nmr = LRnumer(ssd5,calld5,nd5,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssd5,calld5,rel,nd5,dg,a,cc))
LRd5 = nmr/dnm
mx = max(mx,LRd5)
d5_LRs<-cbind("D5",nmr,dnm,LRd5)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

fga = read.table("/Users/sb7689/Dropbox/KKKN/Statistics_DO programs/Frequency tables/FGA_B.count.csv",head=T,sep=",")
nfga = fga[,race]			# allele counts
names(nfga) = fga[,2]		# allele names
ssfga = c("20","23")
callfga = list(list(c("20","23"),c("22","26"))) 
all_checkfga<-c(unlist(callfga),ssfga)
nfga<-rare_adjust(all_checkfga,nfga)
nmr = LRnumer(ssfga,callfga,nfga,dg,a,cc)
dnm=(nmr*rel[2]+LRdenom(ssfga,callfga,rel,nfga,dg,a,cc))
LRfga = nmr/dnm
mx = max(mx,LRfga)
fga_LRs<-cbind("FGA",nmr,dnm,LRfga)
totnmr=totnmr*nmr
totdnm=totdnm*dnm

totLR<-LRd8*LRd21*LRd7*LRcsf*LRd3*LRtho*LRd13*LRd16*LRvwa*LRtpo*LRd18*LRd5*LRfga

#write the per-locus LRs to a file:
locus_LRs<-rbind(headers,d8_LRs,d21_LRs,d7_LRs,dcsf_LRs,d3_LRs,tho_LRs,d13_LRs,d16_LRs,vwa_LRs,tpo_LRs,d18_LRs,d5_LRs,fga_LRs)
locus_totals<-cbind("Totals",totnmr,totdnm,totLR)

write.table(locus_LRs,"/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Lab Retriever/Lab Retriever testing/Test 7/Morales Condom NS_Int 1Unk R result.txt", sep="\t",col.names=FALSE, row.names=FALSE, append=TRUE)

write.table(locus_totals,"/Volumes/Inman Drive 2 1Tb/Dropbox/KKKN/Lab Retriever/Lab Retriever testing/Test 7/Morales Condom NS_Int 1Unk R result.txt", sep="\t",col.names=FALSE, row.names=FALSE, append=TRUE)


# Create file, place the race header in first quotes, and then add the Locus_LR table to that file. This creates a table with the race at the beginning of the table; multiple race computations can be performed in one file by copying and pasting the same code but changing the race number at the beginning of the code. Change the race below for each iteration.

#sink(NULL)
#sink ("INSERT PATH/FILENAME TO OUTPUT HERE", append=TRUE)
"INSERT RACE HERE"

#write output file:
#write.table(locus_LRs,"INSERT PATH/FILENAME TO OUTPUT HERE",append=TRUE,quote=FALSE,sep="\t",col.names=FALSE)

date()
