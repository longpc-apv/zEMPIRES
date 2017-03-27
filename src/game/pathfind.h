
/*                                          __
 *         ____      __    ___ ___   _____ /\_\  _ __    __    ____
 *        /\_ ,`\  /'__`\/' __` __`\/\ '__`\/\ \/\`'__\/'__`\ /',__\
 *        \/_/  /_/\  __//\ \/\ \/\ \ \ \L\ \ \ \ \ \//\  __//\__, `\
 *          /\____\ \____\ \_\ \_\ \_\ \ ,__/\ \_\ \_\\ \____\/\____/
 *          \/____/\/____/\/_/\/_/\/_/\ \ \/  \/_/\/_/ \/____/\/___/
 *                                     \ \_\
 *                                      \/_/
 *
 *      Open source reimplementation of the Genie engine.
 *      By Andre Caceres Carrilho
 *
 *      Engine pathfinding system (mostly a low-level A* implementation)
 *      Credits: Patrick Lester for the article "A* Pathfinding for Beginners"
 *
 *      See license.md for copyright information.
 */

#ifndef GN_PATHFIND_H
#define GN_PATHFIND_H

#include "common.h"

#ifdef GN_PATH_LIMIT
# define PF_LIMIT_LIST      1024 /* Max cells on FILO stacks (open, closed) */
# define PF_LIMIT_PATH      256  /* Max path cells */
# define PF_LIMIT_QUEUE     2048 /* Max pathfind jobs in queue */
# define PF_LIMIT_FRAME     64   /* Max pathfind calls per frame */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GN_PATHDIST_COUNT     4
typedef struct {
  int16_t     x, y;
  int32_t     parent;
  /*
    * Compiler trick: here we declare four int16 variables instead of just
    * three. The compiler would do this anyway to align the struct on the
    * memory in order to speedup IO timings.
    */
  int16_t     dist[GN_PATHDIST_COUNT];
} Gn_PathNode;

/*
 * For a thread safe code we group all recurrent data for the A-star
 * algorithm in this structure.
 */
typedef struct {
  /*
   * Grid dimensions and bitset (flags) for every cell that can be used
   * during the path computation (walkable, visited, closed).
   */
  int16_t     rows, cols;
  Gn_Byte*    p_flag;

  /*
   * Grid cells stack. This actually holds the memory adressed by the open
   * and closed (pointers) lists below. The trick is that a cell is never
   * on both lists, and rows*cols is the maximum number of cells that can be.
   */
  Gn_PathNode*  p_cell;
  int32_t     n_cell;

  /*
   * First In Last Out (FILO) stacks (Open and Closed lists)
   * We only store the pointers to the cell stack in order to reduce
   * assignment operations and reduce memory usage
   */
  Gn_PathNode   **p_open, **p_closed;
  int32_t       n_open,   n_closed;
} Gn_PathContext;

/*
 * A pathfind job to be processed in queue. This structure just hold the
 * pointers to the input and output data, which will be allocated in the
 * actor (that is going to walk through the path) structure.
 */
typedef struct {
  /* Input (start and target position) */
  Gn_Point*   p_start;
  Gn_Point*   p_end;
  /* Output (Computed path) */
  Gn_Point*   p_path;
  int32_t*    n_size;
} Gn_PathJob;

/*
 * This is the path-finding system used by the engine, will encapsulate
 * everything related to the algorithm (context) and is going to handle
 * the (process) job queue.
 */
typedef struct {
  Gn_PathContext*  p_ctx;
  /* First In First Out (FIFO) stack */
  Gn_PathJob* p_entries;
  int32_t     n_start;
  int32_t     n_end;
} Gn_PathSystem;

int Gn_FindPath (Gn_PathContext *ctx, Gn_Point *path, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
Gn_PathContext* Gn_CreatePathContext (int16_t nrows, int16_t ncols);
void Gn_DestroyPathContext (Gn_PathContext* ctx);

#ifdef __cplusplus
}
#endif
#endif /* GN_PATHFIND_H */
