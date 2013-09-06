/*
 * mp4_demux.h
 *
 *  Created on: 2013-4-1
 *      Author: Administrator
 */

#ifndef MP4_DEMUX_H_
#define MP4_DEMUX_H_

#include <inttypes.h>
#include <vector>


static union __indian_swap{

	unsigned char byte[4];
	unsigned int  size;
}indian_a,indian_b;

unsigned char read_uint8(FILE* fp){

	unsigned char ch;

	fread(&ch,sizeof(ch),1, fp);

	return ch;
}

uint16_t read_uint16_big(FILE* fp){

	unsigned short k = 0;

	fread(&k,sizeof(k), 1, fp);

	return k;
}

uint16_t read_uint16_lit(FILE* fp){

	unsigned short k = 0;

	fread(&k, sizeof(k), 1, fp);

	return ( ( (k&0xff00) >> 8) | ( (k&0xff) << 8) );
}


int read_uint32_big(FILE* fp){

	int k = 0;

	fread(&k, sizeof(k), 1, fp);

	return k;
}

int read_uint32_lit(FILE* fp){

	int k = 0;

	fread(&k, sizeof(k), 1, fp);

	indian_a.size = k;

	for(int i=0, j = 3;i < 4; i++,j--){
		indian_b.byte[i] = indian_a.byte[j];
	}

	return indian_b.size;
}

//////////////////////////////////////////////////////

#ifndef MP4_BASE_BOX
#define MP4_BASE_BOX	\
	uint32_t type;		\
	uint32_t size
#endif

#ifndef MP4_FULL_BOX
#define MP4_FULL_BOX	\
	MP4_BASE_BOX;		\
	uint8_t	version;	\
	uint8_t flags[3]
#endif


#ifndef	MP4_SAMPLE_ENTRY_FIELDS
#define	MP4_SAMPLE_ENTRY_FIELDS					\
	MP4_BASE_BOX;								\
	char		reserved[6];					\
	uint16_t	data_reference_index

#endif


#ifndef VISUAL_SAMPLE_ENTRY_FIELDS
#define VISUAL_SAMPLE_ENTRY_FIELDS				\
	MP4_SAMPLE_ENTRY_FIELDS;					\
	uint16_t 	pre_defined;					\
	uint16_t 	reserved1;						\
	uint32_t	pre_defined1[3];				\
	uint16_t	width;							\
	uint16_t 	height;							\
	uint32_t 	horiz_res;						\
	uint32_t 	vert_res;						\
	uint32_t	reserved2;						\
	uint16_t	frames_count;					\
	/*AVC Decoder Configuration Record*/		\
	char		compressor_name[33];			\
	uint16_t	bit_depth;						\
	uint16_t	pre_defined2
#endif


///////////////////////////////////////////////level5
struct mp4_avcC_box{

	uint32_t	size;
	uint32_t	type;
	uint8_t		configurationVersion;
	uint8_t		AVCProfileIndication;
	uint8_t		profileCompatibility;
	uint8_t		AVCLevelIndication;

	uint8_t		lengthSizeMinusOne;			//& 0x03 == 2bit

	uint8_t		numOfSequenceParameterSet;	//& 0x1f == 5bit
	struct SPS{
		uint16_t	sequenceParameterSetNALLength;
		uint8_t*	sequenceParameterSetNalUint;
	}*sps;

	uint8_t		numOfPictureParameterSets;
	struct PPS{
		uint16_t	pictureParameterSetNALLength;
		uint8_t*	pictureParameterSetNALUint;
	}*pps;
};


struct mp4_avc1_box{

	VISUAL_SAMPLE_ENTRY_FIELDS;
	struct mp4_avcC_box* avcC;
	struct mp4_btrt_box* btrt;
	struct mp4_m4ds_box* m4ds;
};

struct mp4_mp4a_box{

};

struct sample_description{

	uint32_t size;
	uint32_t type;
	struct	 mp4_avc1_box* avc1;
	struct   mp4_mp4a_box* mp4a;
};

struct mp4_stsd_box{	//sample description

	MP4_FULL_BOX;
	uint32_t 			num_of_entries;
	sample_description* sampleDescript;
};

//----------------------------------------------
struct mp4_stts_box{	//time-to-sample

	MP4_FULL_BOX;
	uint32_t	numOfEntries;
	uint32_t*	time_to_sample_table;	//as name
};

//-----------------------------------------------
struct mp4_stss_box{	//sync sample

	MP4_FULL_BOX;
	uint32_t	numOfEntries;
	uint32_t*	sync_sample_table;
};

//-----------------------------------------------
struct mp4_list_t{

	uint32_t	first_chunk_num;
	uint32_t	sample_amount_in_cur_table;
	uint32_t	sample_description_id;
};

struct mp4_stsc_box{	//sample-to-chunk

	MP4_FULL_BOX;
	uint32_t	map_amount;
	mp4_list_t*	scMap;
};

//-----------------------------------------------
struct mp4_stsz_box{	//sample size

	MP4_FULL_BOX;
	uint32_t	samples_size_total;
	uint32_t	table_size;
	uint32_t*	sample_size_table;
};

//-----------------------------------------------
struct mp4_stco_box{	//chunk  offset

	MP4_FULL_BOX;
	uint32_t	numOfEntries;
	uint32_t*	chunk_offset_table;
};


///////////////////////////////////////////////level4

struct mp4_vmhd_box{ 			//video media head

	MP4_FULL_BOX;
	uint32_t graphics_mode;
	uint8_t opcolor[4];     	//color transaction
};


struct mp4_smhd_box{			//sound media lead

	MP4_FULL_BOX;
	uint16_t balance;
	uint16_t reserved;
};

struct mp4_dinf_box{

};

struct mp4_stbl_box{			//the most struct

	struct mp4_stsd_box stsd;
	struct mp4_stts_box stts;
	struct mp4_stss_box stss;
	struct mp4_stsc_box stsc;
	struct mp4_stsz_box stsz;
	struct mp4_stco_box stco;
};


//////////////////////////////////////////////level3
struct mp4_mdhd_box{

	MP4_FULL_BOX;
	uint32_t create_time;
	uint32_t modification_time;
	uint32_t timescale;
	uint32_t duration;
	uint16_t language;
	uint16_t pre_defined;
};

struct mp4_hdlr_box{

	MP4_FULL_BOX;
	uint32_t pre_defined;
	uint8_t  handler_type[5];
	uint32_t reserved[3];
	char*    name; //end with '\0'
};

struct mp4_minf_box{

	MP4_BASE_BOX;
	struct mp4_vmhd_box* vmhd;
	struct mp4_smhd_box* smhd;
	struct mp4_hdlr_box  hdlr;
	struct mp4_dinf_box  dinf;
	struct mp4_stbl_box  stbl;
};

//////////////////////////////////////////////level2

struct mp4_tkhd_box{

	MP4_FULL_BOX;
	uint32_t create_time;
	uint32_t modification_time;
	uint32_t track_id;
	uint32_t reserved1;
	uint32_t duration;

	uint32_t reserved2[2];
	uint16_t layer;
	uint16_t alternate_group;

	uint16_t volume;
	uint16_t reserved3;
	uint32_t matrix[9];
	uint32_t width;
	uint32_t height;
};

struct mp4_mdia_box{

	MP4_BASE_BOX;
	struct mp4_mdhd_box mdhd;
	struct mp4_hdlr_box hdlr;
	struct mp4_minf_box minf;
};

struct mp4_edts_box{

};
//////////////////////////////////////////////level1

struct mp4_mvhd_box{

	MP4_FULL_BOX;				//12 bytes
	uint32_t create_time;		//4  bytes
	uint32_t modification_time;	//4  bytes
	uint32_t timescale;			//4  bytes
	uint32_t duration;			//4  bytes
	uint32_t rate;				//4  bytes
	uint16_t volume;			//2  bytes
	uint16_t reserved1;         //2  bytes
	uint32_t reserved2[2];		//8  bytes
	uint32_t matrix[9];			//36 bytes
	uint32_t pre_defined[6];    //24 bytes
	uint32_t next_trak_id;      //4  bytes
};

struct mp4_udta_box{

};

struct mp4_trak_box{

	struct mp4_tkhd_box tkhd;
	struct mp4_mdia_box mdia;
	struct mp4_edts_box edts;
	struct mp4_udta_box udta;
};

struct mp4_iods_box{

};

/////////////////////////////////////////////////////level0

struct mp4_moov_box{

	MP4_BASE_BOX;
	struct mp4_mvhd_box mvhd;
	struct mp4_iods_box iods;
	std::vector<struct mp4_trak_box *> trak;
	struct mp4_udta_box udta;
};


#endif /* MP4_DEMUX_H_ */
