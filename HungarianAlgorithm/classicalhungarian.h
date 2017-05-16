#ifndef CLASSICALHUNGARIAN_H
#define CLASSICALHUNGARIAN_H

#include <memory>
#include <vector>
#include "common.h"
#include <limits>

class ClassicalHungarian
{

public:
    Array<double> C_orig;
    Array<int> M;
    enum MunkresFunc
    {
        Min = 0,
        Max
    };

private:
    Array<double> C;
    Array<int> path;
    std::vector<int> RowCover;
    std::vector<int> ColCover;
    int nrow;
    int ncol;
    int path_count = 0;
    int path_row_0;
    int path_col_0;
    int step;

public:
    ClassicalHungarian();
    //Return value represent quality of assigment, or -1 if something gone wrong
    double RunMunkres();

    std::vector<double> GetSingleCosts();

    void SetCostMatrix(const Array<double>& CostMatrix, MunkresFunc func);

private:
    void step_one();

    //Find a zero (Z) in the resulting matrix.  If there is no starred
    //zero in its row or column, star Z. Repeat for each element in the
    //matrix. Go to Step 3.
    void step_two();

    //Cover each column containing a starred zero.  If K columns are covered,
    //the starred zeros describe a complete set of unique assignments.  In this
    //case, Go to DONE, otherwise, Go to Step 4.
    void step_three();

    //methods to support step 4
    void find_a_zero(std::shared_ptr<int> row, std::shared_ptr<int> col);

    bool star_in_row(int row);

    void find_star_in_row(int row, std::shared_ptr<int> col);

    //Find a noncovered zero and prime it.  If there is no starred zero
    //in the row containing this primed zero, Go to Step 5.  Otherwise,
    //cover this row and uncover the column containing the starred zero.
    //Continue in this manner until there are no uncovered zeros left.
    //Save the smallest uncovered value and Go to Step 6.
    void step_four();

    // methods to support step 5
    void find_star_in_col(int c, std::shared_ptr<int> r);

    void find_prime_in_row(int r, std::shared_ptr<int> c);

    void augment_path();

    void clear_covers();

    void erase_primes();

    //Construct a series of alternating primed and starred zeros as follows.
    //Let Z0 represent the uncovered primed zero found in Step 4.  Let Z1 denote
    //the starred zero in the column of Z0 (if any). Let Z2 denote the primed zero
    //in the row of Z1 (there will always be one).  Continue until the series
    //terminates at a primed zero that has no starred zero in its column.
    //Unstar each starred zero of the series, star each primed zero of the series,
    //erase all primes and uncover every line in the matrix.  Return to Step 3.
    void step_five();

    //methods to support step 6
    void find_smallest(std::shared_ptr<double> minval);

    //Add the value found in Step 4 to every element of each covered row, and subtract
    //it from every element of each uncovered column.  Return to Step 4 without
    //altering any stars, primes, or covered lines.
    void step_six();

    double step_seven();


};

#endif // CLASSICALHUNGARIAN_H
