#include "classicalhungarian.h"

ClassicalHungarian::ClassicalHungarian()
{
    step = 1;
}

//For each row of the cost matrix, find the smallest element and subtract
//it from every element in its row.  When finished, Go to Step 2.
void ClassicalHungarian::step_one()
{
    double min_in_row;

    for (int r = 0; r < nrow; r++)
    {
        min_in_row = C[r][0];
        for (int c = 0; c < ncol; c++)
            if (C[r][c] < min_in_row)
                min_in_row = C[r][c];
        for (int c = 0; c < ncol; c++)
            C[r][c] -= min_in_row;
    }
    step = 2;
}

//Find a zero (Z) in the resulting matrix.  If there is no starred
//zero in its row or column, star Z. Repeat for each element in the
//matrix. Go to Step 3.
void ClassicalHungarian::step_two()
{
    for (int r = 0; r < nrow; r++)
        for (int c = 0; c < ncol; c++)
        {
            if (C[r][c] == 0 && RowCover[r] == 0 && ColCover[c] == 0)
            {
                M[r][c] = 1;
                RowCover[r] = 1;
                ColCover[c] = 1;
            }
        }
    for (int r = 0; r < nrow; r++)
        RowCover[r] = 0;
    for (int c = 0; c < ncol; c++)
        ColCover[c] = 0;
    step = 3;
}

//Cover each column containing a starred zero.  If K columns are covered,
//the starred zeros describe a complete set of unique assignments.  In this
//case, Go to DONE, otherwise, Go to Step 4.
void ClassicalHungarian::step_three()
{
    int colcount;
    for (int r = 0; r < nrow; r++)
        for (int c = 0; c < ncol; c++)
            if (M[r][c] == 1)
                ColCover[c] = 1;

    colcount = 0;
    for (int c = 0; c < ncol; c++)
        if (ColCover[c] == 1)
            colcount += 1;
    if (colcount >= ncol || colcount >= nrow)
        step = 7;
    else
        step = 4;
}

//methods to support step 4
void ClassicalHungarian::find_a_zero(std::shared_ptr<int> row, std::shared_ptr<int> col)
{
    int r = 0;
    int c;
    bool done;
    *row = -1;
    *col = -1;
    done = false;
    while (!done)
    {
        c = 0;
        while (true)
        {
            if (C[r][c] == 0 && RowCover[r] == 0 && ColCover[c] == 0)
            {
                *row = r;
                *col = c;
                done = true;
            }
            c += 1;
            if (c >= ncol || done)
                break;
        }
        r += 1;
        if (r >= nrow)
            done = true;
    }
}

bool ClassicalHungarian::star_in_row(int row)
{
    bool tmp = false;
    for (int c = 0; c < ncol; c++)
        if (M[row][c] == 1)
            tmp = true;
    return tmp;
}

void ClassicalHungarian::find_star_in_row(int row, std::shared_ptr<int> col)
{
    *col = -1;
    for (int c = 0; c < ncol; c++)
        if (M[row][c] == 1)
            *col = c;
}

//Find a noncovered zero and prime it.  If there is no starred zero
//in the row containing this primed zero, Go to Step 5.  Otherwise,
//cover this row and uncover the column containing the starred zero.
//Continue in this manner until there are no uncovered zeros left.
//Save the smallest uncovered value and Go to Step 6.
void ClassicalHungarian::step_four()
{
    std::shared_ptr<int> row = std::make_shared<int>(-1);
    std::shared_ptr<int> col = std::make_shared<int>(-1);
    bool done;

    done = false;
    while (!done)
    {
        find_a_zero(row, col);
        if (*row == -1)
        {
            done = true;
            step = 6;
        }
        else
        {
            M[*row][*col] = 2;
            if (star_in_row(*row))
            {
                find_star_in_row(*row, col);
                RowCover[*row] = 1;
                ColCover[*col] = 0;
            }
            else
            {
                done = true;
                step = 5;
                path_row_0 = *row;
                path_col_0 = *col;
            }
        }
    }
}

// methods to support step 5
void ClassicalHungarian::find_star_in_col(int c, std::shared_ptr<int> r)
{
    *r = -1;
    for (int i = 0; i < nrow; i++)
        if (M[i][c] == 1)
            *r = i;
}

void ClassicalHungarian::find_prime_in_row(int r, std::shared_ptr<int> c)
{
    for (int j = 0; j < ncol; j++)
        if (M[r][j] == 2)
            *c = j;
}

void ClassicalHungarian::augment_path()
{
    for (int p = 0; p < path_count; p++)
        if (M[path[p][0]][path[p][1]] == 1)
            M[path[p][0]][path[p][1]] = 0;
        else
            M[path[p][0]][path[p][1]] = 1;
}

void ClassicalHungarian::clear_covers()
{
    for (int r = 0; r < nrow; r++)
        RowCover[r] = 0;
    for (int c = 0; c < ncol; c++)
        ColCover[c] = 0;
}

void ClassicalHungarian::erase_primes()
{
    for (int r = 0; r < nrow; r++)
        for (int c = 0; c < ncol; c++)
            if (M[r][c] == 2)
                M[r][c] = 0;
}


//Construct a series of alternating primed and starred zeros as follows.
//Let Z0 represent the uncovered primed zero found in Step 4.  Let Z1 denote
//the starred zero in the column of Z0 (if any). Let Z2 denote the primed zero
//in the row of Z1 (there will always be one).  Continue until the series
//terminates at a primed zero that has no starred zero in its column.
//Unstar each starred zero of the series, star each primed zero of the series,
//erase all primes and uncover every line in the matrix.  Return to Step 3.
void ClassicalHungarian::step_five()
{
    bool done;
    std::shared_ptr<int> r = std::make_shared<int>(-1);
    std::shared_ptr<int> c = std::make_shared<int>(-1);

    path_count = 1;
    path[path_count - 1][0] = path_row_0;
    path[path_count - 1][1] = path_col_0;
    done = false;
    while (!done)
    {
        find_star_in_col(path[path_count - 1][1], r);
        if (*r > -1)
        {
            path_count += 1;
            path[path_count - 1][0] = *r;
            path[path_count - 1][1] = path[path_count - 2][1];
        }
        else
            done = true;
        if (!done)
        {
            find_prime_in_row(path[path_count - 1][0], c);
            path_count += 1;
            path[path_count - 1][0] = path[path_count - 2][0];
            path[path_count - 1][1] = *c;
        }
    }
    augment_path();
    clear_covers();
    erase_primes();
    step = 3;
}

//methods to support step 6
void ClassicalHungarian::find_smallest(std::shared_ptr<double> minval)
{
    for (int r = 0; r < nrow; r++)
        for (int c = 0; c < ncol; c++)
            if (RowCover[r] == 0 && ColCover[c] == 0)
                if (*minval > C[r][c])
                    *minval = C[r][c];
}

//Add the value found in Step 4 to every element of each covered row, and subtract
//it from every element of each uncovered column.  Return to Step 4 without
//altering any stars, primes, or covered lines.
void ClassicalHungarian::step_six()
{
    std::shared_ptr<double> minval = std::make_shared<double>(std::numeric_limits<double>::max());
    find_smallest(minval);
    for (int r = 0; r < nrow; r++)
        for (int c = 0; c < ncol; c++)
        {
            if (RowCover[r] == 1)
                C[r][c] += *minval;
            if (ColCover[c] == 0)
                C[r][c] -= *minval;
        }
    step = 4;
}

double ClassicalHungarian::RunMunkres()
{
    double RetVal = -1;

    bool done = false;
    step = 1;
    while (!done)
    {
        switch (step)
        {
            case 1:
                step_one();
                break;
            case 2:
                step_two();
                break;
            case 3:
                step_three();
                break;
            case 4:
                step_four();
                break;
            case 5:
                step_five();
                break;
            case 6:
                step_six();
                break;
            case 7:
                RetVal = step_seven();
                done = true;
                break;
        }
    }

    return RetVal;
}

double ClassicalHungarian::step_seven()
{
    double RetVal = 0;

    for (int r = 0; r < nrow; ++r)
    {
        for (int c = 0; c < ncol; ++c)
        {
            if (M[r][c] == 1)
            {
                RetVal += C_orig[r][c];
            }
        }
    }

    return RetVal;
}

std::vector<double> ClassicalHungarian::GetSingleCosts()
{
    std::vector<double> RetVal;

    for (int r = 0; r < nrow; ++r)
    {
        for (int c = 0; c < ncol; ++c)
        {
            if (M[r][c] == 1)
            {
                RetVal.push_back(C_orig[r][c]);
            }
        }
    }

    return RetVal;
}

void ClassicalHungarian::SetCostMatrix(const Array<double>& CostMatrix, ClassicalHungarian::MunkresFunc func)
{
    C.Initialize(CostMatrix);
    C_orig.Initialize(CostMatrix);
    nrow = C.size(0);
    ncol = C.size(1);
    M.Initialize(nrow, ncol, 0);
    path.Initialize((nrow * 2) + 1, 2, 0);
    RowCover = std::vector<int>(nrow);
    ColCover = std::vector<int>(ncol);
    step = 1;

    if (func == ClassicalHungarian::Max)
    {
        double max = C[0][0];

        for (int r = 0; r < nrow; ++r)
        {
            for (int c = 0; c < ncol; ++c)
            {
                if (C[r][c] > max)
                {
                    max = C[r][c];
                }
            }
        }

        for (int r = 0; r < nrow; ++r)
        {
            for (int c = 0; c < ncol; ++c)
            {
                C[r][c] = max - C[r][c];
            }
        }
    }
}
