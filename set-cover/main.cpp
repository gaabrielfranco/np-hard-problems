#include <algorithm>
#include <chrono>
#include <climits>
#include <cstring>
#include <iostream>
#include <set>
#include <vector>

typedef std::chrono::high_resolution_clock Clock;

/*
    Calculates the cost of a MCC solution
    Params:
        - G: graph
        - n: number of elements
        - m: number of subsets
        - subset: MCC solution
*/

size_t calc_cost(size_t* costs, size_t m, std::vector<size_t>& subset)
{
    size_t total_cost = 0;
    for (size_t i = 0; i < subset.size(); i++)
    {
        total_cost += costs[subset[i] - 1];
    }

    return total_cost;
}

/*
    Checks if a subset is a MCC solution
    Params:
        - G: graph
        - n: number of elements
        - m: number of subsets
        - subset: possible MCC solution
*/

bool is_solution(std::vector<std::vector<size_t>>& G, size_t n, size_t m,
                 std::vector<size_t>& subset)
{
    bool visited[n] = {false};

    for (size_t i = 0; i < subset.size(); i++)
    {
        for (size_t j = 0; j < G[subset[i]].size(); j++)
        {
            visited[G[subset[i]][j] - 1] = true;
        }
    }

    for (size_t i = 0; i < n; i++)
    {
        if (!visited[i])
        {
            return false;
        }
    }

    return true;
}

/*
    MCC solver using a brute force algorithm
    Params:
        - G: graph
        - n: number of elements
        - m: number of subsets
        - costs: array of costs of each subset
*/

void brute_foce_solver(std::vector<std::vector<size_t>>& G, size_t n, size_t m,
                       size_t* costs)
{
    std::vector<size_t> subset, min_subset;
    size_t min_cost = ULLONG_MAX;

    // Generates all subsets
    for (size_t i = 0; i < (1 << m); i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if ((i & (1 << j)) != 0)
            {
                subset.push_back(j);
            }
        }
        if (!subset.empty())
        {
            if (is_solution(G, n, m, subset))
            {
                size_t cost = calc_cost(costs, m, subset);
                if (cost < min_cost)
                {
                    min_cost = cost;
                    min_subset = subset;
                }
            }
            subset.clear();
        }
    }

    std::cout << "Solution:\n{ ";
    for (size_t i = 0; i < min_subset.size(); i++)
    {
        std::cout << min_subset[i] + 1 << " ";
    }
    std::cout << "} with cost = ";
    std::cout << min_cost << std::endl;
}

/*
    MCC solver using a greedy algorithm
    Params:
        - G_reverse: graph
        - n: number of elements
        - m: number of subsets
        - costs: array of costs of each subset
*/

void greedy_solver(std::vector<std::vector<size_t>>& G,
                   std::vector<std::vector<size_t>>& G_reverse, size_t n,
                   size_t m, size_t* costs)
{
    std::set<size_t> greedy_sol;
    std::vector<size_t> v;

    for (size_t i = 0; i < n; i++)
    {
        G_reverse.push_back(v);
    }

    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < G[i].size(); j++)
        {
            if (G_reverse[G[i][j] - 1].empty())
            {
                // Putting the subset
                G_reverse[G[i][j] - 1].push_back(i);
            }
            else
            {
                bool was_inserted = false;

                // Putting the subset ordered by cost
                for (size_t k = 0; k < G_reverse[G[i][j] - 1].size(); k++)
                {
                    if (costs[i] <= costs[G_reverse[G[i][j] - 1][k]])
                    {
                        G_reverse[G[i][j] - 1].insert(
                            G_reverse[G[i][j] - 1].begin() + k, i);
                        was_inserted = true;
                        break;
                    }
                }

                if (!was_inserted)
                {
                    G_reverse[G[i][j] - 1].push_back(i);
                }
            }
        }
    }

    for (size_t i = 0; i < n; i++)
    {
        greedy_sol.insert(G_reverse[i][0]);
    }

    size_t min_cost = 0;

    std::cout << "\nUsing Greedy\n";
    std::cout << "Solution:\n{ ";
    for (auto i = greedy_sol.begin(); i != greedy_sol.end(); i++)
    {
        std::cout << *i + 1 << " ";
        min_cost += costs[*i];
    }
    std::cout << "} with cost = ";
    std::cout << min_cost << std::endl;
}

int main(int argc, char** argv)
{
    size_t n, m, element;
    char c;
    std::vector<std::vector<size_t>> G;
    std::vector<std::vector<size_t>> G_reverse;
    size_t* costs = (size_t*)malloc(m * sizeof(size_t));
    std::vector<size_t> v;

    if (argc == 1 ||
        (argc == 2 && (strcmp(argv[1], "-bf") && strcmp(argv[1], "-gr") &&
                       strcmp(argv[1], "-all"))))
    {
        // std::cout << argv[0] << std::endl;
        // std::cout << argv[1] << std::endl;
        std::cout
            << "-bf: Executes the brute force algorithm\n-gr: Executes the "
               "greedy algorithm\n-all: Executes both algorithms\n";
        return 0;
    }

    // Input

    scanf("%zu %zu", &n, &m);

    for (size_t i = 0; i < m; i++)
    {
        while (scanf("%zu%c", &element, &c))
        {
            v.push_back(element);

            if (c == '\n')
            {
                G.push_back(v);
                v.clear();
                break;
            }
        }
    }

    for (size_t i = 0; i < m; i++)
    {
        scanf("%zu", &costs[i]);
    }

    if (!strcmp(argv[1], "-bf") || !strcmp(argv[1], "-all"))
    {
        // Brute force solution
        std::cout << "\nUsing Brute Force\n";
        auto start = Clock::now();
        brute_foce_solver(G, n, m, costs);
        auto end = Clock::now();
        std::cout << "Brute Force duration: ";
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                         end - start)
                         .count();
        std::cout << "ms\n";
    }

    if (!strcmp(argv[1], "-gr") || !strcmp(argv[1], "-all"))
    {
        // Greedy solution
        auto start = Clock::now();
        greedy_solver(G, G_reverse, n, m, costs);
        auto end = Clock::now();
        std::cout << "Greedy duration: ";
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                         end - start)
                         .count();
        std::cout << "ms\n";
    }
    return 0;
}