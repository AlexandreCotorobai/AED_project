//
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//   cc -Wall -O2 -D_use_zlib_=0 solution_speed_run.c -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 solution_speed_run.c -lm -lz
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//

//
// static configuration
//

#define _max_road_size_ 800 // the maximum problem size
#define _min_road_speed_ 2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_ 9  // must not be larger than 9 (only because of the PDF figure)
#define _solucao1_
//#define _solucao2_
//#define _solucao3_
//#define _solucao4_

//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include "../P02/elapsed_time.h"
#include "make_custom_pdf.c"

//
// road stuff
//

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

static void init_road_speeds(void)
{
  double speed;
  int i;

  for (i = 0; i <= _max_road_size_; i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if (max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if (max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}

//
// description of a solution
//

typedef struct
{
  int n_moves;                        // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_]; // the positions (the first one must be zero)
} solution_t;

//
// the (very inefficient) recursive solution given to the students
//

static solution_t solution_1, solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_1_recursion(int move_number, int position, int speed, int final_position)
{
  int i, new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if (position == final_position && speed == 1)
  {
    // is it a better solution?
    if (move_number < solution_1_best.n_moves)
    {
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for (new_speed = speed - 1; new_speed <= speed + 1; new_speed++)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
        solution_1_recursion(move_number + 1, position + new_speed, new_speed, final_position);
    }
}

static void solve_1(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_1_recursion(0, 0, 0, final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}

//
// example of the slides
//

static void example(void)
{
  int i, final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_1(final_position);
  make_custom_pdf_file("example.pdf", final_position, &max_road_speed[0], solution_1_best.n_moves, &solution_1_best.positions[0], solution_1_elapsed_time, solution_1_count, "Plain recursion");
  printf("mad road speeds:");
  for (i = 0; i <= final_position; i++)
    printf(" %d", max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for (i = 0; i <= solution_1_best.n_moves; i++)
    printf(" %d", solution_1_best.positions[i]);
  printf("\n");
}

// ---------------------------------------------------------------------------------------------------------------
// OUR SOLUTION --------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------

static solution_t solution_2, solution_2_best;
static double solution_2_elapsed_time; // time it took to solve the problem
static unsigned long solution_2_count; // effort dispended solving the problem

static void solution_2_new(int move_number, int position, int speed, int final_position)
{
  // explicar que esta solucao deixou de ser uma arvore ternaria mas sim apenas uma lista, sendo mais eficaz
  int new_speed = 0;
  int value;
  int validIncrementation;

  solution_2.positions[move_number] = position;

  // printf("%d %d %d \n", move_number, position, speed);

  if (position == final_position && speed == 1)
  {
    solution_2_best = solution_2;
    solution_2_best.n_moves = move_number;
    return;
  }

  for (value = 1; value >= -1; value--)
  {
    int newskip = 0;
    int oldskip = 0;
    int s;
    validIncrementation = 1;

    solution_2_count++;

    for (s = speed + value; s > 0; s--)
    {
      oldskip = newskip;
      newskip = s + newskip;
      for (int p = position + oldskip; p <= position + newskip; p++)
      {

        if (p > final_position || s > max_road_speed[p])
        {
          validIncrementation = 0;
          break;
        }
      }
      if (validIncrementation == 0)
        break;
    }
    if (validIncrementation == 1)
    {
      new_speed = speed + value;
      break;
    }
  }

  solution_2_new(move_number + 1, position + new_speed, new_speed, final_position);
}

static void solve_2(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_2_elapsed_time = cpu_time();
  solution_2_count = 0ul;
  solution_2_best.n_moves = final_position + 100;
  solution_2_new(0, 0, 0, final_position);
  solution_2_elapsed_time = cpu_time() - solution_2_elapsed_time;
}

// ---------------------------------------------------------------------------------------------------------------
// Teacher corrected solution ------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------

static solution_t solution_3, solution_3_best;
static double solution_3_elapsed_time; // time it took to solve the problem
static unsigned long solution_3_count; // effort dispended solving the problem
// static speed_compare speedArr, speedArr_best; // array of speeds

static void solution_3_recursion(int move_number, int position, int speed, int final_position)
{
  int i, new_speed;

  // record move
  solution_3_count++;
  solution_3.positions[move_number] = position;

  // is it a solution?
  if (position == final_position && speed == 1)
  {
    // is it a better solution?
    if (move_number < solution_3_best.n_moves)
    {
      solution_3_best = solution_3;
      solution_3_best.n_moves = move_number;
    }
    return;
  }

  if (solution_3_best.n_moves != final_position + 100)
  {
    // printf("fAFAF %d\n", solution_1_best.n_moves);
    return;
  }

  // no, try all legal speeds
  for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
      {
        if (move_number < solution_3_best.n_moves)
        {
          solution_3_recursion(move_number + 1, position + new_speed, new_speed, final_position);
        }
      }
    }
}

static void solve_3(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_3_elapsed_time = cpu_time();
  solution_3_count = 0ul;
  solution_3_best.n_moves = final_position + 100;
  solution_3_recursion(0, 0, 0, final_position);
  solution_3_elapsed_time = cpu_time() - solution_3_elapsed_time;
}

// ---------------------------------------------------------------------------------------------------------------
// OUR SOLUTION IN DYNAMIC PROGRAMING ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------

static solution_t solution_4, solution_4_best, solution_4_copy;
static double solution_4_elapsed_time; // time it took to solve the problem
static unsigned long solution_4_count; // effort dispended solving the problem
int save = 0;
int saveSpeed = 0;
int savePosition = 0;

static void solution_4_new(int move_number, int position, int speed, int final_position)
{
  // explicar que esta solucao deixou de ser uma arvore ternaria mas sim apenas uma lista, sendo mais eficaz
  int new_speed = 0;
  int value, validIncrementation;

  solution_4.positions[move_number] = position;

  if (position == final_position && speed == 1)
  {
    solution_4_best = solution_4;
    solution_4_best.n_moves = move_number;
    return;
  }

  for (value = 1; value >= -1; value--)
  {
    int newskip = 0;
    int oldskip = 0;
    int s;
    validIncrementation = 1;

    solution_4_count++;

    for (s = speed + value; s > 0; s--)
    {
      oldskip = newskip;
      newskip = s + newskip;
      for (int p = position + oldskip; p <= position + newskip; p++)
      {
        if (p > final_position && save == 0)
        {
          solution_4_copy = solution_4;
          solution_4_copy.n_moves = move_number;
          saveSpeed = speed;
          // solution_4_count = 0;
          savePosition = position;

          save = 1;
        }
        if (p > final_position || s > max_road_speed[p])
        {
          validIncrementation = 0;
          break;
        }
      }
      if (validIncrementation == 0)
        break;
    }
    if (validIncrementation == 1)
    {
      new_speed = speed + value;
      break;
    }
  }

  solution_4_new(move_number + 1, position + new_speed, new_speed, final_position);
}

static void solve_4(int final_position)
{
  int speed = 0;
  int position = 0;
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_4_elapsed_time = cpu_time();
  solution_4_count = 0ul;
  solution_4_best.n_moves = final_position + 100;
  if (save == 1)
  {
    solution_4 = solution_4_copy;
    solution_4_count = 0;
  }
  save = 0;
  solution_4_new(solution_4.n_moves, savePosition, saveSpeed, final_position);
  solution_4_elapsed_time = cpu_time() - solution_4_elapsed_time;
}

// ---------------------------------------------------------------------------------------------------------------
// TEACHERS SOLUTION IN DYNAMIC PROGRAMING -----------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------
static solution_t solution_5, solution_5_best, solution_5_copy;
static double solution_5_elapsed_time; // time it took to solve the problem
static unsigned long solution_5_count; // effort dispended solving the problem
static int table[_max_road_size_ + 1][3];

static void solution_5_recursion(int move_number, int position, int speed, int final_position)
{

  int i, new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if (position == final_position && speed == 1)
  {
    // is it a better solution?
    if (move_number < solution_1_best.n_moves)
    {
      solution_2_best = solution_2;
      solution_2_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
        solution_5_recursion(move_number + 1, position + new_speed, new_speed, final_position);
    }
}

static void solve_5(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_5_elapsed_time = cpu_time();
  solution_5_count = 0ul;
  solution_5_best.n_moves = final_position + 100;
  solution_5_recursion(0, 0, 0, final_position);
  solution_5_elapsed_time = cpu_time() - solution_5_elapsed_time;
}

//
// main program
//

int main(int argc, char *argv[argc + 1])
{
#define _time_limit_ 3600.0
  int n_mec, final_position, print_this_one;
  char file_name[64];

  // generate the example data
  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
  {
    example();
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;
  solution_1_elapsed_time = 0.0;
  printf("    + --- ---------------- --------- +\n");
  printf("    |                plain recursion |\n");
  printf("--- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- +\n");

  FILE *fptr;

  while (final_position <= _max_road_size_ /* && final_position <= 20*/)
  {
    print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |", final_position);

// first solution method (very bad)
#ifdef _solucao1_
    char *solution_name = "Teachers_solution";
    if (solution_1_elapsed_time < _time_limit_)
    {
      solve_1(final_position);
      if (print_this_one != 0)
      {
        sprintf(file_name, "%d_%03d_1.pdf", n_mec, final_position);
        make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_1_best.n_moves, &solution_1_best.positions[0], solution_1_elapsed_time, solution_1_count, "Plain recursion");
      }
      printf(" %3d %16lu %9.3e |", solution_1_best.n_moves, solution_1_count, solution_1_elapsed_time);
    }
    else
    {
      solution_1_best.n_moves = -1;
      printf("                                |");
    }
#endif
// second solution method (less bad) (EXCELENT!)
#ifdef _solucao2_
    char *solution_name = "Our_solution";
    if (solution_2_elapsed_time < _time_limit_)
    {
      solve_2(final_position);
      if (print_this_one != 0)
      {
        sprintf(file_name, "%d_%03d_2.pdf", n_mec, final_position);
        make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_2_best.n_moves, &solution_2_best.positions[0], solution_2_elapsed_time, solution_2_count, "Our Solution");
      }
      printf(" %3d %16lu %9.3e |", solution_2_best.n_moves, solution_2_count, solution_2_elapsed_time);
    }
    else
    {
      solution_2_best.n_moves = -1;
      printf("                                |");
    }
#endif
// third solution method (teacher solucion improvement)
#ifdef _solucao3_
    char *solution_name = "Teachers_solved";
    if (solution_3_elapsed_time < _time_limit_)
    {
      solve_3(final_position);
      if (print_this_one != 0)
      {
        sprintf(file_name, "%d_%03d_3.pdf", n_mec, final_position);
        make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_3_best.n_moves, &solution_3_best.positions[0], solution_3_elapsed_time, solution_3_count, "Teacher corrected solution");
      }
      printf(" %3d %16lu %9.3e |", solution_3_best.n_moves, solution_3_count, solution_3_elapsed_time);
    }
    else
    {
      solution_3_best.n_moves = -1;
      printf("                                |");
    }
#endif
// fourth solution method (ou solution in dynamic programing)
#ifdef _solucao4_
    char *solution_name = "Our_Dynamic";
    if (solution_4_elapsed_time < _time_limit_)
    {
      solve_4(final_position);
      if (print_this_one != 0)
      {
        sprintf(file_name, "%d_%03d_4.pdf", n_mec, final_position);
        make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_4_best.n_moves, &solution_4_best.positions[0], solution_4_elapsed_time, solution_4_count, "Our Solution in Dynamic Programing");
      }
      printf(" %3d %16lu %9.3e |", solution_4_best.n_moves, solution_4_count, solution_4_elapsed_time);
    }
    else
    {
      solution_4_best.n_moves = -1;
      printf("                                |");
    }
#endif

    // Writing results on file

    sprintf(file_name, "%s_%d_results.txt", solution_name, n_mec);
    fptr = fopen(file_name, "a");
    if (fptr == NULL)
    {
      printf("Error!");
      exit(1);
    }
    fprintf(fptr, "%d,%d,%d,%9.3e\n", final_position, solution_1_best.n_moves, solution_1_count, solution_1_elapsed_time);

    // done
    printf("\n");
    fflush(stdout);
    // new final_position
    if (final_position < 50)
      final_position += 1;
    else if (final_position < 100)
      final_position += 5;
    else if (final_position < 200)
      final_position += 10;
    else
      final_position += 20;
    fclose(fptr);
  }
  printf("--- + --- ---------------- --------- +\n");
  
  return 0;
#undef _time_limit_
}