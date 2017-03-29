
/*
 *    libGFF: Genie File Format
 *    By Andre Caceres Carrilho
 *
 *      This code is part of the zEMPIRES project, an open source
 *      reimplementation of Ensemble Studios Genie engine.
 *
 *      Genie engine sprites (SLP) file format
 *      Credits: Format description found on the web (unknown authors)
 *
 *      See license.md for copyright information.
 */

#ifndef GFF_FILESLP_H
#define GFF_FILESLP_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * In game graphics/sprites (.SLP) file format
 *
 * The graphics from the first AoE used a command line to encode the bitmaps,
 * while newer versions of the engine used the "ArtDesk 1.00 SLP Writer" to
 * this task. The decoded SLP data are indexes (byte array) to a 8 bit color
 * palette (.PAL). Most of in game bitmaps use palette file indexed by 50500
 * residing on interfac.drs
 */

/*
 * Genie SLP file header
 * This represents the initial data of the file.
 */
typedef struct {
  char        file_version[4];
  int32_t     frame_count;
  char        information[24];
} GSLPHeader;

/*
 * Genie SLP frame header
 * Following the header, there are "frame_count" frame headers structures
 */
typedef struct {
  /* Offset to command data  and RowEdge (see below) structures */
  uint32_t    command_offset;
  uint32_t    outline_offset;

  /* Aparently these two variables are not used by the vanilla Genie engine */
  uint32_t    palette_offset;
  uint32_t    properties;

  /* Dimensions and center coordinates (hot-spot) */
  int32_t     size_w, size_h;
  int32_t     center_x, center_y;
} GSLPFrameHeader;

typedef struct {
  uint16_t    dist_right, dist_left;
} GSLPRowEdge;

typedef struct {
  GSLPFrameHeader   header;

  GSLPRowEdge       *outline;  /* [height] */
  uint32_t          *command;  /* [height] */

  /* Raw pixel data (palette indexes) */
  uint8_t           *enc_data;  /* [???] */
  uint8_t           *dec_data;  /* [height*width] */

  /*
   * This is a mask to flag the decoded raw data pixels have to be translated
   * of (player << 4) in order to get the correct civilization palette color
   * (blue, red, etc...). See GSLP_GetRawFrameData ()
   */
  uint8_t           *civcol; /* [height*width] */

  /* Helper flags */
  uint8_t       encoded;
  uint8_t       decoded;
  uint8_t       need_player; /* need player index ? */
} GSLPFrame;

typedef struct {
  GSLPHeader      header;
  GSLPFrame       *frames;
} GSLPFile;

GSLPFile* GSLP_DecodeFile(TtkBuffer *mem_buf);
void GSLP_DestroyFile(GSLPFile *slp_file);

int GSLP_DecodeFrame(GSLPFrame *slp_frame);
int GSLP_EncodeFrame(GSLPFrame *slp_frame);

uint8_t* GSLP_GetRawData(const GSLPFrame *slp_frame);
uint8_t* GSLP_GetRawDataPlayer(const GSLPFrame *slp_frame, int player_index);

#ifdef __cplusplus
}
#endif
#endif /* !GFF_FILESLP_H */
