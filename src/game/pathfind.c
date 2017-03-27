
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

/*
 * History
 *
 * 03-aug-15  Created: Initial code design (oriented, C++) and implementations
 *            it wasn't working correctly because of the bug described below
 *
 * 19-fev-17  Bugfix: Wasn't checking if reached target cell, the algorithm
 *            kept running on the entire grid and didn't get the correct path
 *
 * 20-fev-17  Boost: Pointer usage improvement. Instead of copying the cells
 *            attributes from the open to the closed list is better to use a stack
 *
 * 20-fev-17  Boost: Bitset for cell flags. Using only one bit for each flag of
 *            the cell (visited, closed, walkable). Code ported from C++ to C
 *
 * 26-fev-17  Boost: Removed unecessary computations of heuristic distances on
 *            already visited cells. The distance to the target cell don't change
 *
 * 27-fev-17  Boost: inline Assembly SSE code (only works on GCC compiler) to
 *            reverse the path at the end of the search
 *
 * 02-mar-17  Feature: Code design for the pathfinding system
 */

/* TODO: inrange check to avoid segfault */

#include "pathfind.h"

#define PFDIST_F        0
#define PFDIST_G        1
#define PFDIST_H        2

#define PFBIT_WALK      0   /* cell walkable*/
#define PFBIT_WENT      1   /* cell visited */
#define PFBIT_MARK      2   /* cell closed */

#define PFBIT_CH1(x, y)     x |= 1 << y         /* change y-th bit to 1 */
#define PFBIT_CH0(x, y)     x &= ~(1 << y)      /* change y-th bit to 0 */
#define PFBIT_CHK(x, y)    ((x >> y) & 1)       /* check the y-th bit */

/* http://stackoverflow.com/questions/664852/ */
#if GN_HEURISTIC == GN_HEURISTIC_MANHATTAN
# define PF_ABS(x)       \
    mask = x >> 31;     \
    x ^= mask;          \
    x += mask & 1;
#endif

/*
 * Gn_ResetPathContext ()
 * Clears the visited and closed bits (flags) and lists used on the path-
 * finding function. Necessary at the start of every call.
 */
static void
Gn_ResetPathContext (Gn_PathContext *ctx)
{
  int16_t i, j;
  Gn_Byte *ptr = ctx->p_flag;
  for (i = 0; i < ctx->rows; ++i, ptr += ctx->cols)
    for (j = 0; j < ctx->cols; ++j)
      {
        /* Todo: I think some SSE code would easily fit here! */
        /* Todo: I also think this loops can be simplified a little */
        PFBIT_CH0 (ptr[j], PFBIT_WENT);
        PFBIT_CH0 (ptr[j], PFBIT_MARK);
      }
  ctx->n_cell = 0;
  ctx->n_open = 0;
  ctx->n_closed = 0;
}

/*
 * Gn_AdjustOpenList ()
 * This function reposition the i-th element of the open list based on its F
 * distance. The list should be in decreasing order.
 */
static void
Gn_AdjustOpenList (Gn_PathNode **arr, int32_t n, int32_t i)
{
  if (n-- <= 1)
    return;
  Gn_PathNode *x = arr[i];
  while (i > 0 && x->dist[PFDIST_F] > arr[i-1]->dist[PFDIST_F])
    {
      arr[i] = arr[i-1];
      i--;
    }
  while (i < n && x->dist[PFDIST_F] < arr[i+1]->dist[PFDIST_F])
    {
      arr[i] = arr[i+1];
      i++;
    }
  arr[i] = x;
}

/*
 * Gn_ReversePath ()
 * At the end of A-star algorithm the path array is stored from the target cell
 * up to the starting cell, therefore, we need to reverse the path array order.
 * Here we only swap the pointers to increase the performance.
 */
static void
Gn_ReversePath (Gn_Point *path, int32_t n)
{
  Gn_Point **sp, **ep, *tp;
#ifndef GN_HAVE_SSE
  sp = (Gn_Point**) path;
  ep = (Gn_Point**) path + n-1;
#else
  /*
   * Benchmarking without optimization flags "-O2" (for non-SSE code)
   * showed that this version is around 2.16 times faster... Todo: try
   * aligned memory and "movaps" instead
   */
  sp = (Gn_Point**) path;
  ep = (Gn_Point**) path + n-GN_POINTER_SIZE;
#endif
  n >>= 1;
#ifdef GN_HAVE_SSE
  while (n >= GN_POINTER_SIZE)
    {
      asm volatile (
        "movups (%0), %%xmm0;"          /* xmm0 = *sp */
        "movups (%1), %%xmm1;"          /* xmm1 = *ep */
        "shufps $0x1b, %%xmm0, %%xmm0;" /* reverse order inside register */
        "shufps $0x1b, %%xmm1, %%xmm1;" /* mask = 00 01 10 11 = 0x1b */
        "movups %%xmm1, (%0);"          /* *sp = xmm1 */
        "movups %%xmm0, (%1);"          /* *ep = xmm0 */
        : : "r" (sp), "r" (ep) );
      sp += GN_POINTER_SIZE;
      ep -= GN_POINTER_SIZE;
      n  -= GN_POINTER_SIZE;
    }
  /*
   * For the remaining elements we need to correct the end pointer
   * We dont check if there is any remaining elements to avoid branch
   * is just faster this way
   */
  ep += GN_POINTER_SIZE-1;
#endif
  while (n--)
    {
      tp = *sp;
      *sp++ = *ep;
      *ep-- = tp;
    }
}

/*
 * Gn_FindPath ()
 * Performs the A-star algorithm between the current position and the goal
 * (target) point. Returns the path size or negative values if wanst able to
 * find it. The code assumes that the path pointer was previously allocated.
 */
int
Gn_FindPath (Gn_PathContext *ctx, Gn_Point *path,
  int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  int32_t n_path = 0; /* final path size */
  int16_t /* auxiliar variables list: */
  xc, yc, /* c1: current open list cell position  */
  xp, yp, /* c2: current neighbouring (in relation to c1) cell position */
  dx, dy, /* deltas from c2 to the target cell */
  i, j, k, w; /* loops iterators */
#if GN_HEURISTIC == GN_HEURISTIC_MANHATTAN
  int16_t mask;   /* Mask for integer absolute value computation */
#endif
  Gn_PathNode *rpt; /* pointer to iterate the closed list and retrieve the path */
  Gn_Byte *bst; /* pointer to the current neighbouring cell (c2) flags */

  /* Clear the visited and closed flags for the new run */
  Gn_ResetPathContext (ctx);

  /* Add the starting cell to the open list */
  ctx->n_open = 1;
  ctx->p_open[0] = &ctx->p_cell[0];
  ctx->p_open[0]->parent = 0;
  ctx->p_open[0]->x = x1;
  ctx->p_open[0]->y = y1;
  ctx->p_open[0]->dist[PFDIST_F] = 0;
  ctx->p_open[0]->dist[PFDIST_G] = 0;
  ctx->p_open[0]->dist[PFDIST_H] = 0;

  while (ctx->n_open)
    {
      /*
       * Switch the best candidate from the open list to the closed list
       * as the open list is sorted in decreasing order of F distances
       * we can just take the last element from it
       */
      ctx->p_closed[ctx->n_closed] = ctx->p_open[ --ctx->n_open ];

      /* Making a copy of the current (x,y) coordinates into c1 */
      xc = ctx->p_closed[ ctx->n_closed ]->x;
      yc = ctx->p_closed[ ctx->n_closed ]->y;

      /*
       * Update closed list size and check against the limits imposed
       * (if defined) This is to ensure that the algorithm wont cause
       * slowdown on real-time applications
       */
      ctx->n_closed++;
#if defined GN_PATH_LIMIT
      if (ctx->n_closed > PF_LIMIT_LIST)
        return GN_FAILURE;
#endif

      /* Checking if we found the target cell, and finish if it is */
      if (xc == x2 && yc == y2)
        break;

      /* Iterate through the nine neighbouring cells ... */
      for (i = -1; i <= 1; ++i)
        {
          for (j = -1; j <= 1; ++j)
            {
              /* .. and discard the diagonals and center (current) cells */
              if ( (i && j) || (!i && !j) )
                continue;

              /*
               * c2 (xp,yp) store the coordinates of current neighbouring cell
               * which are computed as c1 (xc,yc) plus the iterators indexes
               */
              xp = xc + j;
              yp = yc + i;

              /* direct pointer to this cell flags to facilitate the access */
              bst = &ctx->p_flag[ yp * ctx->cols + xp ];

              /*
               * Discard this cell if it is not walkable
               * or was added to the closed list
               */
              if (PFBIT_CHK (*bst, PFBIT_MARK) || PFBIT_CHK (*bst, PFBIT_WALK))
                continue;

              /*
               * If this cell was already visited we have to find
               * the index in open list and reevaluate its values
               */
              if (PFBIT_CHK (*bst, PFBIT_WENT) )
                {
                  /*
                   * Little trick: Searching from the back of the list
                   * is almost twice as fast as the other way around
                   */
                  for (k = 0, w = ctx->n_open-1; w >= 0; --w)
                    if (ctx->p_open[w]->x == xp  &&  ctx->p_open[w]->y == yp)
                      {
                        k = w;   /* No need to look further */
                        break;
                      }

                  /*
                   * Reset the parent of this cell (c2) to the current one (c1),
                   * and recomputing the travelled distance up to this cell
                   */
                  ctx->p_open[k]->parent = ctx->n_closed-1;
                  ctx->p_open[k]->dist[PFDIST_G] =
                    ctx->p_closed[ ctx->n_closed-1 ]->dist[PFDIST_G] +1;

                  /*
                   * The heuristic distance from this position to the
                   * target cell was already computed and did not changed since
                   * the position remains the same, so... F = G+H
                   */
                  ctx->p_open[k]->dist[PFDIST_F] =
                    ctx->p_open[k]->dist[PFDIST_G] +
                    ctx->p_open[k]->dist[PFDIST_H];

                  /*
                   * We need to reposition this cell on the open list
                   * because the fitness was changed
                   */
                  Gn_AdjustOpenList (ctx->p_open, ctx->n_open, k);
                }
              else
                {
                  /*
                   * If we didnt visited this cell yet we mark as visited
                   * and grab a new place holder from the memory stack
                   */
                  PFBIT_CH1 (*bst, PFBIT_WENT);

                  ctx->p_open[ctx->n_open] = &ctx->p_cell[ ctx->n_cell++ ];

                  /* Storing the c2 coordinates */
                  ctx->p_open[ctx->n_open]->x = xp;
                  ctx->p_open[ctx->n_open]->y = yp;

                  /*
                   * Set c1 (now the last cell on closed list) as parent of c2
                   * and computes the geometric distance from the starting cell
                   */
                  ctx->p_open[ctx->n_open]->parent = ctx->n_closed-1;
                  ctx->p_open[ctx->n_open]->dist[PFDIST_G] =
                    ctx->p_closed[ ctx->n_closed-1 ]->dist[PFDIST_G] +1;

                  /*
                   * Computing the deltas from c2 to the target cell depending
                   * on the heuristc selected a different calc will be performed
                   * to get the distance
                   */
                  dx = xp - x2;
                  dy = yp - y2;

#if GN_HEURISTIC == GN_HEURISTIC_EUCLIDIAN
                  ctx->p_open[ctx->n_open]->dist[PFDIST_H] = sqrtf (dx*dx + dy*dy);
#elif GN_HEURISTIC == GN_HEURISTIC_MANHATTAN
                  PF_ABS (dx)
                  PF_ABS (dy)
                  ctx->p_open[ctx->n_open]->dist[PFDIST_H] = dx + dy;
#endif

                  /* Compute the F-score of this cell F=G+H */
                  ctx->p_open[ctx->n_open]->dist[PFDIST_F] =
                    ctx->p_open[ctx->n_open]->dist[PFDIST_G] +
                    ctx->p_open[ctx->n_open]->dist[PFDIST_H];

                  /*
                   * Update open list size and check against the limits imposed
                   * (if defined) This is to ensure that the algorithm wont
                   * cause slowdown on real-time applications
                   */
                  ctx->n_open++;

#if defined GN_PATH_LIMIT
                  if (ctx->n_open > PF_LIMIT_LIST)
                    return GN_FAILURE;
#endif

                  /*
                   * Insert this cell in the right position on the open list
                   * based on its fitness score (F)
                   */
                  Gn_AdjustOpenList (ctx->p_open, ctx->n_open, ctx->n_open-1);
                }
            }
        }
    } /* !open list */

  /* Iterating the closed list and retrieving the path at the output pointer */
  rpt = ctx->p_closed[ ctx->n_closed-1 ];
  while (rpt->parent)
    {
      /*
       * Go to the next cell on the closed list
       * Store the position and update the path size
       */
      rpt = ctx->p_closed[ rpt->parent ];

      path[n_path].x = rpt->x;
      path[n_path].y = rpt->y;

      n_path++;

#if defined GN_PATH_LIMIT
      if (n_path >= PF_LIMIT_PATH)
        return GN_FAILURE;
#endif
    }
  Gn_ReversePath (path, n_path);
  return n_path;
}

/*
 * Gn_CreatePathContext ()
 *
 */
Gn_PathContext*
Gn_CreatePathContext (int16_t nrows, int16_t ncols)
{
  int32_t n = nrows * ncols;
  if (nrows < 1 || ncols < 1)
    return GN_NULL;

  Gn_PathContext *ctx = (Gn_PathContext*) malloc (sizeof (Gn_PathContext));
  if (!ctx)
    return GN_NULL;

  ctx->rows = nrows;
  ctx->cols = ncols;

  ctx->p_flag = (Gn_Byte*) malloc (n * sizeof (Gn_Byte));
  ctx->p_cell = (Gn_PathNode*)  malloc (n * sizeof (Gn_PathNode));
  ctx->p_open = (Gn_PathNode**) malloc (n * sizeof (Gn_PathNode*));
  ctx->p_closed = (Gn_PathNode**) malloc (n * sizeof (Gn_PathNode*));

  if (!ctx->p_flag || !ctx->p_open || !ctx->p_closed)
    return GN_NULL;

  ctx->n_cell = GN_NULL;
  ctx->n_open = GN_NULL;
  ctx->n_closed = GN_NULL;
  return ctx;
}

/*
 * Gn_DestroyPathContext ()
 *
 */
void
Gn_DestroyPathContext (Gn_PathContext* ctx)
{
  if (!ctx)
    return;
  if (ctx->p_flag && ctx->p_cell && ctx->p_open && ctx->p_closed)
    {
      free (ctx->p_flag);
      free (ctx->p_cell);
      free (ctx->p_open);
      free (ctx->p_closed);
    }
  free (ctx);
  ctx = GN_NULL;
}
