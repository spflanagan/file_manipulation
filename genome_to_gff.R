#Author: Sarah P. Flanagan
#Last updated: 2 June 2016
#Date: 2 June 2016
#Purpose: Match up data from genome alignments/blast to annotations

rm(list=ls())

#*******************START INPUT INFO HERE*************************#
genome.path<-"E:/ubuntushare/scovelli_genome/"
dat.file<-"E:/ubuntushare/Trinity_DEbrain_toScogenome_blastresults"
out.dir<-"E:/ubuntushare"
blast<-TRUE #is dat.file a blast file?
sam<-FALSE #is dat.file a sam file?
#********************END INPUT INFO HERE**************************#

#####GENOME DATA
genome<-read.delim(paste(genome.path,"SSC_genome.agp",sep=""),	
	comment.char="#",header=F)
colnames(genome)<-c("object","object_beg","object_end","part_number",
	"W","component_id","component_beg","component_end","orientation")
gen<-genome[genome$W=="W",]
scaffold.order<-gen$component_id

#####SCAFFOLD DATA
scaffs<-read.delim(paste(genome.path,"SSC_scaffolds.agp",sep=""),	
	comment.char="#",header=F)
colnames(scaffs)<-c("object","object_beg","object_end","part_number",
	"W","component_id","component_beg","component_end","orientation")
scaffs<-scaffs[scaffs$W=="W",]
gen<-rbind(gen,scaffs)

#####ANNOTATION INFO
gff<-read.delim(header=F,
	paste(genome.path,"annotated_scovelli_scaffolds_March2016.gff",sep=""))
gff.names<-c("sequence","source","feature","start","end","score","strand",
	"frame","attributes")
colnames(gff)<-gff.names
	#sequence is name of sequence
	#start is first bp(one base offset)
	#end is last bp (one base offset)
gff.chrom<-levels(gff$V1)

#####GET SAM INFO
sam2gff<-function(samrow){
	rel.gff<-gff[gff$sequence %in% samrow["RNAME"],]
	if(nrow(rel.gff)==0){
		scaffid<-gen[gen$object %in% samrow["RNAME"],]
		gffid<-scaffid[
			as.numeric(scaffid$object_beg) <= 
				as.numeric(samrow["POS"]) & 
			as.numeric(scaffid$object_end) >= 
				as.numeric(samrow["POS"]),]
		start.pos<-as.numeric(samrow["POS"])-
			as.numeric(gffid$object_beg)
		end.pos<-as.numeric(blastrow["send"])-as.numeric(gffid$object_beg)
		rel.gff<-gff[gff$sequence %in% gffid$component_id,]
		out<-rel.gff[as.numeric(rel.gff$start) <= start.pos & 
			as.numeric(rel.gff$end) >= end.pos,]
	} else {
		out<-rel.gff[as.numeric(rel.gff$start) <= 
				as.numeric(samrow["POS"]) & 
			as.numeric(rel.gff$end) >= as.numeric(samrow["POS"]),]
	}

	if(nrow(out)>0){
		out<-cbind(QNAME=rep(samrow["QNAME"],nrow(out)), 
			POS=rep(samrow["POS"],nrow(out)), out)
		return(out)
	}
}


#####GET BLAST INFO
blast2gff<-function(blastrow){
	rel.gff<-gff[gff$sequence %in% blastrow["sseqid"],]
	if(nrow(rel.gff)==0){
		scaffid<-gen[gen$object %in% blastrow["sseqid"],]
		gffid<-scaffid[
			as.numeric(scaffid$object_beg) <= 
				as.numeric(blastrow["sstart"]) & 
			as.numeric(scaffid$object_end) >= 
				as.numeric(blastrow["send"]),]
		start.pos<-as.numeric(blastrow["sstart"])-
			as.numeric(gffid$object_beg)
		end.pos<-as.numeric(blastrow["send"])-as.numeric(gffid$object_beg)
		rel.gff<-gff[gff$sequence %in% gffid$component_id,]
		out<-rel.gff[as.numeric(rel.gff$start) <= start.pos & 
			as.numeric(rel.gff$end) >= end.pos,]
	} else {
		out<-rel.gff[as.numeric(rel.gff$start) <= 
				as.numeric(blastrow["sstart"]) & 
			as.numeric(rel.gff$end) >= as.numeric(blastrow["send"]),]
	}
	if(nrow(out)>0){
		out<-cbind(qseqid=rep(blastrow["qseqid"],	nrow(out)), 
			Chrom=rep(blastrow["sseqid"],nrow(out)),
			pident=rep(blastrow["pident"],nrow(out)),
			length=rep(blastrow["length"],nrow(out)),
			mismatch=rep(blastrow["mismatch"],nrow(out)),
			gapopen=rep(blastrow["gapopen"],nrow(out)), 
			evalue=rep(blastrow["evalue"],nrow(out)),
			bitscore=rep(blastrow["bitscore"],nrow(out)),out)
		return(out)
	}
}

#####WHERE THE WORK HAPPENS
dat<-read.delim(dat.file,header=F)
if(blast==T){
	colnames(dat)<-c("qseqid","sseqid","pident","length","mismatch",
	"gapopen","qstart","qend","sstart","send","evalue","bitscore")
	output<-do.call("rbind",apply(dat,1,blast2gff))
	outname<-"blast2gff.txt"
}
if(sam==T){
	colnames(dat)<-c("QNAME","FLAG","RNAME","POS","MAPQ","CIGAR","RNEXT","PNEXT",
	"TLEN","SEQ","QUAL","OPT1","OPT2","OPT3","OPT4","OPT5","OPT6","OPT7",
	"OPT8","OPT9") 
	output<-do.call("rbind",apply(dat,1,sam2gff))
	outname<-"sam2gff.txt"
}
if(nrow(output)>0){
	if(out.dir==""){ outname<-paste(genome.path,outname) } else {
		outname<-paste(outdir,outname) }
	write.table(output,outname,sep='\t',quote=F,col.names=T,row.names=F)
} else{
	print("No output could be created. Did you specify blast OR sam file?")
}


