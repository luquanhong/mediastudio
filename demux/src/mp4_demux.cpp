//============================================================================
// Name        : mp4_demux.cpp
// Author      : LQH
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <fstream>
#include <string>

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include "mp4_demux.h"

using namespace std;

#ifndef VARIABLES
#define VARIABLES			\
	int k = 0;				\
	unsigned char p[5];		\
	int	inner_size = 0;
#endif

static struct mp4_moov_box moov;

void test_indian(){

	char p[4] = {0x00,0x00,0x00,0x1c};
	int ret = p[0]>>24 | p[1] >> 16 | p[2] >> 8 | p[3];

	cout << "Hello World!!!=="<< ret << endl; // prints Hello World!!!
}

////////////////////////////////////////////////level6
//----------------------------------------------------------------------

struct mp4_avcC_box* mp4_read_avcC_box(FILE* fp){

	struct mp4_avcC_box* box = new mp4_avcC_box;

	box->size 						= read_uint32_lit(fp);
	box->type 						= read_uint32_big(fp);
	box->configurationVersion 		= read_uint8(fp);
	box->AVCProfileIndication 		= read_uint8(fp);
	box->profileCompatibility 		= read_uint8(fp);
	box->AVCLevelIndication 		= read_uint8(fp);
	box->lengthSizeMinusOne 		= (read_uint8(fp) & 0x03 );
	box->numOfSequenceParameterSet 	= (read_uint8(fp) & 0x1F );

	if(box->type == ( ('a')|('v'<<8) |('c' << 16) |('C' << 24) ) )
			printf("!type: 			avcC\n");

	printf("!size is 		%d\n",box->size);
	printf("!configurationVersion:	%u\n",box->configurationVersion);
	printf("!AVCProfileIndication: 	%x\n",box->AVCProfileIndication);
	printf("!lengthSizeMinusOne:	%u\n",box->lengthSizeMinusOne);

	printf("!numOfSPS:		%x\n",box->numOfSequenceParameterSet);
	box->sps = new mp4_avcC_box::SPS[box->numOfSequenceParameterSet];

	for(int i = 0; i < box->numOfSequenceParameterSet;i++){

		box->sps[i].sequenceParameterSetNALLength = read_uint16_lit(fp);
		box->sps[i].sequenceParameterSetNalUint =
				new uint8_t[box->sps[i].sequenceParameterSetNALLength];

		fread(box->sps[i].sequenceParameterSetNalUint,
			  box->sps[i].sequenceParameterSetNALLength, 1, fp);

		printf("!sps:: ");
		for(int j=0; j < box->sps[i].sequenceParameterSetNALLength; j++){

			printf("0X%x ", box->sps[i].sequenceParameterSetNalUint[j]);
		}
		printf("$\n");

	}

	box->numOfPictureParameterSets = read_uint8(fp);
	printf("numOfPPS: 		%x\n",box->numOfPictureParameterSets);
	box->pps = new mp4_avcC_box::PPS[box->numOfPictureParameterSets];

	for(int i = 0; i < box->numOfPictureParameterSets; i++){

		box->pps[i].pictureParameterSetNALLength = read_uint16_lit(fp);
		box->pps[i].pictureParameterSetNALUint =
				new uint8_t[box->pps[i].pictureParameterSetNALLength];

		fread(box->pps[i].pictureParameterSetNALUint,
			  box->pps[i].pictureParameterSetNALLength, 1, fp);

		printf("!pps:: ");
		for(int j = 0; j < box->pps[i].pictureParameterSetNALLength; j++){

			printf("0X%x ", box->pps[i].pictureParameterSetNALUint[j]);
		}
		printf("$\n");

	}

	return box;
}


struct mp4_avc1_box* mp4_read_avc1_box(FILE* fp, int size){

	printf("\t\t\t\t\t\t+%s\n","avc1");

	struct mp4_avc1_box* box = new mp4_avc1_box;

	box->size 					= size;
	box->type 					= ( ('a'<<24) |('v'<<16) |('c'<<8) |('1') );

	fread(&(box->reserved), sizeof(box->reserved), 1, fp); // 6 bytes
	box->data_reference_index 	= read_uint16_lit(fp);
	box->pre_defined 			= read_uint16_big(fp);
	box->reserved1 				= read_uint16_big(fp);
	fread(&(box->pre_defined1), sizeof(box->pre_defined1), 1, fp);
	box->width 					= read_uint16_lit(fp);
	box->height 				= read_uint16_lit(fp);
	box->horiz_res 				= read_uint32_lit(fp);
	box->vert_res 				= read_uint32_lit(fp);
	box->reserved2 				= read_uint32_lit(fp);
	box->frames_count 			= read_uint16_lit(fp);
	fread(box->compressor_name, sizeof(box->compressor_name), 1, fp);
	box->bit_depth 				= read_uint8(fp);
	box->pre_defined2 			= read_uint16_big(fp);

	printf("\n!----------------------------------------------------\n");
	printf("!AVC Decoder Configuration Record:\n\n");
	printf("!compressor_name is 	%s\n",box->compressor_name);
	printf("!data_reference_index:	0X%x\n",box->data_reference_index);
	printf("!width:			%u\n",box->width);
	printf("!height:		%u\n",box->height);
	printf("!frames_count:		0X%x\n",box->frames_count);
	printf("!bit_depth:		%u\n",box->bit_depth);
	printf("!pre_defined2:		0X%x\n\n",box->pre_defined2);

	box->avcC = mp4_read_avcC_box(fp);

	printf("!----------------------------------------------------\n\n");

	return box;
}

struct mp4_mp4a_box* mp4_read_mp4a_box(FILE* fp, int size){

	printf("\t\t\t\t\t\t+%s\n","mp4a");

	return NULL;
}

struct mp4_stsd_box mp4_read_stsd_box(FILE* fp, int size){

	printf("\t\t\t\t\t+%s\n","stsd");

	struct mp4_stsd_box box;

	box.size 			= size;
	box.version 		= read_uint8(fp);
	fread(&box.flags,sizeof(box.flags),1,fp);
	box.num_of_entries 	= read_uint32_lit(fp);

	printf("\t\t\t\t\t\tnumber of entries: %u\n", box.num_of_entries);

	box.sampleDescript 	= new sample_description[box.num_of_entries];

	for(unsigned int i = 0;i < box.num_of_entries; i++){

		box.sampleDescript[i].size = read_uint32_lit(fp);
		box.sampleDescript[i].type = read_uint32_lit(fp);	//mark

		if(box.sampleDescript[i].type == ( ('m'<<24) | ('p'<<16) |
											('4'<<8) | ('a') ) ){

			box.sampleDescript[i].mp4a
				= mp4_read_mp4a_box(fp, box.sampleDescript[i].size );
			box.sampleDescript[i].avc1 = 0;
		}else if(box.sampleDescript[i].type == (('a'<<24) | ('v'<<16) |
												('c'<<8)  | ('1') ) ){

			box.sampleDescript[i].avc1
				= mp4_read_avc1_box(fp, box.sampleDescript[i].size);
			box.sampleDescript[i].mp4a = 0;
		}
	}

	return box;
}

//----------------------------------------------------------------------
struct mp4_stts_box mp4_read_stts_box(FILE* fp, int size){

	printf("\t\t\t\t\t+%s\n","stts");

	mp4_stts_box box;

	box.size 				= size;
	box.version 			= read_uint8(fp);
	fread(box.flags, sizeof(box.flags), 1, fp);
	box.numOfEntries 		= read_uint32_lit(fp);

	printf("\t\t\t\t\t\tnumber of entries:	%u\n",box.numOfEntries);
	printf("\t\t\t\t\t\ttime-to-sample table: \n\t\t\t\t\t\t");

	box.time_to_sample_table = new uint32_t[box.numOfEntries];

	for(unsigned int i = 0; i < box.numOfEntries; i++){

		box.time_to_sample_table[i] = read_uint32_lit(fp);

		printf("%u ",box.time_to_sample_table[i]);
		if((i+1)%8 == 0) printf("\n");
	}

	printf("\n\n");

	return box;
}

//-----------------------------------------------------------------------
struct mp4_stss_box mp4_read_stss_box(FILE* fp, int size){

	printf("\t\t\t\t\t+%s\n","stss");

	struct mp4_stss_box box;

	box.size 				= size;
	box.version 			= read_uint8(fp);
	fread(box.flags, sizeof(box.flags), 1, fp);
	box.numOfEntries 		= read_uint32_lit(fp);

	printf("\t\t\t\t\t\tnumber of entries:	%u\n",box.numOfEntries);
	printf("\t\t\t\t\t\tsync sample table: \n");

	box.sync_sample_table	= new uint32_t[box.numOfEntries];

	printf("\t\t\t\t\t\t");
	for(unsigned int i = 0; i < box.numOfEntries; i++){

		box.sync_sample_table[i] = read_uint32_lit(fp);

		printf("%4u ", box.sync_sample_table[i]);
		if( (i+1)%12 == 0) printf("\t\t\t\t\t\t\n");
	}

	printf("\n\n");

	return box;
}

//------------------------------------------------------------------------
struct mp4_stsc_box mp4_read_stsc_box(FILE* fp, int size){

	printf("\t\t\t\t\t+%s\n","stsc");

	struct mp4_stsc_box box;

	box.size 			= size;
	box.version 		= read_uint8(fp);
	fread(box.flags, sizeof(box.flags), 1, fp);
	box.map_amount 		= read_uint32_lit(fp);

	printf("\t\t\t\t\t\tmap-amount: %u\n",box.map_amount);

	box.scMap = new mp4_list_t[box.map_amount];
	printf("\t\t\t\t\t\tfirst chunk:\tsamples-per-chunk:\tsample-description-id\n");

	for(unsigned int i = 0; i < box.map_amount; i++){

		box.scMap[i].first_chunk_num 			= read_uint32_lit(fp);
		box.scMap[i].sample_amount_in_cur_table = read_uint32_lit(fp);
		box.scMap[i].sample_description_id 		= read_uint32_lit(fp);

		printf("\t\t\t\t\t\t%d",box.scMap[i].first_chunk_num);
		printf("\t\t%d",box.scMap[i].sample_amount_in_cur_table);
		printf("\t\t\t%d\n",box.scMap[i].sample_description_id);
	}

	printf("\n\n");

	return box;
}

//------------------------------------------------------------------------
struct mp4_stsz_box mp4_read_stsz_box(FILE* fp, int size){

	printf("\t\t\t\t\t+%s\n","stsz");

	struct mp4_stsz_box box;

	box.size = size;
	box.version = read_uint8(fp);
	fread(box.flags, sizeof(box.flags), 1, fp);
	box.samples_size_total = read_uint32_lit(fp);

	if(box.samples_size_total == 0){

		box.table_size = read_uint32_lit(fp);
		box.sample_size_table = new uint32_t[box.table_size];

		printf("\t\t\t\t\t\tall samples amount: %u\n",box.samples_size_total);
		printf("\t\t\t\t\t\tsample table size:  %u\n",box.table_size);

		printf("\t\t\t\t\t\tsample-size-table:\n");
		printf("\t\t\t\t\t\t");

		for(unsigned int i = 0; i < box.table_size; i++){

			box.sample_size_table[i] = read_uint32_lit(fp);

			printf("%8u ",box.sample_size_table[i]);
			if((i + 1)%12 == 0)	printf("\n\t\t\t\t\t\t");
		}
	}

	printf("\n\n");

	return box;
}

//------------------------------------------------------------------------
struct mp4_stco_box mp4_read_stco_box(FILE* fp, int size){

	printf("\t\t\t\t\t+%s\n","stco");

	mp4_stco_box box;

	box.size = size;
	box.version = read_uint8(fp);
	fread(box.flags, sizeof(box.flags), 1, fp);
	box.numOfEntries = read_uint32_lit(fp);
	box.chunk_offset_table = new uint32_t[box.numOfEntries];

	printf("\t\t\t\t\t\tall offset amount: %u\n",box.numOfEntries);
	printf("\t\t\t\t\t\tchunk-offset-table:\n");

	printf("\t\t\t\t\t\t");

	for(unsigned int i = 0; i < box.numOfEntries; i++){

		box.chunk_offset_table[i] = read_uint32_lit(fp);

		printf("%8u ",box.chunk_offset_table[i]);
		if((i + 1)%12 == 0)	printf("\n\t\t\t\t\t\t");
	}

	printf("\n\n");

	return box;
}

/////////////////////////////////////////////////level5

struct mp4_vmhd_box* mp4_read_vmhd_box(FILE* fp, int size){

	printf("\t\t\t\t+%s\n","vmhd");

	VARIABLES;
	struct mp4_vmhd_box* box = new mp4_vmhd_box;

	box->size 			= size;
	box->type 			= 0;
	box->version 		= read_uint8(fp);
	fread(&box->flags,sizeof(box->flags), 1, fp);
	box->graphics_mode 	= read_uint32_lit(fp);
	fread(&box->opcolor,sizeof(box->opcolor),1,fp);

	printf("\t\t\t\t\tgraphic_mod: %u\n",box->graphics_mode);
	printf("\t\t\t\t\topcolor:     %u, %u, %u, %u\n",
			box->opcolor[0], box->opcolor[1],
			box->opcolor[2], box->opcolor[3]);



	return box;
}

struct mp4_smhd_box* mp4_read_smhd_box(FILE* fp, int size){

	printf("\t\t\t\t+%s\n","smhd");

	VARIABLES;
	struct mp4_smhd_box* box = new mp4_smhd_box;

	box->size 			= size;
	box->type		 	= 's'|'m'<<8|'h'<<16|'d'<<24;
	box->version 		= read_uint8(fp);
	fread(&box->flags,sizeof(box->flags),1, fp);
	box->balance 		= read_uint16_big(fp);
	box->reserved 		= read_uint16_big(fp);

	printf("\t\t\t\t\tbalance: %d,%d\n",
	           (box->balance & 0xff00) >> 8, box->balance & 0xff);

	return box;
}

struct mp4_dinf_box mp4_read_dinf_box(FILE*fp, int size){

	printf("\t\t\t\t+%s\n","dinf");

	struct mp4_dinf_box box;

	return box;
}

struct mp4_stbl_box mp4_read_stbl_box(FILE* fp, int size){

	printf("\t\t\t\t+%s\n","stbl");

	VARIABLES;
	struct mp4_stbl_box box;

	int boxSize = 0;
	int curPos = ftell(fp);

	do{

		fseek(fp,curPos,SEEK_SET);

		boxSize = read_uint32_lit(fp);
		fread(p,4,1,fp);
		p[4] = 0;

		string name = (char*)p;

		if(name == "stsd"){

			box.stsd = mp4_read_stsd_box(fp,boxSize);
		}else if(name == "stts"){

			box.stts = mp4_read_stts_box(fp,boxSize);
		}else if(name == "stss"){

			box.stss = mp4_read_stss_box(fp,boxSize);
		}else if(name == "stsc"){

			box.stsc = mp4_read_stsc_box(fp,boxSize);
		}else if( (name == "stsz") || (name == "stz2") ){

			box.stsz = mp4_read_stsz_box(fp,boxSize);
		}else if( (name == "stco") || (name == "c064") ){

			box.stco = mp4_read_stco_box(fp,boxSize);
		}else{

			printf("\t\t\t\t\t+%s===============mark undifined\n\n", p);
		}

		curPos += boxSize;
		inner_size += boxSize;

	}while((inner_size + 8) != size);

	return box;
}

/////////////////////////////////////////////////level4

struct mp4_mdhd_box mp4_read_mdhd_box(FILE* fp, int size){

	printf("\t\t\t+%s\n","mdhd");

	VARIABLES;
	struct mp4_mdhd_box box;

	box.size 				= size;
	box.type 				= 0;
	box.version 			= read_uint8(fp);
	fread(&box.flags,sizeof(box.flags),1,fp);
	box.create_time 		= read_uint32_lit(fp);
	box.modification_time 	= read_uint32_lit(fp);
	box.timescale 			= read_uint32_lit(fp);
	box.duration 			= read_uint32_lit(fp);
	box.language 			= read_uint16_big(fp);
	box.pre_defined 		= read_uint16_big(fp);

	printf("\t\t\t\ttimescale: %u\n",box.timescale);
	printf("\t\t\t\tduration:  %u\n",box.duration);
	printf("\t\t\t\tlanguange: %u\n",box.language);

	return box;
}

struct mp4_hdlr_box mp4_read_hdlr_box(FILE* fp, int size){

	printf("\t\t\t+%s\n","hdlr");

	VARIABLES;
	struct mp4_hdlr_box box;

	box.size 				= size;
	box.type 				= 'h'|'d'<<8|'l'<<16|'r'<<24;
	box.version 			= read_uint8(fp);
	fread(&box.flags,sizeof(box.flags), 1, fp);
	box.pre_defined 		= read_uint32_lit(fp);
	fread(&box.handler_type,sizeof(box.handler_type), 1, fp);
	box.handler_type[4] 	= 0;
	fread(&box.reserved,sizeof(box.reserved),1,fp);
	fread(&inner_size,1,1,fp);

	box.name = new char[inner_size + 1];

	for(int i=0; i<inner_size; i++){

		fread(&box.name[i],sizeof(unsigned char), 1, fp);
	}
	box.name[inner_size] = 0;

	printf("\t\t\t\thandle_type: %s\n",box.handler_type);
	printf("\t\t\t\tname:        %s\n",box.name);

	return box;
}


struct mp4_minf_box mp4_read_minf_box(FILE* fp, int size){

	printf("\t\t\t+%s\n","minf");

	VARIABLES;
	struct mp4_minf_box box;

	int boxSize = 0;
	int curPos = ftell(fp);

	do{

		fseek(fp,curPos,SEEK_SET);

		boxSize = read_uint32_lit(fp);
		fread(p,4,1,fp);
		p[4] = 0;

		string name = (char*)p;

		if(name == "vmhd"){

			box.vmhd = mp4_read_vmhd_box(fp,boxSize);
		}else if(name == "smhd"){

			box.smhd = mp4_read_smhd_box(fp,boxSize);
		}else if(name == "dinf"){

			box.dinf = mp4_read_dinf_box(fp,boxSize);
		}else if(name == "stbl"){

			box.stbl = mp4_read_stbl_box(fp,boxSize);
		}else{

			printf("\t\t\t\t====type: %s ===size: %u\n",p,boxSize);
		}

		curPos += boxSize;
		inner_size += boxSize;

	}while(inner_size + 8 != size);


	return box;
}



//////////////////////////////////////////////////level3

struct mp4_edts_box mp4_read_edts_box(FILE* fp, int size){

	printf("\t\t+%s\n","edts");
}

struct mp4_tkhd_box mp4_read_tkhd_box(FILE* fp, int size){

	printf("\t\t+%s\n","tkhd");

	VARIABLES;
	mp4_tkhd_box box;

	box.size 				= size;
	box.type 				= 0;
	box.version 			= read_uint8(fp);
	fread(&box.flags,sizeof(box.flags),1,fp);
	box.create_time 		= read_uint32_lit(fp);
	box.modification_time 	= read_uint32_lit(fp);
	box.track_id 			= read_uint32_lit(fp);
	box.reserved1 			= read_uint32_lit(fp);
	box.duration 			= read_uint32_lit(fp);
	fread(&box.reserved2,sizeof(box.reserved2),1,fp);
	box.layer 				= read_uint16_big(fp);
	box.alternate_group 	= read_uint16_big(fp);
	box.volume 				= read_uint16_big(fp);
	box.reserved3 			= read_uint16_big(fp);
	fread(&box.matrix,sizeof(box.matrix),1,fp);
	box.width 				= read_uint32_lit(fp);
	box.height 				= read_uint32_lit(fp);

	printf("\t\t\ttrak id: %u\n",box.track_id);
	printf("\t\t\tduration: %d\n",box.duration);
	printf("\t\t\tlayer: %d\n",box.layer);
	printf("\t\t\talternate group: %d\n",box.alternate_group);
	printf("\t\t\tvolume: 0x%x\n",box.volume);

	printf("\t\t\tmatrix:\n");
	for(int i=0;i<3;i++){
		printf("\t\t\t");
		for(int j=0;j<3;j++){

			printf("%8u ", box.matrix[i*3+j]);
		}
		printf("\n");
	}

	printf("\t\t\twidth:   [%u].[%u]\n",
			(short)((box.width & 0xffff0000) >> 16),
			(short)(box.width & 0xffff) );

	printf("\t\t\theight:  [%u].[%u]\n",
			(box.height & 0xffff0000) >> 16,
			box.height & 0xffff);

	return box;
}


struct mp4_mdia_box mp4_read_mdia_box(FILE* fp, int size){

	printf("\t\t+%s\n","mdia");

	VARIABLES;
	struct mp4_mdia_box box;

	box.size = size;
	box.type = 'm' |'d'<<8|'i'<<16|'a'<<24;

	int boxSize =0;
	int curPos = ftell(fp);

	do{

		fseek(fp,curPos,SEEK_SET);

		boxSize = read_uint32_lit(fp);
		fread(p,4,1,fp);
		p[4] = 0;

		string name = (char*)p;

		if(name == "mdhd"){

			box.mdhd = mp4_read_mdhd_box(fp,boxSize);
		}else if(name == "hdlr"){

			box.hdlr = mp4_read_hdlr_box(fp,boxSize);
		}else if(name == "minf"){

			box.minf = mp4_read_minf_box(fp,boxSize);
		}else{

			printf("\t\t===type: %s\n",p);
		}

		curPos += boxSize;
		inner_size += boxSize;

	}while(inner_size+8 != size);

	return box;
}



//////////////////////////////////////////////////level2

struct mp4_udta_box mp4_read_udta_box(FILE* fp, int size){

	 printf("\t\t+%s\n", "udta");

	 return mp4_udta_box();
}

void mp4_read_iods_box(FILE* fp, int size){

	 printf("\t+iods-------------------------undefined\n\n");
}

void mp4_read_trak_box(FILE* fp, int size){

	printf("\t+%s\n","trak");

	VARIABLES;
	int boxSize = 0;
	int curPos =  ftell(fp);
	struct mp4_trak_box* trak = new struct mp4_trak_box;

	do{

		fseek(fp,curPos,SEEK_SET);

		boxSize = read_uint32_lit(fp);
		fread(p,4,1,fp);
		p[4] = 0;

		string name = (char*)p;
		if(name == "tkhd"){

			trak->tkhd = mp4_read_tkhd_box(fp,boxSize);
		}else if(name == "edts"){

			trak->edts = mp4_read_edts_box(fp,boxSize);
		}else if(name == "mdia"){

			trak->mdia = mp4_read_mdia_box(fp,boxSize);
		}else if(name == "udta"){

			trak->udta = mp4_read_udta_box(fp,boxSize);
		}else{
			printf("\t\t+%s==========\n",p);
		}

		curPos += boxSize;
		inner_size += boxSize;

	}while(inner_size + 8 != size);

	moov.trak.push_back(trak);
}

void mp4_read_mvhd_box(FILE* fp, int size){

	printf("\t+%s\n","mvhd");

	VARIABLES;
	mp4_mvhd_box mvhd;

	memset(&mvhd,0,sizeof(mvhd) );

	mvhd.size 				= size;
	mvhd.type 				= ('m' | 'v'<<8 | 'h'<<16 | 'd'<<24);
	mvhd.version 			= read_uint8(fp);
	fread(mvhd.flags,sizeof(mvhd.flags),1,fp);
	mvhd.create_time 		= read_uint32_lit(fp);
	mvhd.modification_time 	= read_uint32_lit(fp);
	mvhd.timescale 			= read_uint32_lit(fp);
	mvhd.duration 			= read_uint32_lit(fp);
	mvhd.rate 				= read_uint32_lit(fp);
	mvhd.volume 			= read_uint16_big(fp);
	fread(&mvhd.reserved1,sizeof(mvhd.reserved1),1,fp);
	fread(mvhd.reserved2,sizeof(mvhd.reserved2),1,fp);
	fread(mvhd.matrix,sizeof(mvhd.matrix),1,fp);
	fread(mvhd.pre_defined,sizeof(mvhd.pre_defined),1,fp);
	mvhd.next_trak_id 		= read_uint32_lit(fp);

	moov.mvhd = mvhd;

	printf("\t\ttimescale: %u\n",moov.mvhd.timescale);
	printf("\t\tduration: %u\n",moov.mvhd.duration);
	printf("\t\trate: %u\n",moov.mvhd.rate);
	printf("\t\tvolume: 0x%x\n",moov.mvhd.volume);

	printf("\t\tmatrix:\n");
	for(int i=0;i<3;++i){

		printf("\t\t");
		for(int j=0;j<3;j++){
			printf(" %8u",moov.mvhd.matrix[i*3+j]);
		}
		printf("\n");
	}

	printf("\t\tnext track id: %u\n",moov.mvhd.next_trak_id);
	printf("\n");
}

//////////////////////////////////////////////////level1

void mp4_read_mdat_box(FILE* fp, int size){

	printf("\t+%s\n","mdat");
	printf("\tthis is the real media data %d\n",size);
}

void mp4_read_moov_box(FILE* fp, int size){

	printf("+%s\n","moov");

	VARIABLES;
	int level2_box_size = 0;

	moov.size = size;
	moov.type = 'm' | 'o' <<8 | 'o' << 16 | 'v' << 24;

	int curPos = ftell(fp);

	do{

		fseek(fp,curPos,SEEK_SET);

		level2_box_size = read_uint32_lit(fp);

		fread(&k,sizeof(k),1,fp);
		indian_a.size = k;
		memcpy(p,indian_a.byte,sizeof(indian_a.byte) );
		p[4] = 0;

		string name = (char* )p;

		if(name == "mvhd"){

			mp4_read_mvhd_box(fp,level2_box_size);
		}else if(name == "trak"){

			mp4_read_trak_box(fp,level2_box_size);
		}else if(name == "iods"){

			mp4_read_iods_box(fp,level2_box_size);
		}else if(name == "udta"){

			mp4_read_udta_box(fp,level2_box_size);
		}else{
			printf("=====%s\n\n",p);
		}

		curPos += level2_box_size;
		inner_size += level2_box_size;

	}while(inner_size + 8 != size);


}

void mp4_read_ftyp_box(FILE* fp, int size){

	printf("+ftyp\n");

	size -= 8;

	char* ch = new char[size + 1];
	//printf("size is %d\n",size);

	for(int i = 0;i < size; ++i){

		fread(&ch[i],sizeof(char),1,fp);
	}

	ch[size] = 0;
	printf("\tftyp: %s\n",ch);

	delete [] ch;

}

/////////////////////////////////////////////////level0
int mp4_read_root_box(FILE* fp){

	int k = 0;
	unsigned char p[5];

	int level1_box_size = read_uint32_lit(fp);
	//printf("level1_box_size is %d\n",level1_box_size);

	fread(&k,sizeof(k),1,fp);
	indian_a.size = k;
	memcpy(p,indian_a.byte,sizeof(indian_a.byte));
	p[4] = 0;

	string name = (char*)p;

	if(name == "moov"){

		mp4_read_moov_box(fp,level1_box_size);
	}else if(name == "ftyp"){

		mp4_read_ftyp_box(fp,level1_box_size);
	}else if(name == "mdat"){

		mp4_read_mdat_box(fp,level1_box_size);
	}else if(name == "free"){

		printf("+free\n");
	}else if(level1_box_size == 0){

		return 1;
	}else{

		printf("%s==mark undifined \n",p);
	}


	return level1_box_size;
}
////////////////////////////////////////////////////read media data

uint32_t get_sample_num_in_cur_chunk(const struct mp4_stsc_box& stsc,
									 const uint32_t index){

	uint32_t sample_num_in_cur_chunk = 0;

	for(int i =0; i< stsc.map_amount; i++){

		if(i+1 == stsc.map_amount){

			sample_num_in_cur_chunk =
					stsc.scMap[i].sample_amount_in_cur_table;

			break; // exit
		}

		if( (index + 1 >= stsc.scMap[i].first_chunk_num) &&
			(index + 1 <  stsc.scMap[i+1].first_chunk_num   ) ){

			sample_num_in_cur_chunk =
					stsc.scMap[i].sample_amount_in_cur_table;

			break;	// exit
		}
	}

	return sample_num_in_cur_chunk;
}

uint32_t get_sample_index(const struct mp4_stsc_box& stsc,
						  const uint32_t chunk_index){

	/*
	 * first 	chunk 	samples	id
	 * 1		4		1
	 * 116		2		1
	 */

	uint32_t start = chunk_index;
	uint32_t sample_index = 0;

	for(int i = 0; i < stsc.map_amount; i++){

		uint32_t chunk_amount = 0;
		uint32_t cur = stsc.scMap[i].first_chunk_num;

		if(i+1 == stsc.map_amount){

			chunk_amount = start - cur;
			sample_index += stsc.scMap[i].sample_amount_in_cur_table * chunk_amount;
			break;
		}

		uint32_t next = stsc.scMap[i + 1].first_chunk_num;

		if(start > cur){

			if(start < next){

				chunk_amount = start - cur;
				sample_index += stsc.scMap[i].sample_amount_in_cur_table * chunk_amount;
				break;
			}else{

				chunk_amount = next - cur;
				sample_index += stsc.scMap[i].sample_amount_in_cur_table * chunk_amount;
			}
		}else if( start == cur){

			break;
		}
	}

	return sample_index;
}

uint32_t get_sample_size(const struct mp4_stsz_box& stsz,
						 const uint32_t chunk_index){

	for(int i = 0; i < stsz.table_size; i++){

		if(chunk_index == i)
			return stsz.sample_size_table[i];
	}
}

void mp4_copy_chunk_data(FILE* fin, const struct mp4_stbl_box& stbl, int chunk_index, FILE* fout){

	fseek(fin, stbl.stco.chunk_offset_table[chunk_index], SEEK_SET );

	uint32_t sample_num_in_cur_chunk =
			get_sample_num_in_cur_chunk(stbl.stsc, chunk_index);

	uint32_t sample_index =
			get_sample_index(stbl.stsc, chunk_index + 1);

	printf("offset:0X%x \n",stbl.stco.chunk_offset_table[chunk_index]);

	for(int i = 0; i < sample_num_in_cur_chunk; i++){

		uint32_t sample_size = get_sample_size(stbl.stsz, sample_index + i);
		printf("sample index = %u sample size: %u\n",sample_index+ i,sample_size);

		uint32_t NALU_size 	= 0;
		uint32_t NALU_total = 0;

		while(NALU_total < sample_size ){

			NALU_size 	= 	read_uint32_lit(fin);
			NALU_total 	+= 	(NALU_size + 4);

			char* ptr = new char[NALU_size];
			fread(ptr, NALU_size, 1, fin);

			static uint32_t one = 1 << 24;
			fwrite(&one, sizeof(one), 1, fout);
			fwrite(ptr,NALU_size, 1, fout);

			delete [] ptr;

			NALU_size = 0;
		}
	}
}


void mp4_write_media_data(FILE* fin, const struct mp4_trak_box* trak, string name){

	FILE* fout;

	int numOfChunkOffset = trak->mdia.minf.stbl.stco.numOfEntries;

	struct mp4_avc1_box* avc1 =
			trak->mdia.minf.stbl.stsd.sampleDescript[0].avc1;

	name += "x";
	fout = fopen(name.c_str(), "w");

	if(fout == NULL){

		printf("can't open the file %s\n",name.c_str());
		return;
	}

	if(avc1 != 0){

		uint32_t one = 1 << 24;				//nal uint start: 0x00 0x00 0x00 0x01
		fwrite(&one,sizeof(one), 1, fout);

		struct mp4_avcC_box* avcC = avc1->avcC;
		fwrite( avcC->sps[0].sequenceParameterSetNalUint,
				avcC->sps[0].sequenceParameterSetNALLength, 1, fout);

		fwrite(&one, sizeof(one), 1, fout);
		fwrite( avcC->pps[0].pictureParameterSetNALUint,
				avcC->pps[0].pictureParameterSetNALLength, 1, fout);
	}

	for(int chunk_index = 0; chunk_index < numOfChunkOffset; chunk_index++){

		mp4_copy_chunk_data(fin, trak->mdia.minf.stbl , chunk_index, fout);
	}

	fclose(fout);
}

////////////////////////////////////////////////////test

void get_frame_by_second(int sec){

	int timescale = 0;
}

////////////////////////////////////////////////////program enter
int main() {

	ios_base::sync_with_stdio(true);

	string movieName = "movie.mp4";

	FILE *fin = fopen(movieName.c_str(), "r");
	if(fin == NULL){
		printf("open the movie file is failure\n");
		return 0;
	}

	int currentPos = ftell(fin);
	//printf("currentPos is %d\n",currentPos);

	for(; !feof(fin) ;){
		fseek(fin,currentPos,SEEK_SET);

		printf("-------------------------------------------------------level 0\n\n");
		currentPos += mp4_read_root_box(fin);
	}

	mp4_write_media_data(fin,moov.trak[0],("video"));

	return 0;
}
