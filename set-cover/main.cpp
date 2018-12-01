#include <climits>
#include <iostream>
#include <vector>

size_t calc_cost(size_t* costs, size_t m, std::vector<size_t> subset)
{
    size_t total_cost = 0;
    for (size_t i = 0; i < subset.size(); i++)
    {
        total_cost += costs[subset[i] - 1];
    }

    return total_cost;
}

bool is_solution(std::vector<std::vector<size_t>> G, size_t n, size_t m,
                 std::vector<size_t> subset)
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

void gen_subsets(std::vector<std::vector<size_t>> G, size_t n, size_t m,
                 size_t* costs)
{
    std::vector<size_t> subset, min_subset;
    size_t min_cost = ULLONG_MAX;

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

int main()
{
    size_t n, m, element;
    char c;

    scanf("%zu %zu", &n, &m);
    std::vector<std::vector<size_t>> G;
    size_t* costs = (size_t*)malloc(m * sizeof(size_t));
    std::vector<size_t> v;

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

    gen_subsets(G, n, m, costs);

    return 0;
}